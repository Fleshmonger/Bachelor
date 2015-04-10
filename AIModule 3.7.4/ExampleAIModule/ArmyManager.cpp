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
			Duty duty = assignments[unit];
			assignments.erase(unit);
			enlisted[duty].erase(unit);

			end = army.end();
		}
		else
			it++;
	}
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
		assignments[unit] = DUTY_IDLE;
		enlisted[DUTY_IDLE].insert(unit);
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
		Duty duty = assignments[unit];
		assignments.erase(unit);
		enlisted[duty].erase(unit);
	}
}


// Assigns the unit a duty.
void ArmyManager::assignUnit(BWAPI::Unit * unit, Duty duty)
{
	// Verify unit.
	if (contains(unit))
	{
		// Remove unit from current duty.
		Duty oldDuty = assignments[unit];
		enlisted[oldDuty].erase(unit);

		// Add unit to new duty.
		assignments[unit] = duty;
		enlisted[duty].insert(unit);
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


// Returns a copy of units enlisted with the specified duty.
utilUnit::UnitSet ArmyManager::getEnlisted(Duty duty)
{
	return enlisted[duty];
}