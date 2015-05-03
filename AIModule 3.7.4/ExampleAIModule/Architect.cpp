#include "Architect.h"


// Constructor
Architect::Architect(Accountant * accountant, Landlord * landlord) :
	accountant(accountant),
	landlord(landlord),
	constructSchedule(),
	buildSchedule()
{
}


// Deconstructor
Architect::~Architect()
{
}


// Commands the worker to build a structure
void Architect::commandBuild(BWAPI::Unit * builder, BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType)
{
	if (utilMap::isExplored(buildingLocation, buildingType))
		builder->build(buildingLocation, buildingType);
	else
		utilUnit::command(builder, BWAPI::UnitCommandTypes::Move, BWAPI::Position(buildingLocation));
}


// Attempts to schedule a building in the vassal and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, Vassal * vassal)
{
	// Verify vassal.
	if (vassal &&
		vassal->getDepot())
	{
		// Aquire builder.
		BWAPI::Unit * builder = landlord->getIdleWorker(vassal->getRegion());

		// Aquire building location.
		BWAPI::TilePosition buildingLocation = getBuildLocation(builder, vassal->getDepot()->getTilePosition(), buildingType);

		// Schedule building.
		return scheduleBuilding(buildingType, buildingLocation, builder);
	}
	else
		return false;
}


/*
// Attempts to schedule a building in the region and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWTA::Region * region)
{
	// Schedule building.
	scheduleBuilding(buildingType, landlord->getVassal(region));
}
*/


// Attempts to schedule a building at the location and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation)
{
	// Schedule building.
	return scheduleBuilding(buildingType, buildingLocation, landlord->getIdleWorker(BWTA::getRegion(buildingLocation)));
}


// Attempts to schedule a building at the location with the builder and returns true if it succeeds.
//TODO Check if the building can actually be placed at the building location?
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation, BWAPI::Unit * builder)
{
	// Verify schedule.
	if (buildingType &&										// Verify building type.
		buildingType.isBuilding() &&
		buildingLocation &&									// Verify building location.
		accountant->isAffordable(buildingType) &&			// Verify resources.
		utilUnit::isOwned(builder) &&						// Verify builder.
		builder->exists() &&
		BWAPI::Broodwar->canMake(builder, buildingType))
	{
		// Order the construction.
		landlord->employWorker(builder, TASK_BUILD);
		buildSchedule.insert(std::make_pair(buildingType, std::make_pair(builder, buildingLocation)));
		accountant->allocate(buildingType);
		schedule[buildingType]++;
		commandBuild(builder, buildingLocation, buildingType);
		return true;
	}
	else
		return false;
}


// Attempts to schedule a refinery at the tile position and returns true if successful.
bool Architect::scheduleRefinery(BWAPI::UnitType refineryType, BWAPI::TilePosition buildingLocation)
{
	// Verify order.
	if (refineryType.isRefinery() &&										// Verify type.
		buildingLocation &&													// Verify location
		accountant->isAffordable(refineryType))								// Verify resources.
	{
		// Aquire builder.
		BWAPI::Unit * builder = landlord->getIdleWorker(BWTA::getRegion(buildingLocation));

		// Verify builder.
		if (builder &&
			builder->exists())
		{
			// Schedule building.
			landlord->employWorker(builder, TASK_BUILD);
			buildSchedule.insert(std::make_pair(refineryType, std::make_pair(builder, buildingLocation)));
			accountant->allocate(refineryType);
			schedule[refineryType]++;
			commandBuild(builder, buildingLocation, refineryType);
			return true;
		}
	}

	// Scheduling was unsuccessful.
	return false;
}


// Attempts to schedule a refinery on the geyser and returns true if it succeeds.
bool Architect::scheduleRefinery(BWAPI::UnitType refineryType, BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (geyser)
		return scheduleRefinery(refineryType, geyser->getInitialTilePosition());
	else
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
				schedule[buildingType]--;
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
				schedule[building->getType()]--;
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
	BWAPI::UnitType buildingType = building->getType();
	removeBuild(buildingType, building->getTilePosition());
	scheduleConstruct(building);
	schedule[buildingType]++;
}


// Identifies a building as constructed.
void Architect::completeConstruct(BWAPI::Unit * building)
{
	removeConstruct(building);
}


// Verifies orders and commands builders.
// TODO Remove verification?
void Architect::update()
{
	// Remove invalid build orders and continue valid orders.
	{
		std::multimap <BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::iterator it = buildSchedule.begin();
		while (it != buildSchedule.end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit * builder = it->second.first;
			BWAPI::TilePosition buildingLocation = it->second.second;
			++it;
			if (builder &&
				builder->exists() &&
				//BWAPI::Broodwar->canBuildHere(builder, buildTarget, buildingType) &&
				BWAPI::Broodwar->canMake(builder, buildingType))
				commandBuild(builder, buildingLocation, buildingType);
			else
				removeBuild(buildingType, buildingLocation);
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
//TODO Reimplement harvesting zones.
bool Architect::validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType)
{
	return
		location &&
		location.isValid() &&
		!landlord->getHarvestingZone(BWTA::getRegion(location)).contains(location, buildingType) &&
		BWAPI::Broodwar->canBuildHere(builder, location, buildingType, false);
}


// Returns the amount of scheduled buildings of the specified type.
int Architect::scheduled(BWAPI::UnitType buildingType)
{
	//return buildSchedule.count(buildingType) + constructSchedule.count(buildingType);
	return schedule[buildingType];
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