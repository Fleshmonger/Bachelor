#include "Architect.h"


// Constructor
Architect::Architect(Accountant * accountant, Landlord * landlord) :
	accountant(accountant),
	landlord(landlord),
	constructions(),
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
	return scheduleBuilding(buildingType, buildingLocation, landlord->getIdleWorker(BWTA::getRegion(buildingLocation)));
}


// Attempts to schedule a building at the location with the builder and returns true if it succeeds.
//TODO Check if the building can actually be placed at the building location?
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation, BWAPI::Unit * builder)
{
	// Verify schedule.
	if (buildingType &&																		// Verify building type.
		buildingType.isBuilding() &&
		buildingLocation &&																	// Verify building location.
		BWAPI::Broodwar->canBuildHere(builder, buildingLocation, buildingType, false) &&
		accountant->isAffordable(buildingType) &&											// Verify resources.
		utilUnit::isOwned(builder) &&														// Verify builder.
		builder->exists() &&
		BWAPI::Broodwar->canMake(builder, buildingType))
	{
		// Order the construction.
		landlord->employWorker(builder, TASK_BUILD);
		buildSchedule.insert(std::make_pair(buildingType, std::make_pair(builder, buildingLocation)));
		commandBuild(builder, buildingLocation, buildingType);
		accountant->allocate(buildingType);
		accountant->addSchedule(buildingType);
		return true;
	}
	else if (buildingType.isRefinery())
	{
		if (!buildingType)
			BWAPI::Broodwar->sendText("BuildingType");
		if (!buildingType.isBuilding())
			BWAPI::Broodwar->sendText("isBuilding");
		if (!buildingLocation)
			BWAPI::Broodwar->sendText("buildingLocation");
		if (!accountant->isAffordable(buildingType))
			BWAPI::Broodwar->sendText("isAffordable");
		if (!utilUnit::isOwned(builder))
			BWAPI::Broodwar->sendText("isOwned");
		if (!builder->exists())
			BWAPI::Broodwar->sendText("exists");
		if (!BWAPI::Broodwar->canMake(builder, buildingType))
			BWAPI::Broodwar->sendText("canMake");
	}
	return false;
}


// Constructs a new building.
void Architect::scheduleConstruct(BWAPI::Unit * building)
{
	BWAPI::UnitType buildingType = building->getType();
	//constructSchedule.insert(std::make_pair(buildingType, building));
	constructions.insert(building);
	accountant->addSchedule(buildingType);
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
				buildSchedule.erase(it);
				accountant->deallocate(build.first);
				accountant->removeSchedule(buildingType);
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
	if (contains(building))
	{
		constructions.erase(building);
		accountant->removeSchedule(building->getType());
	}
	/*
	// Verify construct
	if (utilUnit::isOwned(building) &&
		building->getType().isBuilding())
	{
		// Match construct.
		std::pair<std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator, std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator> range = constructSchedule.equal_range(building->getType());
		std::multimap<BWAPI::UnitType, BWAPI::Unit*>::iterator it = range.first, end = range.second;
		while (it != end)
		{
			// Match cosntruct.
			BWAPI::Unit * construct = (*it).second;
			if (construct == building)
			{
				// Remove construct.
				constructSchedule.erase(it);
				accountant->removeSchedule(building->getType());
				return;
			}
			else
				++it;
		}
	}
	*/
}


// Identifies a building as built.
void Architect::completeBuild(BWAPI::Unit * building)
{
	BWAPI::UnitType buildingType = building->getType();
	removeBuild(buildingType, building->getTilePosition());
	scheduleConstruct(building);
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
				BWAPI::Broodwar->canBuildHere(builder, buildingLocation, buildingType, false) &&
				BWAPI::Broodwar->canMake(builder, buildingType))
				commandBuild(builder, buildingLocation, buildingType);
			else
				removeBuild(buildingType, buildingLocation);
		}
	}

	// Remove all invalid construction orders.
	{
		utilUnit::UnitSet::iterator
			it = constructions.begin(),
			end = constructions.end();
		while (it != end)
		{
			BWAPI::Unit * building = *it;
			++it;
			if (!building || !building->exists())
			{
				removeConstruct(building);
				end = constructions.end();
			}
		}
	}

}


// Returns whether the unit is monitored.
bool Architect::contains(BWAPI::Unit * unit)
{
	return constructions.count(unit) > 0;
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