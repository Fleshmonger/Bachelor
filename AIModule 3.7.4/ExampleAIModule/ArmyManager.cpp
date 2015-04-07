#include "ArmyManager.h"


// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge) :
	archivist(archivist),
	workerManager(workerManager),
	combatJudge(combatJudge),
	army(),
	assignments(),
	enlisted()
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
		unit->exists() &&
		utilUnit::isOwned(unit))
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
	if (contains(unit))
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
	// Verify unit.
	if (contains(unit))
	{
		// Remove unit from old task.
		Task oldTask = assignments[unit];
		enlisted[oldTask].erase(unit);

		// Add unit to new task.
		assignments[unit] = task;
		enlisted[task].insert(unit);
	}
}

// Simulate the army manager AI, ordering, creating and upgrading troops.
void ArmyManager::update()
{
	// Verify army.
	// TODO Is this necessary?
	utilUnit::UnitSet::iterator it = army.begin(), end = army.end();
	while (it != end)
	{
		// Verify unit.
		BWAPI::Unit * unit = *it;
		if (!unit->exists())
		{
			// Remove unit.
			it = army.erase(it);
			Task task = assignments[unit];
			assignments.erase(unit);
			enlisted[task].erase(unit);

			end = army.end();
		}
		else
			it++;
	}
}


// Returns whether the unit is in the army pool.
bool ArmyManager::contains(BWAPI::Unit * unit)
{
	return army.count(unit) > 0;
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