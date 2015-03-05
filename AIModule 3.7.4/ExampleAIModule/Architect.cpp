#include "Architect.h"

Architect::Architect(WorkerManager * workerManager, Accountant * accountant)
{
	// Managers
	this->workerManager = workerManager;
	this->accountant = accountant;
	// Local
	buildSchedule = new std::multimap < BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition> >;
	constructSchedule = new std::multimap < BWAPI::UnitType, BWAPI::Unit* >;
	pylons = new std::set<Unit*>();
}

//Unused deconstructor
Architect::~Architect()
{
}

// Returns the nearest tileposition which is buildable.
// Incomplete!
BWAPI::TilePosition Architect::getBuildLocation(BWAPI::Unit * builder, BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation)
{
	if (buildingType && desiredLocation)
	{
		// Check in a spiral pattern.
		bool horizontal = false;
		int dx = 0, dy = 0, length = 0, step = 1;
		while (length < Broodwar->mapWidth() || length < Broodwar->mapHeight())
		{
			// Check the current tile.
			BWAPI::TilePosition tile = BWAPI::TilePosition::TilePosition(desiredLocation.x() + dx, desiredLocation.y() + dy);
			if (tile.isValid() && Broodwar->canBuildHere(builder, tile, buildingType))
				return tile;
			// Find next tile.
			if (dx == dy)
				length++;
			if (horizontal)
			{
				dx += step;
				//tile.x += step;
				if (dx == length * step)
					horizontal = false;
			}
			else
			{
				dy += step;
				if (dy == length * step)
				{
					horizontal = true;
					step *= -1;
				}
			}

		}
	}
	return BWAPI::TilePositions::None;
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
			BWAPI::Unit * builder = workerManager->takeWorker();
			if (builder)
			{
				TilePosition desiredBuildLocation, targetBuildLocation;
				if (buildingType == BWAPI::UnitTypes::Protoss_Pylon && depot)
					desiredBuildLocation = depot->getTilePosition();
				else if (!pylons->empty())
				{
					// Find a pylon to build near.
					std::set<Unit*>::iterator it = pylons->begin();
					desiredBuildLocation = (*it)->getTilePosition();
					// TODO go through all pylons if no position has been found!
				}
				targetBuildLocation = getBuildLocation(builder, buildingType, desiredBuildLocation);
				if (targetBuildLocation)
				{
					// Order the construction.
					builder->build(targetBuildLocation, buildingType);
					buildSchedule->insert(std::make_pair(buildingType, std::make_pair(builder, targetBuildLocation)));
					accountant->allocUnit(buildingType);
					return true;
				} // Closure: location
				else
					workerManager->addWorker(builder);
			} // Closure: builder.
		} // Closure: affordable.
	} // Closure: is building.
	// Attempt unsuccessful.
	return false;
}

// Constructs a new building.
void Architect::scheduleConstruct(BWAPI::Unit * building)
{
	BWAPI::UnitType buildingType = building->getType();
	constructSchedule->insert(std::make_pair(buildingType, building));
}

/*
// Removes an iterator from the build schedule.
void Architect::removeBuild(std::map<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::const_iterator it)
{
auto build = *it;
BWAPI::Unit * builder = build.second.first;
if (builder && builder->exists())
workerManager->addWorker(builder);
accountant->deallocUnit(build.first);
buildSchedule->erase(it);
}
*/

// Removes a build order from the build schedule.
void Architect::removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildTarget)
{
	std::pair<std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator, std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator> range = buildSchedule->equal_range(buildingType);
	std::multimap <BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator it = range.first, end = range.second;
	while (it != end)
	{
		BWAPI::TilePosition location = (*it).second.second;
		//if (location == building->getTilePosition())
		if (location == buildTarget)
		{
			std::pair<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>> build = *it;
			BWAPI::Unit * builder = build.second.first;
			if (builder && builder->exists())
				workerManager->addWorker(builder);
			accountant->deallocUnit(build.first);
			buildSchedule->erase(it);
			//removeBuild(it);
			return;
		}
		else
			++it;
	}
}

/*
// Removes a build order from the build schedule.
void Architect::removeBuild(BWAPI::Unit * building)
{
BWAPI::UnitType buildingType = building->getType();
auto range = buildSchedule->equal_range(buildingType);
auto it = range.first, end = range.second;
while (it != end)
{
BWAPI::TilePosition location = (*it).second.second;
if (location == building->getTilePosition())
{
auto build = *it;
BWAPI::Unit * builder = build.second.first;
if (builder && builder->exists())
workerManager->addWorker(builder);
accountant->deallocUnit(build.first);
buildSchedule->erase(it);
//removeBuild(it);
return;
}
else
++it;
}
}
*/

/*
// Removes an iterator from the construct schedule.
void Architect::removeConstruct(std::map<BWAPI::UnitType, BWAPI::Unit*>::iterator it)
{
constructSchedule->erase(it);
}
*/

// Removes a construction order from the construct schedule.
void Architect::removeConstruct(BWAPI::Unit * building)
{
	std::pair<std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator, std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator> range = constructSchedule->equal_range(building->getType());
	std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator it = range.first, end = range.second;
	//auto range = constructSchedule->equal_range(building->getType());
	//auto it = range.first, end = range.second;
	while (it != end)
	{
		BWAPI::Unit * construct = (*it).second;
		if (construct == building)
		{
			constructSchedule->erase(it);
			//removeConstruct(it);
			return;
		}
		else
			++it;
	}
}

// Identifies a building as built.
void Architect::completeBuild(BWAPI::Unit * building) // Rename this.
{
	removeBuild(building->getType(), building->getTilePosition());
	//removeBuild(building);
	scheduleConstruct(building);
}

// Identifies a building as constructed.
void Architect::completeConstruct(BWAPI::Unit * building) // Rename this.
{
	removeConstruct(building);
}

// Returns the amount of oders of a specific building type currently scheduled.
int Architect::scheduled(BWAPI::UnitType buildingType)
{
	return buildSchedule->count(buildingType) + constructSchedule->count(buildingType);
}

// Adds a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::addPylon(BWAPI::Unit * pylon)
{
	pylons->insert(pylon);
}

// Removes a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::removePylon(BWAPI::Unit * pylon)
{
	pylons->erase(pylon);
}

// Designates the current base position.
void Architect::setDepot(BWAPI::Unit * depot)
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
	{
		std::multimap <BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator it = buildSchedule->begin();
		while (it != buildSchedule->end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit * builder = it->second.first;
			BWAPI::TilePosition buildTarget = it->second.second;
			++it;
			if (builder &&
				builder->exists() &&
				Broodwar->canBuildHere(builder, buildTarget, buildingType) &&
				Broodwar->canMake(builder, buildingType))
				builder->build(buildTarget, buildingType);
			else
				removeBuild(buildingType, buildTarget);
		}
	}
	// Remove all invalid construction orders.
	{
		std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator it = constructSchedule->begin();
		while (it != constructSchedule->end())
		{
			BWAPI::Unit * building = it->second;
			++it;
			if (!building || !building->exists())
				removeConstruct(building);
		}
	}

}