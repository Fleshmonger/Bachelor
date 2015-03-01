#include "Accountant.h"

Accountant::Accountant()
{
	mineralsAlloc = 0;
	gasAlloc = 0;
	supplyAlloc = 0;
}

// Unused deconstructor
Accountant::~Accountant()
{
}

// Returns whether we can afford to build a given unit type.
bool Accountant::isAffordable(BWAPI::UnitType unitType)
{
	BWAPI::Player * self = Broodwar->self();
	return
		unitType.mineralPrice() <= self->minerals() - mineralsAlloc &&
		unitType.gasPrice() <= self->gas() - gasAlloc &&
		unitType.supplyRequired() <= self->supplyTotal() - self->supplyUsed() - supplyAlloc;
	return false;
}

// Allocates the costs of building a given unit type.
void Accountant::allocUnit(BWAPI::UnitType unitType)
{
	mineralsAlloc += unitType.mineralPrice();
	gasAlloc += unitType.gasPrice();
	supplyAlloc += unitType.supplyRequired();
}

// Deallocates the cost of a given unit type.
void Accountant::deallocUnit(BWAPI::UnitType unitType)
{
	mineralsAlloc -= unitType.mineralPrice();
	gasAlloc -= unitType.gasPrice();
	supplyAlloc -= unitType.supplyRequired();
}


// Returns the amount of unallocated minerals.
int Accountant::minerals()
{
	return Broodwar->self()->minerals() - mineralsAlloc;
}

// Returns the amount of unallocated gas.
//int Accountant::gas()
//{
//	return Broodwar->self()->gas() - gasAlloc;
//}

// Returns the amount of unallocated supply.
//int Accountant::supply()
//{
//return Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() - supplyAlloc;
//}
