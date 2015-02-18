#include "Architect.h"

Architect::Architect(WorkerManager * workerManager)
{
	this->workerManager = workerManager;
	buildOrders = new std::map < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> > ;
	constructOrders = new std::map < BWAPI::UnitType, BWAPI::Unit > ;
	pylons = new BWAPI::Unitset();
}

//Unused deconstructor
Architect::~Architect()
{

}

// Attempt to construct a new building. Returns true if it succeeds, otherwise returns false.
bool Architect::orderBuilding(BWAPI::UnitType buildingType)
{
	// Check if we already have such an order.
	if (!hasOrder(buildingType))
	{
		if (buildingType.isBuilding())
		{
			// Check if we have the resources.
			if (canAfford(buildingType))
			{
				// Find a build location
				TilePosition desiredBuildLocation;
				if (buildingType == BWAPI::UnitTypes::Protoss_Pylon)
				{
					// Build the pylon around the depot.
					if (depot)
						desiredBuildLocation = depot->getTilePosition();
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
						desiredBuildLocation = (*pylonIt)->getTilePosition();
					}
					else
						return false;
				}
				TilePosition buildLocation = Broodwar->getBuildLocation(buildingType, depot->getTilePosition());
				if (buildLocation)
				{
					// Find a builder.
					BWAPI::Unit builder = workerManager->takeWorker();
					if (builder)
					{
						// Order the construction.
						builder->build(buildingType, buildLocation);
						(*buildOrders)[buildingType] = std::make_pair(builder, buildLocation);
						return true;
					} // closure: builder
				} // closure: location
			} // closure: affordable
		} // closure: is building
	} // closure: order exists
	return false; // Something went wrong.
}

// Returns whether or not the architect is processing an order of a given building type.
// It is currently impossible to detect who commissioned the order.
bool Architect::hasOrder(BWAPI::UnitType buildingType)
{
	return buildOrders->count(buildingType) == 1 || buildOrders->count(buildingType) == 1;
}

bool Architect::canAfford(BWAPI::UnitType buildingType)
{
	BWAPI::PlayerInterface * player = Broodwar->self();
	return player->minerals() >= buildingType.mineralPrice() &&
		player->gas() >= buildingType.gasPrice();
}

/*
// Removes a completed order from the list of current orders.
// Throws an exception if the order does not exist (eg. a building was built without the architect knowing.)
void Architect::buildingCompleted(BWAPI::Unit building)
{
	auto order = orders->at(building->getType()); // Should this be deconstructed?
	workerManager->addWorker(order.first);
	orders->erase(building->getType());
}
*/

// Removes a building from the orders.
// Throws an exception if the order does not exist.
void Architect::removeBuildOrder(BWAPI::UnitType buildingType)
{
	if (buildOrders->count(buildingType) == 1)
	{
		auto order = buildOrders->at(buildingType); // Should this be deconstructed?
		BWAPI::Unit builder = order.first;
		if (builder && builder->exists())
		{
			workerManager->addWorker(order.first);
		}
		buildOrders->erase(buildingType);
	}
}

// Removes a build order and adds a new related construction order 
void Architect::updateBuildOrder(BWAPI::Unit building) // Rename this.
{
	BWAPI::UnitType buildingType = building->getType();
	constructOrders->insert(std::make_pair(buildingType, building));
	removeBuildOrder(buildingType);
}

// Removes a completed construction order.
void Architect::updateConstructOrder(BWAPI::UnitType buildingType) // Rename this.
{
	constructOrders->erase(buildingType);
}

/*
// Increments the amount of currently unfinished supply buildings.
void Architect::addIncompletePylon()
{
	incompletePylons++;
}
*/

// Adds a pylon to the pylon pool, used for Protoss construction.
void Architect::addPylon(BWAPI::Unit pylon)
{
	pylons->insert(pylon);
}

// Removes a pylon to the pylon pool, ignoring it when placing buildings.
void Architect::removePylon(BWAPI::Unit pylon)
{
	pylons->erase(pylon);
}

// Designates the current base position.
void Architect::setDepot(BWAPI::Unit depot)
{
	this->depot = depot;
}

// Simulate the architect AI. Creates pylons and commands builders.
void Architect::update() 
{
	// Order a supply building if more supply is needed, and none are being constructed.
	if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() == 0 &&
		hasOrder(SUPPLY))
		orderBuilding(SUPPLY);

	// Check if all build orders are still valid, and ensure builders are building.
	// Removes all invalid orders-
	{
		auto it = buildOrders->begin();
		while (it != buildOrders->end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit builder = it->second.first;
			BWAPI::TilePosition buildLocation = it->second.second;
			if (builder && builder->exists() &&
				Broodwar->canBuildHere(buildLocation, buildingType) &&
				canAfford(buildingType))
			{
				builder->build(buildingType, buildLocation);
				++it;
			}
			else
			{
				if (builder && builder->exists())
					workerManager->addWorker(builder);
				it = buildOrders->erase(it);
			}
		}
	}

	// Check if all construction orders are still valid.
	// Removes all invalid orders.
	{
		auto it = constructOrders->begin();
		while (it != constructOrders->end())
		{
			BWAPI::Unit building = it->second;
			if (!building || !building->exists())
				it = constructOrders->erase(it);
			else
				++it;
		}
	}
}