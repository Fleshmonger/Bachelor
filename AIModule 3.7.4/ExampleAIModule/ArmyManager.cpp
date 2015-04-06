#include "ArmyManager.h"


// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge) :
	archivist(archivist),
	workerManager(workerManager),
	combatJudge(combatJudge)
{
}


// Deconstructor
ArmyManager::~ArmyManager()
{
}


// Adds a unit to the army pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists())
	{
		// Add unit.
		army.insert(unit);
		assignments[unit] = IDLE;
		enlisted[IDLE].insert(unit);
	}
}


// Removes a unit from the army pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit)
	{
		// Remove unit.
		army.erase(unit);
		Task task = assignments[unit];
		assignments.erase(unit);
		enlisted[task].erase(unit);
	}
}

void ArmyManager::assignUnit(BWAPI::Unit * unit, Task task)
{
	// Remove unit from old task.
	Task oldTask = assignments[unit];
	enlisted[oldTask].erase(unit);

	// Add unit to new task.
	assignments[unit] = task;
	enlisted[task].insert(unit);
}

// Simulate the army manager AI, ordering, creating and upgrading troops.
// TODO Simplify defense
void ArmyManager::update()
{
	// Verify army.
	// TODO Is this necessary?
	utilUnit::UnitSet::iterator it = army.begin();
	while (it != army.end())
	{
		BWAPI::Unit * unit = *it;
		if (!unit->exists())
		{
			it = army.erase(it);
			Task task = assignments[unit];
			assignments.erase(unit);
			enlisted[task].erase(unit);
		}
		else
			it++;
	}

	/*
	// Invasion check.
	utilUnit::UnitSet invaders = archivist->invaders();
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
	}
	else
	{
		// Conscript attackers.
		attackers.insert(idle.begin(), idle.end());

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
	*/
}


// Returns a copy of the army and their assignments.
utilUnit::UnitSet ArmyManager::getArmy()
{
	return army;
}


// Returns a copy of units enlisted with the specified task.
utilUnit::UnitSet ArmyManager::getEnlisted(Task task)
{
	return enlisted[task];
}