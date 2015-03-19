#include "ArmyManager.h"

// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect) :
	archivist(archivist),
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	troops(std::set<BWAPI::Unit*>()),
	attackers(std::set<BWAPI::Unit*>()),
	idle(std::set<BWAPI::Unit*>()),
	invaders(std::map<BWAPI::Unit*, int>()),
	defenders(std::map<BWAPI::Unit*, BWAPI::Unit*>())
{
}

// Deconstructor
ArmyManager::~ArmyManager()
{
}

// Calculates the total hitpoints and shields a set of units has.
int ArmyManager::toughness(std::set<BWAPI::Unit*> units)
{
	int toughness = 0;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
	{
		// TODO Simplify this or merge the method.
		BWAPI::UnitType unitType;
		if (unit->getPlayer() == Broodwar->self())
			unitType = unit->getType();
		else
			unitType = archivist->getType(unit);
		toughness += unitType.maxHitPoints() + unitType.maxShields();
	}
	return toughness;
}

// Calculates the total damage a set of units deal per time unit.
double ArmyManager::damage(std::set<BWAPI::Unit*> units)
{
	double damage = 0;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
	{
		// TODO Simplify this or merge the method.
		BWAPI::UnitType unitType;
		if (unit->getPlayer() == Broodwar->self())
			unitType = unit->getType();
		else
			unitType = archivist->getType(unit);
		BWAPI::WeaponType weapon = unitType.groundWeapon();
		if (weapon != BWAPI::WeaponTypes::None)
			damage += ((double) weapon.damageAmount()) / (double) weapon.damageCooldown();
	}
	return damage;
}

// Calculates the time-to-kill for the attackers versus defenders.
double ArmyManager::TTK(std::set<BWAPI::Unit*> attackers, std::set<BWAPI::Unit*> defenders)
{
	int toughness = this->toughness(defenders);
	double damage = this->damage(attackers);
	if (damage <= 0)
		return -1;
	else
		return toughness / damage;
}

// Adds a unit to the troop pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	troops.insert(unit);
	idle.insert(unit);
}

// Removes a unit from the troop pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	troops.insert(unit);
	attackers.erase(unit);
	idle.erase(unit);
}

// Simulate the army manager AI, ordering, creating and upgrading troops.
void ArmyManager::update()
{
	// Initialize.
	std::set<BWAPI::Unit*> enemyBuildings = archivist->getBuildings();

	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);

	// Add new invaders.
	std::set<BWAPI::Unit*> newInvaders = archivist->invaders();
	BOOST_FOREACH(BWAPI::Unit * invader, newInvaders)
		if (invaders.count(invader) == 0)
			invaders[invader] = 0;

	// Validate and intercept invaders.
	{
		std::map<BWAPI::Unit*, int>::iterator it = invaders.begin();
		while (it != invaders.end())
		{
			BWAPI::Unit * invader = (*it).first;
			++it;
			// Validate.
			if (invader &&
				(!invader->isVisible() || invader->exists()) &&
				archivist->getPosition(invader))
			{
				// Determine amount of needed defenders.
				int requiredDefenders;
				if (invader->getType().isWorker())
					requiredDefenders = DEFENDERS_PER_SCOUT;
				else
					requiredDefenders = DEFENDERS_PER_ATTACKER;

				// Attempt to assign required defenders.
				while (invaders[invader] < requiredDefenders)
				{
					BWAPI::Unit * defender = workerManager->takeWorker();
					if (defender &&
						defender->exists())
					{
						defenders[defender] = invader;
						++invaders[invader];
					}
					else
						break;
				}
			}
			else
			{
				// Clear invalid invader.
				invaders.erase(invader);
			}
		}
	}

	// Command defenders.
	{
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = defenders.begin();
		while (it != defenders.end())
		{
			BWAPI::Unit * defender = (*it).first;
			BWAPI::Unit * invader = (*it).second;
			++it;
			// Validate.
			if (defender &&
				defender->exists() &&
				invaders.count(invader) > 0)
			{
				// Intercept invader.
				defender->attack(archivist->getPosition(invader));
			}
			else
			{
				// Relieve invalid defender.
				defenders.erase(defender);
				if (defender &&
					defender->exists() &&
					defender->getType().isWorker())
					workerManager->addWorker(defender);
				--invaders[invader];
			}
		}
	}

	// Calculate attack.
	if (canAttack())
		attackers.insert(idle.begin(), idle.end());

	// Command attackers.
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackTarget = archivist->getPosition(*enemyBuildings.begin());
		std::set<BWAPI::Unit*>::iterator it = attackers.begin();
		while (it != attackers.end())
		{
			BWAPI::Unit * attacker = *it;
			++it;
			if (attacker &&
				attacker->exists())
			{
				if (attacker->isIdle())
					attacker->attack(attackTarget);
			}
			else
				removeUnit(attacker);
		}
	}
}

// Returns whether or not we would win an attack.
// TODO Does not account for losing troops during combat.
// TODO Does not account for enemy upgrades.
// TODO Does not account for current enemy unit stats.
// TODO Does not account for armor.
// TODO Does not account for distance and range.
// TODO Does not account for maneuvering into attacking range.
// TODO Does not account for damage types.
// TODO Does not account for splash damage.
// TODO Does not account for abilities.
bool ArmyManager::canAttack()
{
	std::set<BWAPI::Unit*> enemies = archivist->getTroops(), enemyTurrets = archivist->getTurrets();
	enemies.insert(enemyTurrets.begin(), enemyTurrets.end());
	return
		!troops.empty() &&
		(enemies.empty() || TTK(troops, enemies) < TTK(enemies, troops));
}

// TEMPORARY used for testing.
std::set<BWAPI::Unit*> ArmyManager::getTroops()
{
	return troops;
}