#include "Producer.h"


// Constructor
Producer::Producer(Accountant * accountant) :
	accountant(accountant),
	trainingSchedule(),
	productionSchedule(),
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
	if (utilUnit::isOwned(factory) &&
		factory->exists())
		factories[factory->getType()].insert(factory);
}


// Adds a unit to the production schedule.
void Producer::addProduction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit))
	{
		// Verify schedule.
		BWAPI::UnitType unitType = unit->getType();
		if (trainingSchedule.count(unitType) > 0 &&
			trainingSchedule[unitType] > 0)
		{
			// Remove training schedule.
			trainingSchedule[unitType]--;
			accountant->deallocate(unitType);

			// Add production schedule.
			if (productionSchedule.count(unitType) > 0)
				productionSchedule[unitType]++;
			else
				productionSchedule[unitType] = 1;
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


// Removes a unit from the production schedule.
void Producer::removeProduction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit) &&
		productionSchedule.count(unit->getType()))
		productionSchedule[unit->getType()]--;
}


// Attempts to train a unit at an available facility and returns true if successful.
bool Producer::scheduleTraining(BWAPI::UnitType unitType)
{
	// Search through factories.
	std::pair<BWAPI::UnitType, int> builderInfo = unitType.whatBuilds();
	BOOST_FOREACH(BWAPI::Unit * factory, factories[builderInfo.first])
	{
		// Verify factory.
		if (factory->exists() &&
			!factory->isTraining())
			return scheduleTraining(unitType, factory);
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
		if (trainingSchedule.count(unitType) > 0)
			trainingSchedule[unitType]++;
		else
			trainingSchedule[unitType] = 1;
		accountant->allocate(unitType);
		return true;
	}
	else
		return false;
}


// Returns the amount of scheduled units of the unit type.
unsigned int Producer::scheduled(BWAPI::UnitType unitType)
{
	int scheduled = 0;
	if (trainingSchedule.count(unitType) > 0)
		scheduled += trainingSchedule[unitType];
	if (productionSchedule.count(unitType) > 0)
		scheduled += productionSchedule[unitType];
	return scheduled;
}


// Returns current factories of the specified type.
//TODO Is this needed?
utilUnit::UnitSet Producer::getFactories(BWAPI::UnitType factoryType)
{
	return factories[factoryType];
}