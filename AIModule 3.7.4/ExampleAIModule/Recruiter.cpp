#include "Recruiter.h"


// Constructor
Recruiter::Recruiter(Accountant * accountant) :
	accountant(accountant),
	trainingSchedule(),
	constructionSchedule(),
	factories()
{
}


// Deconstructor
Recruiter::~Recruiter()
{
}


// Adds a factory to the pool.
void Recruiter::addFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (utilUnit::isOwned(factory) &&
		factory->exists())
		factories[factory->getType()].insert(factory);
}


// Removes a factory from the pool
void Recruiter::removeFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (factory)
		factories[factory->getType()].erase(factory);
}


// Adds a unit to the training schedule.
void Recruiter::addConstruction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit))
	{
		// Verify training schedule.
		BWAPI::UnitType unitType = unit->getType();
		if (trainingSchedule.count(unitType) > 0 &&
			trainingSchedule[unitType] > 0)
		{
			// Remove unit from training schedule.
			trainingSchedule[unitType]--;
			accountant->deallocate(unitType);

			// Insert unit in construction schedule.
			if (constructionSchedule.count(unitType) > 0)
				constructionSchedule[unitType]++;
			else
				constructionSchedule[unitType] = 1;
		}
	}
}


// Removes a unit from the construction schedule.
void Recruiter::removeConstruction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit) &&
		constructionSchedule.count(unit->getType()))
		constructionSchedule[unit->getType()]--;
}


// Attempts to train a unit at an available facility and returns true if successful.
bool Recruiter::scheduleTraining(BWAPI::UnitType unitType)
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
bool Recruiter::scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory)
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
unsigned int Recruiter::scheduled(BWAPI::UnitType unitType)
{
	int scheduled = 0;
	if (trainingSchedule.count(unitType) > 0)
		scheduled += trainingSchedule[unitType];
	if (constructionSchedule.count(unitType) > 0)
		scheduled += constructionSchedule[unitType];
	return scheduled;
}


// Returns current factories of the specified type.
//TODO Is this needed?
utilUnit::UnitSet Recruiter::getFactories(BWAPI::UnitType factoryType)
{
	return factories[factoryType];
}