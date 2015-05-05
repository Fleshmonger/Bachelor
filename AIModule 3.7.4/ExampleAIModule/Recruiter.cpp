#include "Recruiter.h"


// Constructor
Recruiter::Recruiter(Accountant * accountant) :
	accountant(accountant),
	constructions(),
	trainingSchedule(),
	typeFactories()
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
		typeFactories[factory->getType()].insert(factory);
}


// Removes a factory from the pool
void Recruiter::removeFactory(BWAPI::Unit * factory)
{
	// Verify factory.
	if (factory)
		typeFactories[factory->getType()].erase(factory);
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
			constructions.insert(unit);
		}
	}
}


// Removes a unit from the construction schedule.
void Recruiter::removeConstruction(BWAPI::Unit * unit)
{
	// Verify unit.
	if (contains(unit))
	{
		constructions.erase(unit);
		accountant->removeSchedule(unit->getType());
	}
}


// Returns whether the unit is monitored.
bool Recruiter::contains(BWAPI::Unit * unit)
{
	return constructions.count(unit) > 0;
}

// Attempts to train a unit at an available facility and returns true if successful.
bool Recruiter::scheduleTraining(BWAPI::UnitType unitType)
{
	// Search through factories.
	std::pair<BWAPI::UnitType, int> builderInfo = unitType.whatBuilds();
	BOOST_FOREACH(BWAPI::Unit * factory, typeFactories[builderInfo.first])
	{
		// Verify factory.
		if (factory->exists() &&
			!factory->isTraining() &&
			!utilUnit::isCommanded(factory))
			return scheduleTraining(unitType, factory);
	}

	// Scheduling failed.
	return false;
}


// Schedules the unit type to be trained at the factory.
bool Recruiter::scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory)
{
	if (unitType &&								// Verify type.
		accountant->isAffordable(unitType) &&	// Verify cost.
		utilUnit::isOwned(factory) &&			// Verify availability.
		factory->exists() &&
		!factory->isTraining() &&
		!utilUnit::isCommanded(factory) &&
		factory->train(unitType))				// Attempt training.
	{
		// Schedule.
		accountant->addSchedule(unitType);
		accountant->allocate(unitType);
		if (trainingSchedule.count(unitType) > 0)
			trainingSchedule[unitType]++;
		else
			trainingSchedule[unitType] = 1;
		return true;
	}
	else
		return false;
}


// Returns current factories of the specified type.
//TODO Redundant?
utilUnit::UnitSet Recruiter::getFactories(BWAPI::UnitType factoryType)
{
	return typeFactories[factoryType];
}