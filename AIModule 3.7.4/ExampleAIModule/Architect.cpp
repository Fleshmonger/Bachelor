#include "Architect.h"


// Constructor
// TODO: Simplify.
Architect::Architect(Accountant * accountant, WorkerManager * workerManager) :
	accountant(accountant),
	workerManager(workerManager),
	harvesting(0, 0, 0, 0),
	depot(),
	pylons(),
	constructSchedule(),
	buildSchedule()
{
}


// Deconstructor
Architect::~Architect()
{
}


// Fired when the map is analyzed. Reads mineral and geyser positions.
void Architect::analyzed()
{
	// Clear harvesting zone.
	harvesting = Zone(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight(), 0, 0);

	// Expand harvesting zone.
	BOOST_FOREACH(BWAPI::Unit * mineral, BWTA::getStartLocation(BWAPI::Broodwar->self())->getStaticMinerals())
		expandHarvesting(mineral);
	BOOST_FOREACH(BWAPI::Unit * geyser, BWTA::getStartLocation(BWAPI::Broodwar->self())->getGeysers())
		expandHarvesting(geyser);
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
			// Aquire builder.
			BWAPI::Unit * builder = workerManager->getIdleWorker();
			if (builder)
			{
				BWAPI::TilePosition location = getBuildLocation(builder, depot->getTilePosition(), buildingType);
				if (location)
				{
					// Order the construction.
					workerManager->employWorker(builder, TASK_BUILD);
					utilUnit::commandBuild(builder, location, buildingType);
					buildSchedule.insert(std::make_pair(buildingType, std::make_pair(builder, location)));
					accountant->allocate(buildingType);
					return true;
				} // Closure: location
				//workerManager->addWorker(builder);
			} // Closure: builder.
		} // Closure: affordable.
	} // Closure: type.
	// Attempt unsuccessful.
	return false;
}


// Constructs a new building.
void Architect::scheduleConstruct(BWAPI::Unit * building)
{
	BWAPI::UnitType buildingType = building->getType();
	constructSchedule.insert(std::make_pair(buildingType, building));
}


// Removes a build order from the build schedule.
void Architect::removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildTarget)
{
	if (buildingType.isBuilding())
	{
		std::pair<std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator, std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator> range = buildSchedule.equal_range(buildingType);
		std::multimap <BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator it = range.first, end = range.second;
		while (it != end)
		{
			BWAPI::TilePosition location = (*it).second.second;
			if (location == buildTarget)
			{
				std::pair<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>> build = *it;
				BWAPI::Unit * builder = build.second.first;
				if (builder && builder->exists())
					workerManager->employWorker(builder, TASK_IDLE);
					//workerManager->addWorker(builder);
				accountant->deallocate(build.first);
				buildSchedule.erase(it);
				return;
			}
			else
				++it;
		}
	}
}


// Removes a construction order from the construct schedule.
void Architect::removeConstruct(BWAPI::Unit * building)
{
	if (building &&
		building->exists() &&
		utilUnit::isOwned(building) &&
		building->getType().isBuilding())
	{
		std::pair<std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator, std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator> range = constructSchedule.equal_range(building->getType());
		std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator it = range.first, end = range.second;
		while (it != end)
		{
			BWAPI::Unit * construct = (*it).second;
			if (construct == building)
			{
				constructSchedule.erase(it);
				return;
			}
			else
				++it;
		}
	}
}


// Identifies a building as built.
void Architect::completeBuild(BWAPI::Unit * building)
{
	removeBuild(building->getType(), building->getTilePosition());
	scheduleConstruct(building);
}


// Identifies a building as constructed.
void Architect::completeConstruct(BWAPI::Unit * building)
{
	removeConstruct(building);
}


// Resizes the harvesting zone to include the given unit.
void Architect::expandHarvesting(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit && unit->exists())
	{
		// Calculate harvesting.
		BWAPI::TilePosition pos = unit->getTilePosition();
		BWAPI::UnitType type = unit->getType();

		int left = std::min(harvesting.left, pos.x()),
			top = std::min(harvesting.top, pos.y()),
			right = std::max(harvesting.right, pos.x() + type.tileWidth()),
			bottom = std::max(harvesting.bottom, pos.y() + type.tileHeight());

		harvesting = Zone(left, top, right, bottom);
	}
}


// Adds a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::addPylon(BWAPI::Unit * pylon)
{
	pylons.insert(pylon);
}


// Removes a pylon to the pylon pool.
// Used for placing Protoss buildings.
void Architect::removePylon(BWAPI::Unit * pylon)
{
	pylons.erase(pylon);
}


// Designates the current base position.
void Architect::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
	if (depot)
		expandHarvesting(depot);
}


// Creates pylons, validates orders and commands builders.
void Architect::update()
{
	// Order a supply building if more supply is needed, and none are being constructed.
	if (accountant->supply() < MIN_SUPPLY &&
		scheduled(BUILD_SUPPLY) == 0)
		scheduleBuild(BUILD_SUPPLY);
	// Remove invalid build orders and continue valid orders.
	{
		std::multimap <BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator it = buildSchedule.begin();
		while (it != buildSchedule.end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit * builder = it->second.first;
			BWAPI::TilePosition buildTarget = it->second.second;
			++it;
			if (builder &&
				builder->exists() &&
				BWAPI::Broodwar->canBuildHere(builder, buildTarget, buildingType) &&
				BWAPI::Broodwar->canMake(builder, buildingType))
				builder->build(buildTarget, buildingType);
			else
				removeBuild(buildingType, buildTarget);
		}
	}
	// Remove all invalid construction orders.
	{
		std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator it = constructSchedule.begin();
		while (it != constructSchedule.end())
		{
			BWAPI::Unit * building = it->second;
			++it;
			if (!building || !building->exists())
				removeConstruct(building);
		}
	}

}


// Returns whether or not a given building type can be built at a given location.
bool Architect::validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType)
{
	return location.isValid() && !harvesting.contains(location, buildingType) && BWAPI::Broodwar->canBuildHere(builder, location, buildingType);
}


// Returns the amount of oders of a specific building type currently scheduled.
int Architect::scheduled(BWAPI::UnitType buildingType)
{
	return buildSchedule.count(buildingType) + constructSchedule.count(buildingType);
}


// Returns the nearest tileposition in a spiral pattern which is buildable.
BWAPI::TilePosition Architect::getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType)
{
	if (buildingType && desiredLocation)
	{
		// Check in a spiral pattern.
		bool horizontal = false;
		int dx = 0, dy = 0, length = 0, step = 1;
		while (length < BWAPI::Broodwar->mapWidth() || length < BWAPI::Broodwar->mapHeight())
		{
			// Check the current tile.
			BWAPI::TilePosition tile = BWAPI::TilePosition::TilePosition(desiredLocation.x() + dx, desiredLocation.y() + dy);
			if (validBuildLocation(builder, tile, buildingType))
				return tile;

			// Find next tile.
			if (dx == dy)
				length++;
			if (horizontal)
			{
				dx += step;
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