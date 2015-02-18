#include "Producer.h"

Producer::Producer()
{
	infantryFacilities = new BWAPI::Unitset();
	idleInfantryFacilities = new BWAPI::Unitset();
}

// Unused deconstructor
Producer::~Producer()
{
}

// Attempt to produce an infantry unit at an available factory.
// Returns whether or not the attempt was succesful.
bool Producer::orderInfantry(BWAPI::UnitType unitType) // Note: the return is currently unused
{
	if (canAfford(unitType) && !idleInfantryFacilities->empty())
	{
		BWAPI::Unit facility = *idleInfantryFacilities->begin();
		idleInfantryFacilities->erase(idleInfantryFacilities->begin());
		//BWAPI::Unit facility = *idleInfantryFacilities->erase(idleInfantryFacilities->begin());
		facility->train(unitType);
		return true;
	}
	else
		return false;
}

// Attempts to produce a worker at the designated depot.
// Returns whether or not the attempt was succesful.
bool Producer::orderWorker() // Note: the return is currently unused
{
	BWAPI::UnitType workerType = Broodwar->self()->getRace().getWorker();
	if (depot &&
		depot->isIdle() &&
		depot->isCompleted() &&
		canAfford(workerType))
	{
		depot->train(workerType);
		return true;
	}
	else
		return false;
}

bool Producer::canAfford(BWAPI::UnitType unitType)
{
	BWAPI::PlayerInterface * player = Broodwar->self();
	return player->minerals() >= unitType.mineralPrice() &&
		player->gas() >= unitType.gasPrice() &&
		player->supplyTotal() - player->supplyUsed() >= unitType.supplyRequired();
}

// Returns the amount of designated infantry constructing facilities.
int Producer::totalInfantryFacilities()
{
	return infantryFacilities->size();
}

// Designates an infantry constructing facility.
void Producer::addInfantryFacility(BWAPI::Unit facility)
{
	infantryFacilities->insert(facility);
	idleInfantryFacilities->insert(facility);
}

// Undesignates an infantry constructing facility.
void Producer::removeInfantryFacility(BWAPI::Unit facility)
{
	infantryFacilities->erase(facility);
	idleInfantryFacilities->erase(facility);
}

// Designates the current worker producing facility.
void Producer::setDepot(BWAPI::Unit depot)
{
	this->depot = depot;
}

void Producer::update()
{
	// Find all idle infantry facilities.
	auto &it = infantryFacilities->begin();
	while (it != infantryFacilities->end())
	{
		BWAPI::Unit facility = *it;
		if (facility && facility->exists())
		{
			if (facility->isCompleted() && // This should be unecessary
				facility->isIdle())
				idleInfantryFacilities->insert(facility);
			++it;
		}
		else
			it = infantryFacilities->erase(it);
	}
}