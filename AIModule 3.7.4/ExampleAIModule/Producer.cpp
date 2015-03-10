#include "Producer.h"

Producer::Producer(Accountant * accountant)
{
	// Managers
	this->accountant = accountant;
	// Local
	depot = NULL;
	incompleteUnits = new std::set<Unit*>();
	infantryFacilities = new std::set<Unit*>();
	idleInfantryFacilities = new std::set<Unit*>();
}

// Unused deconstructor
Producer::~Producer()
{
}

// Attempt to produce an infantry unit at an available facility.
// Returns true if it succeeds and false otherwise.
bool Producer::trainUnit(BWAPI::UnitType unitType)
{
	if (!unitType.isBuilding())
	{
		// Find the appropriate facility.
		BWAPI::Unit * facility = NULL;
		if (accountant->isAffordable(unitType))
		{
			if (unitType.isWorker())
			{
				if (depot &&
					depot->isIdle() &&
					depot->isCompleted())
					facility = depot;
			}
			else if (!idleInfantryFacilities->empty()) // Assume it is infantry
			{
				facility = *idleInfantryFacilities->begin();
				idleInfantryFacilities->erase(facility);
			}
		}
		// Train the unit at the found facility (if any).
		if (facility)
		{
			facility->train(unitType);
			accountant->allocUnit(unitType);
			return true;
		}
	}
	return false;
}

// Add a unit in production to the incomplete pool.
void Producer::incompleteUnit(BWAPI::Unit * unit)
{
	incompleteUnits->insert(unit);
	accountant->deallocUnit(unit->getType());
}

// Remove a complete unit from the incomplete pool.
void Producer::completeUnit(BWAPI::Unit * unit)
{
	incompleteUnits->erase(unit);
}

/*
// Returns the amount of designated infantry constructing facilities.
// Move to some base building controller (Architect?) which monitors structures.
int Producer::totalInfantryFacilities()
{
return infantryFacilities->size();
}
*/

// Designates an infantry constructing facility.
void Producer::addInfantryFacility(BWAPI::Unit * facility)
{
	infantryFacilities->insert(facility);
	idleInfantryFacilities->insert(facility);
}

// Undesignates an infantry constructing facility.
void Producer::removeInfantryFacility(BWAPI::Unit * facility)
{
	infantryFacilities->erase(facility);
	idleInfantryFacilities->erase(facility);
}

// Designates the current worker producing facility.
void Producer::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}

// Simulates the Producer AI.
// Updates the current pool of idle infantry facilities.
void Producer::update()
{
	// Remove invalid unit constructions
	std::set<Unit*>::iterator it = incompleteUnits->begin();
	while (it != incompleteUnits->end())
	{
		BWAPI::Unit * unit = *it;
		if (unit->exists() && unit->isBeingConstructed())
			++it;
		else
			it = incompleteUnits->erase(it);
	}
	// Find all idle infantry facilities and remove invalid ones.
	// TODO: Search only non-idle facilities.
	it = infantryFacilities->begin();
	while (it != infantryFacilities->end())
	{
		BWAPI::Unit * facility = *it;
		if (facility && facility->exists())
		{
			if (facility->isCompleted() && // This should be unecessary
				facility->isIdle())
				idleInfantryFacilities->insert(facility);
			++it;
		}
		else
		{
			it = infantryFacilities->erase(it);
			idleInfantryFacilities->erase(facility);
		}

	}
}