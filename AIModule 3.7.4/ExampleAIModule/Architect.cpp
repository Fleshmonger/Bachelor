#include "Architect.h"


// Constructor
Architect::Architect(Accountant * accountant, Landlord * landlord) :
	accountant(accountant),
	landlord(landlord),
	harvesting(0, 0, 0, 0),
	constructSchedule(),
	buildSchedule()
{
}


// Deconstructor
Architect::~Architect()
{
}


// Attempt to build a new building. Returns true if it succeeds, otherwise returns false.
bool Architect::scheduleBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation)
{
	// Confirm the unit type.
	if (buildingType.isBuilding())
	{
		// Check if we have the resources.
		if (accountant->isAffordable(buildingType))
		{
			// Aquire builder.
			BWAPI::Unit * builder = landlord->getIdleWorker(BWTA::getRegion(desiredLocation));
			if (builder)
			{
				BWAPI::TilePosition location = getBuildLocation(builder, desiredLocation, buildingType);
				if (location)
				{
					// Order the construction.
					landlord->employWorker(builder, TASK_BUILD);
					utilUnit::commandBuild(builder, location, buildingType);
					buildSchedule.insert(std::make_pair(buildingType, std::make_pair(builder, location)));
					accountant->allocate(buildingType);
					return true;
				}
			}
		}
	}
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
				landlord->dismissWorker(builder);
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

		harvesting = utilMap::Zone(left, top, right, bottom);
	}
}


// Creates pylons, validates orders and commands builders.
void Architect::update()
{
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


// Returns the amount of scheduled buildings of the specified type.
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