#include "ArmyManager.h"


// Constructor
ArmyManager::ArmyManager(Archivist * archivist, CombatJudge * combatJudge, WorkerManager * workerManager, Producer * producer, Architect * architect) :
	archivist(archivist),
	combatJudge(combatJudge),
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	army(utilUnit::UnitSet()),
	attackers(utilUnit::UnitSet()),
	defenders(utilUnit::UnitSet()),
	idle(utilUnit::UnitSet()),
	invaders(utilUnit::UnitSet())
{
}


// Deconstructor
ArmyManager::~ArmyManager()
{
}


// Adds a unit to the troop pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	army.insert(unit);
	idle.insert(unit);
}


// Removes a unit from the troop pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	army.erase(unit);
	attackers.erase(unit);
	idle.erase(unit);
}


// Simulate the army manager AI, ordering, creating and upgrading troops.
// TODO Simplify defense
void ArmyManager::update()
{
	// Initialize.
	utilUnit::UnitSet enemyBuildings = archivist->getBuildings();

	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);

	// Validate invaders.
	{
		utilUnit::UnitSet::iterator it = invaders.begin();
		while (it != invaders.end())
		{
			BWAPI::Unit * invader = *it;
			++it;
			if (invader ||
				(invader->isVisible() && !invader->exists()) ||
				BWTA::getRegion(archivist->getPosition(invader)) != archivist->getHomeRegion())
				invaders.erase(invader);
		}
	}

	// Identify new invaders.
	utilUnit::UnitSet newInvaders = archivist->invaders();
	invaders.insert(newInvaders.begin(), newInvaders.end());

	// Invasion check.
	if (!invaders.empty())
	{
		// Conscript troops.
		defenders.insert(idle.begin(), idle.end());

		// Conscript workers if necessary.
		double defenseStrength = combatJudge->strength(defenders), invaderStrength = combatJudge->strength(invaders);
		while (defenseStrength < invaderStrength)
		{
			BWAPI::Unit * worker = workerManager->takeWorker();
			if (worker &&
				worker->exists())
			{
				defenseStrength += combatJudge->strength(worker);
				defenders.insert(worker);
			}
			else
				break;
		}

		BWAPI::Unit * target = *invaders.begin();
		// Command defense
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
		{
			if (target->isVisible())
				utilUnit::command(defender, BWAPI::UnitCommandTypes::Attack_Unit, target);
			else
				utilUnit::command(defender, BWAPI::UnitCommandTypes::Attack_Move, archivist->getPosition(target));
		}
	}
	else
	{
		// Release defenders.
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
		{
			if (defender &&
				defender->exists())
			{
				if (defender->getType().isWorker())
					workerManager->addWorker(defender);
				else
					idle.insert(defender);
			}
		}
		defenders.clear();
	}

	// Calculate attack.
	if (canAttack())
		attackers.insert(idle.begin(), idle.end());

	// Command attackers.
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackLocation = archivist->getPosition(*enemyBuildings.begin());
		utilUnit::UnitSet::iterator it = attackers.begin();
		while (it != attackers.end())
		{
			BWAPI::Unit * attacker = *it;
			++it;
			if (attacker &&
				attacker->exists())
			{
				if (attacker->isIdle())
					utilUnit::command(attacker, BWAPI::UnitCommandTypes::Attack_Move, attackLocation);
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
	utilUnit::UnitSet enemies = archivist->getTroops(), enemyTurrets = archivist->getTurrets();
	enemies.insert(enemyTurrets.begin(), enemyTurrets.end());
	return combatJudge->strength(army) > combatJudge->strength(enemies);
}

// Returns a copy of the army.
utilUnit::UnitSet ArmyManager::getArmy()
{
	return army;
}