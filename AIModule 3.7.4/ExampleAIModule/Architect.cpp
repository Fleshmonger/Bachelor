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


// Attempts to schedule a building in the region related to the vassal and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, Vassal * vassal)
{
	// Verify vassal.
	if (vassal &&
		vassal->getDepot())
		return scheduleBuilding(buildingType, vassal->getDepot()->getTilePosition());
	else
		return false;
}


// Attempts to schedule a building in the region and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWTA::Region * region)
{
	return scheduleBuilding(buildingType, landlord->getVassal(region));
}


// Attempts to schedule a building near the desired location and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation)
{
	return scheduleBuilding(buildingType, desiredLocation, landlord->getIdleWorker(BWTA::getRegion(desiredLocation)));
}


// Attempts to schedule a building near the desired location with the builder and returns true if it succeeds.
bool Architect::scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation, BWAPI::Unit * builder)
{
	// Confirm the unit type.
	if (buildingType.isBuilding())
	{
		// Check if we have the resources.
		if (accountant->isAffordable(buildingType))
		{
			// Verify builder.
			if (utilUnit::isOwned(builder) &&
				builder->exists() &&
				builder->getType().isWorker())
			{
				BWAPI::TilePosition location = getBuildLocation(builder, desiredLocation, buildingType);
				if (location)
				{
					// Order the construction.
					landlord->employWorker(builder, TASK_BUILD);
					utilUnit::commandBuild(builder, location, buildingType);
					buildSchedule.insert(std::make_pair(buildingType, std::make_pair(builder, location)));
					accountant->allocate(buildingType);
					schedule[buildingType]++;
					return true;
				}
			}
		}
	}

	// Attempt unsuccessful.
	return false;
}


// Attempts to build a refinery on the geyser and returns true if it succeeds.
bool Architect::scheduleRefinery(BWAPI::UnitType refineryType, BWAPI::Unit * geyser)
{
	// Verify order.
	if (refineryType.isRefinery() &&										// Verify type.
		geyser &&															// Verify geyser.
		geyser->exists() &&
		geyser->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
		accountant->isAffordable(refineryType))								// Verify resources.
	{
		// Aquire builder.
		BWAPI::Unit * builder = landlord->getIdleWorker(BWTA::getRegion(geyser->getPosition()));

		// Verify builder.
		if (builder)
		{
			// Schedule building.
			landlord->employWorker(builder, TASK_BUILD);
			builder->build(geyser->getTilePosition(), refineryType);
			buildSchedule.insert(std::make_pair(refineryType, std::make_pair(builder, geyser->getTilePosition())));
			accountant->allocate(refineryType);
			schedule[refineryType]++;
			return true;
		}
	}

	// Scheduling was unsuccessful.
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
			BWAPI::TilePosition buildTarget = it->second.second;
			++it;
			if (builder &&
				builder->exists() &&
				//BWAPI::Broodwar->canBuildHere(builder, buildTarget, buildingType) &&
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
//TODO Reimplement harvesting zones.
bool Architect::validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType)
{
	return
		location &&
		location.isValid() &&
		!landlord->getHarvestingZone(BWTA::getRegion(location)).contains(location, buildingType) &&
		BWAPI::Broodwar->canBuildHere(builder, location, buildingType);
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