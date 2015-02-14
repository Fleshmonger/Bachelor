#include "Architect.h"

Architect::Architect(WorkerManager * workerManager)
{
	this->workerManager = workerManager;
	orders = new std::map < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> > ;
	pylons = new BWAPI::Unitset();
	incompletePylons = 0;
}

//Unused deconstructor
Architect::~Architect()
{

}

// Attempt to construct a new building. Returns true if it succeeds, otherwise returns false.
bool Architect::orderBuilding(BWAPI::UnitType buildingType)
{
	// Check if we already have such an order.
	if (orders->count(buildingType) == 0)
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
						(*orders)[buildingType] = std::make_pair(builder, buildLocation);
						return true;
					} // closure: builder
				} // closure: location
			} // closure: affordable
		} // closure: is building
	} // closure: order exists
	return false; // Something went wrong.
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
void Architect::removeOrder(BWAPI::UnitType buildingType)
{
	if (orders->count(buildingType) == 1)
	{
		auto order = orders->at(buildingType); // Should this be deconstructed?
		BWAPI::Unit builder = order.first;
		if (builder && builder->exists())
		{
			workerManager->addWorker(order.first);
		}
		orders->erase(buildingType);
	}
}

// Increments the amount of currently unfinished supply buildings.
void Architect::addIncompletePylon()
{
	incompletePylons++;
}

// Adds a pylon to the pylon pool.
void Architect::addPylon(BWAPI::Unit pylon)
{
	pylons->insert(pylon);
	incompletePylons--;
}

// Removes a pylon to the pylon pool.
void Architect::removePylon(BWAPI::Unit pylon)
{
	auto it = pylons->find(pylon);
	// Check if the pylon was completed.
	if (it != pylons->end())
		pylons->erase(pylon);
	else
		incompletePylons--;
}

// Designates the current base position.
void Architect::setDepot(BWAPI::Unit depot)
{
	this->depot = depot;
}

// Simulate the architect AI. Creates pylons and commands builders.
void Architect::update() 
{
	// Order a pylon if more supply is needed, and no pylons are being constructed.
	if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() == 0 &&
		incompletePylons == 0)
		orderBuilding(Broodwar->self()->getRace().getSupplyProvider());

	// Check if all orders are still valid, and ensure builders are building.
	auto it = orders->begin();
	while (it != orders->end())
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
			it = orders->erase(it);
		}
	}
}