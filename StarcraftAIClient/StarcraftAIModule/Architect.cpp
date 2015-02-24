#include "Architect.h"

Architect::Architect(WorkerManager * workerManager, Accountant * accountant)
{
	// Managers
	this->workerManager = workerManager;
	this->accountant = accountant;
	// Local
	buildSchedule = new std::multimap < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> > ;
	constructSchedule = new std::multimap < BWAPI::UnitType, BWAPI::Unit > ;
	pylons = new BWAPI::Unitset();

	std::multimap<BWAPI::UnitType, int> test;
	test.insert(std::pair<BWAPI::UnitType, int>(BWAPI::UnitTypes::Protoss_Arbiter,5));
}

//Unused deconstructor
Architect::~Architect()
{

}

// Attempt to build a new building. Returns true if it succeeds, otherwise returns false.
bool Architect::scheduleBuild(BWAPI::UnitType buildingType)
{
	// Confirm the unit type.
	if (buildingType.isBuilding())
	{
		// Check if we have the resources.
		if (accountant->isAffordable(buildingType))
		{
			// Find a build location
			TilePosition buildOrigin, buildTarget;
			if (buildingType == BWAPI::UnitTypes::Protoss_Pylon)
			{
				// Build the pylon around the depot.
				if (depot)
					buildOrigin = depot->getTilePosition();
				else
					return false;
			}
			else
			{
				// Find a pylon to build near.
				if (!pylons->empty())
				{
					// TODO go through all pylons if no position has been found!
					auto pylonIt = pylons->begin();
					buildOrigin = (*pylonIt)->getTilePosition();
				}
				else
					return false;
			}
			buildTarget = Broodwar->getBuildLocation(buildingType, buildOrigin);
			if (buildTarget)
			{
				// Find a builder.
				BWAPI::Unit builder = workerManager->takeWorker();
				if (builder)
				{
					// Order the construction.
					builder->build(buildingType, buildTarget);
					buildSchedule->insert(std::make_pair(buildingType, std::make_pair(builder, buildTarget)));
					accountant->allocUnit(buildingType);
					return true;
				} // closure: builder
			} // closure: location
		} // closure: affordable
	} // closure: is building
	return false; // Something went wrong.
}

// Constructs a new building.
void Architect::scheduleConstruct(BWAPI::Unit building)
{
	BWAPI::UnitType buildingType = building->getType();
	constructSchedule->insert(std::make_pair(buildingType, building));
}

/*
// Removes a building from the build schedule.
void Architect::stopBuild(BWAPI::UnitType buildingType)
{
	if (buildSchedule->count(buildingType) == 1)
	{
		auto order = buildSchedule->at(buildingType); // Should this be deconstructed?
		BWAPI::Unit builder = order.first;
		if (builder && builder->exists())
			workerManager->addWorker(builder);
		accountant->deallocUnit(buildingType);
		buildSchedule->erase(buildingType);
	}
}
*/

// Removes a construction from the construct schedule.
void Architect::stopConstruct(BWAPI::Unit building)
{
	auto range = constructSchedule->equal_range(building->getType());
	auto it = range.first, end = range.second;
	while (it != end)
	{
		BWAPI::Unit construct = (*it).second;
		if (construct == building)
		{
			constructSchedule->erase(it);
			return;
		}
		else
			++it;
	}
}

// Identifies a building as built.
void Architect::completeBuild(BWAPI::Unit building) // Rename this.
{
	// Remove the old build schedule.
	BWAPI::UnitType buildingType = building->getType();
	auto range = buildSchedule->equal_range(buildingType);
	auto it = range.first, end = range.second;
	while (it != end)
	{
		auto build = (*it).second;
		BWAPI::TilePosition location = build.second;
		if (location == building->getTilePosition())
		{
			BWAPI::Unit builder = build.first;
			if (builder && builder->exists())
				workerManager->addWorker(builder);
			accountant->deallocUnit(buildingType);
			buildSchedule->erase(it);
			it = end;
		}
		else
			++it;
	}
	// Schedule construction.
	scheduleConstruct(building);
}

// Identifies a building as constructed.
void Architect::completeConstruct(BWAPI::Unit building) // Rename this.
{
	stopConstruct(building);
}

// Returns the amount of oders of a specific building type currently scheduled.
int Architect::scheduled(BWAPI::UnitType buildingType)
{
	return buildSchedule->count(buildingType) + constructSchedule->count(buildingType);
}

// Adds a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::addPylon(BWAPI::Unit pylon)
{
	pylons->insert(pylon);
}

// Removes a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::removePylon(BWAPI::Unit pylon)
{
	pylons->erase(pylon);
}

// Designates the current base position.
void Architect::setDepot(BWAPI::Unit depot)
{
	this->depot = depot;
}

// Simulate the architect AI.
// Creates pylons, validates orders and commands builders.
void Architect::update() 
{
	// Order a supply building if more supply is needed, and none are being constructed.
	if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() == 0 &&
		scheduled(SUPPLY) == 0)
		scheduleBuild(SUPPLY);
	// Remove invalid build orders and continue valid orders.
	// Code duplication with stopBuild.
	{
		auto it = buildSchedule->begin();
		while (it != buildSchedule->end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit builder = it->second.first;
			BWAPI::TilePosition buildTarget = it->second.second;
			if (builder &&
				builder->exists() &&
				Broodwar->canBuildHere(buildTarget, buildingType) &&
				Broodwar->canMake(buildingType, builder))
			{
				// Continue build
				builder->build(buildingType, buildTarget);
				++it;
			}
			else
			{
				// Cancel build
				if (builder && builder->exists())
					workerManager->addWorker(builder);
				buildSchedule->erase(it++);
				accountant->deallocUnit(buildingType);
			}
		}
	}
	// Remove all invalid construction orders.
	// Code duplication with stopBuild.
	{
		auto it = constructSchedule->begin();
		while (it != constructSchedule->end())
		{
			BWAPI::Unit building = it->second;
			if (!building || !building->exists())
				it = constructSchedule->erase(it);
			else
				++it;
		}
	}
}