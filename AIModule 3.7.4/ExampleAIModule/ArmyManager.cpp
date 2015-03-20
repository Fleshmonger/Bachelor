#include "ArmyManager.h"

// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect) :
	archivist(archivist),
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	armyStrength(0),
	army(std::set<BWAPI::Unit*>()),
	attackers(std::set<BWAPI::Unit*>()),
	idle(std::set<BWAPI::Unit*>()),
	invaderDefense(std::map<BWAPI::Unit*, int>()),
	defenderTargets(std::map<BWAPI::Unit*, BWAPI::Unit*>())
{
}

// Deconstructor
ArmyManager::~ArmyManager()
{
}

// Returns the interpreted strength of unit as a value.
// TODO Does not account for losing troops during combat.
// TODO Does not account for enemy upgrades.
// TODO Does not account for current enemy unit stats.
// TODO Does not account for armor.
// TODO Does not account for distance and range.
// TODO Does not account for maneuvering into attacking range.
// TODO Does not account for damage types.
// TODO Does not account for splash damage.
// TODO Does not account for abilities.
double ArmyManager::strength(BWAPI::Unit * unit)
{
	if (unit && unit->exists())
	{
		BWAPI::UnitType unitType = archivist->getType(unit);
		BWAPI::WeaponType weaponType = unitType.groundWeapon();
		if (weaponType != BWAPI::WeaponTypes::None)
			return (double)(weaponType.damageAmount() * (unitType.maxHitPoints() + unitType.maxShields())) / weaponType.damageCooldown();
	}
	return 0;
}

// Returns the interpreted strength of an army as a value.
double ArmyManager::strength(std::set<BWAPI::Unit*> units)
{
	double unitsStrength = 0;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		unitsStrength += strength(unit);
	return unitsStrength;
}

/*
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
*/

// Adds a unit to the troop pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	army.insert(unit);
	idle.insert(unit);
	armyStrength += strength(unit);
}

// Removes a unit from the troop pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	army.insert(unit);
	attackers.erase(unit);
	idle.erase(unit);
	armyStrength -= strength(unit);
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
		if (invaderDefense.count(invader) == 0)
			invaderDefense[invader] = 0;

	// Validate and intercept invaders.
	{
		std::map<BWAPI::Unit*, int>::iterator it = invaderDefense.begin();
		while (it != invaderDefense.end())
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
				while (invaderDefense[invader] < requiredDefenders)
				{
					BWAPI::Unit * defender = workerManager->takeWorker();
					if (defender &&
						defender->exists())
					{
						defenderTargets[defender] = invader;
						++invaderDefense[invader];
					}
					else
						break;
				}
			}
			else
			{
				// Clear invalid invader.
				invaderDefense.erase(invader);
			}
		}
	}

	// Command defenders.
	{
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = defenderTargets.begin();
		while (it != defenderTargets.end())
		{
			BWAPI::Unit * defender = (*it).first;
			BWAPI::Unit * invader = (*it).second;
			++it;
			// Validate.
			if (defender &&
				defender->exists() &&
				invaderDefense.count(invader) > 0)
			{
				// Intercept invader.
				defender->attack(archivist->getPosition(invader));
			}
			else
			{
				// Relieve invalid defender.
				defenderTargets.erase(defender);
				if (defender &&
					defender->exists())
				{
					if (defender->getType().isWorker())
						workerManager->addWorker(defender);
					else
						addUnit(defender);
				}
				--invaderDefense[invader];
			}
		}
	}

	// Calculate attack.
	if (canAttack())
		attackers.insert(idle.begin(), idle.end());

	// Command attackers.
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackLocation = archivist->getPosition(*enemyBuildings.begin());
		std::set<BWAPI::Unit*>::iterator it = attackers.begin();
		while (it != attackers.end())
		{
			BWAPI::Unit * attacker = *it;
			++it;
			if (attacker &&
				attacker->exists())
			{
				if (attacker->isIdle())
					utilUnit::orderUnit(attacker, BWAPI::UnitCommandTypes::Attack_Move, attackLocation);
					//attacker->attack(attackTarget);
			}
			else
				removeUnit(attacker);
		}
	}
}

// Returns whether or not we would win an attack based on army strength.
bool ArmyManager::canAttack()
{
	std::set<BWAPI::Unit*> enemies = archivist->getTroops(), enemyTurrets = archivist->getTurrets();
	enemies.insert(enemyTurrets.begin(), enemyTurrets.end());
	return armyStrength > strength(enemies);
}

// TEMPORARY used for testing.
std::set<BWAPI::Unit*> ArmyManager::getArmy()
{
	return army;
}