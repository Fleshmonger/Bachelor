#include "Architect.h"

Architect::Architect(WorkerManager * workerManager, Accountant * accountant)
{
	this->workerManager = workerManager;
	this->accountant = accountant;
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
						(*buildOrders)[buildingType] = std::make_pair(builder, buildTarget);
						accountant->allocUnit(buildingType);
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
	return buildOrders->count(buildingType) == 1 || constructOrders->count(buildingType) == 1;
}

// Returns the amount of buildings of a specific type currently scheduled.
int Architect::incompleteCount(BWAPI::UnitType buildingType)
{
	return buildOrders->count(buildingType);
	//return buildOrders->count(buildingType) + constructOrders->count(buildingType);
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
void Architect::removeBuildOrder(BWAPI::UnitType buildingType)
{
	if (buildOrders->count(buildingType) == 1)
	{
		auto order = buildOrders->at(buildingType); // Should this be deconstructed?
		BWAPI::Unit builder = order.first;
		if (builder && builder->exists())
			workerManager->addWorker(builder);
		accountant->deallocUnit(buildingType);
		buildOrders->erase(buildingType);
	}
}

// Removes a build order, deallocates resources and adds a new related construction order 
void Architect::updateBuildOrder(BWAPI::Unit building) // Rename this.
{
	BWAPI::UnitType buildingType = building->getType();
	//constructOrders->insert(std::make_pair(buildingType, building));
	(*constructOrders)[buildingType] = building;
	removeBuildOrder(buildingType);
}

// Removes a completed construction order.
void Architect::updateConstructOrder(BWAPI::UnitType buildingType) // Rename this.
{
	constructOrders->erase(buildingType);
}

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

// Simulate the architect AI.
// Creates pylons, validates orders and commands builders.
void Architect::update() 
{
	// Order a supply building if more supply is needed, and none are being constructed.
	if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() == 0 &&
		!hasOrder(SUPPLY))
		orderBuilding(SUPPLY);
	// Remove invalid build orders and continue valid orders.
	// Code duplication with removeOrder.
	{
		auto it = buildOrders->begin();
		while (it != buildOrders->end())
		{
			BWAPI::UnitType buildingType = it->first;
			BWAPI::Unit builder = it->second.first;
			BWAPI::TilePosition buildTarget = it->second.second;
			if (builder &&
				builder->exists() &&
				Broodwar->canBuildHere(buildTarget, buildingType) &&
				Broodwar->canMake(buildingType, builder))
			{
				builder->build(buildingType, buildTarget);
				++it;
			}
			else
			{
				if (builder && builder->exists())
					workerManager->addWorker(builder);
				it = buildOrders->erase(it);
				accountant->deallocUnit(buildingType);
			}
		}
	}
	// Remove all invalid construction orders.
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