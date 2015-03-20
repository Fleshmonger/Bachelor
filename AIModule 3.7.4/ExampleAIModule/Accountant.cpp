#include "Accountant.h"

// Constructor
Accountant::Accountant() :
	allocatedMinerals(0),
	allocatedGas(0),
	allocatedSupply(0)
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

// Returns whether we can afford to build a given unit type.
bool Accountant::isAffordable(BWAPI::UnitType unitType)
{
	BWAPI::Player * self = Broodwar->self();
	return
		unitType.mineralPrice() <= self->minerals() - allocatedMinerals &&
		unitType.gasPrice() <= self->gas() - allocatedGas &&
		unitType.supplyRequired() <= self->supplyTotal() - self->supplyUsed() - allocatedSupply;
	return false;
}

// Returns the amount of unallocated minerals.
// UNUSED
int Accountant::minerals()
{
	return Broodwar->self()->minerals() - allocatedMinerals;
}

// Returns the amount of unallocated gas.
// UNUSED
int Accountant::gas()
{
	return Broodwar->self()->gas() - allocatedGas;
}

// Returns the amount of unallocated supply.
// UNUSED
int Accountant::supply()
{
	return Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() - allocatedSupply;
}