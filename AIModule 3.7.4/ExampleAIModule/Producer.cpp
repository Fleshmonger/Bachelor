#include "Producer.h"


// Constructor
Producer::Producer(Accountant * accountant) :
	accountant(accountant)
{
}


// Deconstructor
Producer::~Producer()
{
}


// Adds a unit to the factory pool.
void Producer::addFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (factory &&
		factory->exists())
		factories[factory->getType()].insert(factory);
}


// Removes a unit from the factory pool
void Producer::removeFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (factory)
		factories[factory->getType()].erase(factory);
}


// Accounts for the new unit.
void Producer::addProduction(BWAPI::Unit * unit)
{
	BWAPI::UnitType unitType = unit->getType();
	if (scheduled.count(unitType) > 0)
	{
		scheduled.erase(unitType);
		//incompleteUnits.insert(unit);
		accountant->deallocate(unitType);
	}
}


/*
// Removes a unit from the incomplete pool.
void Producer::removeProduction(BWAPI::Unit * unit)
{
	incompleteUnits.erase(unit);
}
*/


// Attempt to produce an infantry unit at an available facility and returns true if successful.
bool Producer::train(BWAPI::UnitType unitType)
{
	// Verify order order.
	if (!unitType.isBuilding())
	{
		// Verify cost.
		if (accountant->isAffordable(unitType))
		{
			// Find factory.
			std::pair<BWAPI::UnitType, int> builderInfo = unitType.whatBuilds();
			BOOST_FOREACH(BWAPI::Unit * factory, factories[builderInfo.first])
			{
				// Check availability.
				if (factory &&
					factory->exists() &&
					factory->isIdle())
				{
					// Train unit.
					factory->train(unitType);
					accountant->allocate(unitType);
					scheduled.insert(unitType);
					return true;
				}
			}
		}
	}
	return false;
}

utilUnit::UnitSet Producer::getFactories(BWAPI::UnitType factoryType)
{
	return factories[factoryType];
}