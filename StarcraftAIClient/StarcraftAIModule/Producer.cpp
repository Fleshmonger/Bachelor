#include "Producer.h"

Producer::Producer()
{
	infantryFacilities = new BWAPI::Unitset();
}

// Unused deconstructor
Producer::~Producer()
{
}

// Attempt to produce an infantry unit at an available factory.
// Returns whether or not the attempt was succesful.
bool Producer::orderInfantry(BWAPI::UnitType unitType) // Note: the return is currently unused
{
	BWAPI::Unit facility = getIdleFacility(infantryFacilities);
	if (facility &&
		facility->isCompleted() &&
		canAfford(unitType))
	{
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

// Returns an idle structure in a list of structures. Returns a nullptr if no such structure exists.
BWAPI::Unit Producer::getIdleFacility(BWAPI::Unitset * facilities)
{
	for (auto &f : *facilities)
	{
		if (f->isIdle())
			return f;
	}
	return nullptr;
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
}

// Undesignates an infantry constructing facility.
void Producer::removeInfantryFacility(BWAPI::Unit facility)
{
	infantryFacilities->erase(facility);
}

// Designates the current worker producing facility.
void Producer::setDepot(BWAPI::Unit depot)
{
	this->depot = depot;
}