#include "Producer.h"


// Constructor
Producer::Producer(Accountant * accountant) :
	accountant(accountant),
	trainingSchedule(),
	factories()
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


// Adds a unit to the production schedule.
void Producer::addProduction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		utilUnit::isOwned(unit))
	{
		// Verify schedule.
		BWAPI::UnitType unitType = unit->getType();
		if (trainingSchedule.count(unitType) > 0)
		{
			// Remove training schedule.
			trainingSchedule.erase(unitType);
			accountant->deallocate(unitType);
		}
	}
}


// Removes a unit from the factory pool
void Producer::removeFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (factory)
		factories[factory->getType()].erase(factory);
}


// Attempts to train a unit at an available facility and returns true if successful.
bool Producer::scheduleTraining(BWAPI::UnitType unitType)
{
	// Find factory.
	std::pair<BWAPI::UnitType, int> builderInfo = unitType.whatBuilds();
	BOOST_FOREACH(BWAPI::Unit * factory, factories[builderInfo.first])
	{
		// Check availability.
		if (factory->exists() &&
			!factory->isTraining())
		{
			return scheduleTraining(unitType, factory);
		}
	}
}


// Schedules the unit type to be trained at the factory.
bool Producer::scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory)
{
	if (unitType &&								// Verify type.
		accountant->isAffordable(unitType) &&	// Verify cost.
		utilUnit::isOwned(factory) &&			// Verify availability.
		factory->exists() &&
		!factory->isTraining() &&
		factory->train(unitType))				// Attempt training.
	{
		// Schedule training.
		trainingSchedule.insert(unitType);
		accountant->allocate(unitType);
		return true;
	}
	else
		return false;
}


// Returns current factories of the specified type.
//TODO Is this needed?
utilUnit::UnitSet Producer::getFactories(BWAPI::UnitType factoryType)
{
	return factories[factoryType];
}