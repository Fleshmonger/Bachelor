#include "Accountant.h"


// Constructor
Accountant::Accountant() :
	allocatedMinerals(0),
	allocatedGas(0),
	allocatedSupply(0),
	schedule()
{
}


// Deconstructor
Accountant::~Accountant()
{
}


// Allocates the costs of building a given unit type.
void Accountant::allocate(BWAPI::UnitType unitType)
{
	allocatedMinerals += unitType.mineralPrice();
	allocatedGas += unitType.gasPrice();
	allocatedSupply += unitType.supplyRequired();
}


// Deallocates the cost of a given unit type.
void Accountant::deallocate(BWAPI::UnitType unitType)
{
	allocatedMinerals -= unitType.mineralPrice();
	allocatedGas -= unitType.gasPrice();
	allocatedSupply -= unitType.supplyRequired();
}


// Adds a unit type to the schedule.
void Accountant::addSchedule(BWAPI::UnitType unitType)
{
	if (isScheduled(unitType))
		schedule[unitType]++;
	else
		schedule[unitType]++;
}


// Removes a unit type from the schedule.
void Accountant::removeSchedule(BWAPI::UnitType unitType)
{
	if (scheduled(unitType) > 1)
		schedule[unitType]--;
	else
		schedule.erase(unitType);
}


// Returns whether we can afford to build a given unit type.
bool Accountant::isAffordable(BWAPI::UnitType unitType)
{
	BWAPI::Player * self = BWAPI::Broodwar->self();
	return
		unitType.mineralPrice() <= self->minerals() - allocatedMinerals &&
		unitType.gasPrice() <= self->gas() - allocatedGas &&
		unitType.supplyRequired() <= self->supplyTotal() - self->supplyUsed() - allocatedSupply;
	return false;
}


// Returns whether the unit type is scheduled.
bool Accountant::isScheduled(BWAPI::UnitType unitType)
{
	return schedule.count(unitType) > 0 && schedule[unitType] > 0;
}


// Returns the amount of scheduled units of the type.
unsigned int Accountant::scheduled(BWAPI::UnitType unitType)
{
	if (isScheduled(unitType))
		return schedule[unitType];
	else
		return 0;
}


// Returns the amount of unallocated minerals.
int Accountant::minerals()
{
	return BWAPI::Broodwar->self()->minerals() - allocatedMinerals;
}


// Returns the amount of unallocated gas.
int Accountant::gas()
{
	return BWAPI::Broodwar->self()->gas() - allocatedGas;
}


// Returns the amount of unallocated supply.
int Accountant::supply()
{
	return BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() - allocatedSupply;
}