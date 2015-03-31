#include "ArmyManager.h"


// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager) :
	archivist(archivist),
	workerManager(workerManager),
	combatJudge(archivist)
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
	if (attackers.count(unit) > 0)
		attackers.erase(unit);
	else
		idle.erase(unit);
}


// Simulate the army manager AI, ordering, creating and upgrading troops.
// TODO Simplify defense
void ArmyManager::update()
{
	// Invasion check.
	utilUnit::UnitSet invaders = archivist->invaders();
	if (!invaders.empty())
	{
		// Conscript troops.
		defenders.insert(idle.begin(), idle.end());

		// Conscript workers if necessary.
		double defenseStrength = combatJudge.strength(defenders), invaderStrength = combatJudge.strength(invaders);
		while (defenseStrength < invaderStrength)
		{
			BWAPI::Unit * worker = workerManager->takeWorker();
			if (worker &&
				worker->exists())
			{
				defenseStrength += combatJudge.strength(worker);
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
	utilUnit::UnitSet enemyBuildings = archivist->getBuildings();
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
	return combatJudge.strength(army) > combatJudge.strength(archivist->getTroops()) + combatJudge.strength(archivist->getTurrets());
}

// Returns a copy of the army.
utilUnit::UnitSet ArmyManager::getArmy()
{
	return army;
}