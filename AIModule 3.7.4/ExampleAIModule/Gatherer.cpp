#include "Gatherer.h"


// Constructor.
Gatherer::Gatherer(Landlord * landlord) :
landlord(landlord),
regions(),
regionMinerals(),
regionRefineries(),
workerResourceType(),
workerRegion(),
workerTargets(),
resourceWorkers()
//workerResources()
{
}


// Deconstructor.
Gatherer::~Gatherer()
{
}


// Verifies resources and commands workers to gather them.
//TODO Transform into update.
void Gatherer::gather()
{
	// Gather resources.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Aquire resources.
		BWTA::Region * region = vassal->getRegion();
		if (!contains(region))
			addRegion(region);

		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_MINE))
			commandGather(worker, region, RESOURCE_MINERAL);

		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_HARVEST))
			commandGather(worker, region, RESOURCE_GAS);
		/*
		utilUnit::UnitList
		* minerals = &regionMinerals[region],
		* refineries = &regionRefineries[region];

		// Mine minerals.
		if (!minerals->empty())
		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_MINE))
		commandGather(worker, region, RESOURCE_MINERAL);

		// Harvest gas.
		if (!refineries->empty())
		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_HARVEST))
		commandGather(worker, region, RESOURCE_GAS);
		*/
	}
}


// Commands the worker to gather resources.
void Gatherer::commandGather(BWAPI::Unit * worker, BWTA::Region * region, ResourceType resourceType)
{
	// Verify input.
	if (utilUnit::isOwned(worker) &&	// Verify worker.
		worker->exists() &&
		worker->getType().isWorker() &&
		region)							// Verify Region.
	{
		// Aquire resource.
		
		if (contains(worker))
		{
			// Check resource.
			BWAPI::Unit * resource = workerTargets[worker];
			if (!resource ||
				workerRegion[worker] != region ||
				workerResourceType[worker] != resourceType)
			{
				// Redesignate.
				removeWorker(worker);
				addWorker(worker, region, resourceType);
			}
			if (resource->isVisible() && !resource->exists())
			{
				// Remove resource and redesignate.
				removeResource(resource, resourceType);
				addWorker(worker, region, resourceType);
			}

		}
		else
			addWorker(worker, region, resourceType);

		// Verify resource.
		BWAPI::Unit * resource;
		if (contains(worker))
			resource = workerTargets[worker];
		if (resource &&
			resource->exists() &&
			resource->getType().isResourceContainer())
		{
			// Command worker.
			if (worker->isCarryingGas() || worker->isCarryingMinerals())
				utilUnit::command(worker, BWAPI::UnitCommandTypes::Return_Cargo);
			else
				utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, resource);
		}
	}
}


/*
// Commands the worker to gather resources.
void Gatherer::commandGather(BWAPI::Unit * worker, utilUnit::UnitList * resources, BWTA::Region * region)
{
	// Verify input.
	if (worker &&				// Verify worker.
		worker->exists() &&
		resources &&			// Verify resources.
		!resources->empty())
	{
		BWAPI::UnitType resourceType = resources->front()->getType();

		// Aquire resource.
		if (contains(worker))
		{
			// Verify resource type.
			BWAPI::Unit * resource = workerTargets[worker];
			if (!resource ||
				!resource->exists() ||
				resource->getType() != resourceType)
			{
				// Redesignate.
				removeWorker(worker);
				addWorker(worker, resources);
			}
		}
		else if (!resources->empty())
			addWorker(worker, resources);

		// Verify worker.
		if (utilUnit::isOwned(worker) &&
			worker->exists() &&
			worker->getType().isWorker())
		{
			// Verify target.
			BWAPI::Unit * resource = workerTargets[worker];
			if (resource &&
				resource->exists() &&
				resource->getType().isResourceContainer())
			{
				// Command worker.
				if (worker->isCarryingGas() || worker->isCarryingMinerals())
					utilUnit::command(worker, BWAPI::UnitCommandTypes::Return_Cargo);
				else
					utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, resource);
			}
		}

	}
}
*/

// Adds a region to the pool.
void Gatherer::addRegion(BWTA::Region * region)
{
	// Verify region.
	if (region &&
		!contains(region))
	{
		// Add region.
		regions.insert(region);
		regionRefineries[region] = utilUnit::UnitList();

		// Add minerals
		utilUnit::UnitList minerals;
		BOOST_FOREACH(BWTA::BaseLocation * baseLocation, region->getBaseLocations())
		{
			// Add minerals.
			BOOST_FOREACH(BWAPI::Unit * mineral, baseLocation->getStaticMinerals())
			{
				// Add mineral.
				minerals.push_front(mineral);
				resourceWorkers[mineral] = utilUnit::UnitSet();
			}
		}
		regionMinerals[region] = minerals;
	}
}


// Removes a mineral and its miners from the pool.
void Gatherer::removeMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(mineral->getInitialPosition());
		if (contains(region))
		{
			// Remove mineral.
			regionMinerals[region].remove(mineral);

			// Remove workers.
			BOOST_FOREACH(BWAPI::Unit * worker, resourceWorkers[mineral])
			{
				workerResourceType.erase(worker);
				workerTargets.erase(worker);
				workerRegion.erase(worker);
			}
			resourceWorkers.erase(mineral);
		}
	}
}


// Adds a refinery for harvesting.
void Gatherer::addRefinery(BWAPI::Unit * refinery)
{
	// Verify mineral.
	if (utilUnit::isOwned(refinery) &&
		refinery->exists() &&
		refinery->getType().isRefinery())
	{
		// Add refinery.
		regionRefineries[BWTA::getRegion(refinery->getPosition())].push_front(refinery);
		resourceWorkers[refinery] = utilUnit::UnitSet();

		//TODO even saturation.
	}
}


// Removes a refinery and its related miners from the pool.
void Gatherer::removeRefinery(BWAPI::Unit * refinery)
{
	// Verify refinery.
	if (refinery)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(refinery->getPosition());
		if (contains(region))
		{
			// Remove refinery.
			regionRefineries[region].remove(refinery);

			// Remove miners.
			BOOST_FOREACH(BWAPI::Unit * miner, resourceWorkers[refinery])
				workerTargets.erase(miner);
			resourceWorkers.erase(refinery);
		}
	}
}


// Removes a resource and its gatherers from the pool.
void Gatherer::removeResource(BWAPI::Unit * resource, ResourceType resourceType)
{
	// Verify mineral.
	if (resource)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(resource->getInitialPosition());
		if (contains(region))
		{
			// Remove mineral.
			getResources(region, resourceType)->remove(resource);

			// Remove workers.
			BOOST_FOREACH(BWAPI::Unit * worker, resourceWorkers[resource])
			{
				workerResourceType.erase(worker);
				workerTargets.erase(worker);
				workerRegion.erase(worker);
			}
			resourceWorkers.erase(resource);
		}
	}
}


// Adds a worker to the resource pool.
void Gatherer::addWorker(BWAPI::Unit * worker, BWTA::Region * region, ResourceType resourceType)
{
	// Verify input.
	utilUnit::UnitList * resources = getResources(region, resourceType);
	if (utilUnit::isOwned(worker) &&	// Verify worker.
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker) &&
		resources &&					// Verify resources.
		!resources->empty())
	{
		// Set target resource.
		BWAPI::Unit * resource = resources->front();
		workerResourceType[worker] = resourceType;
		workerRegion[worker] = region;
		workerTargets[worker] = resource;
		resourceWorkers[resource].insert(worker);

		// Rotate resource priorities.
		resources->pop_front();
		resources->push_back(resource);
	}
}


/*
// Adds a worker to the resource pool.
void Gatherer::addWorker(BWAPI::Unit * worker, utilUnit::UnitList * resources)
{
	// Verify input.
	if (utilUnit::isOwned(worker) &&	// Verify worker.
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker) &&
		!resources->empty())			// Verify resources.
	{
		// Set target resource.
		BWAPI::Unit * resource = resources->front();
		workerTargets[worker] = resource;
		workerResources[worker] = resources;
		resourceWorkers[resource].insert(worker);

		// Rotate resource priorities.
		resources->pop_front();
		resources->push_back(resource);
	}
}
*/


// Removes a worker from the pool.
//TODO Split resource distribution?
void Gatherer::removeWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (contains(worker))
	{
		// Aquire data.
		BWAPI::Unit * resource = workerTargets[worker];
		utilUnit::UnitList * resources = getResources(workerRegion[worker], workerResourceType[worker]);

		// Remove worker.
		workerResourceType.erase(worker);
		workerRegion.erase(worker);
		workerTargets.erase(worker);
		resourceWorkers[resource].erase(worker);

		// Cycle priorities
		resources->remove(resource);
		resources->push_front(resource);

		// Verify distribution.
		BWAPI::Unit * resourceBack = resources->back();
		if (resourceWorkers[resource].size() + 1 < resourceWorkers[resourceBack].size())
		{
			// Redistribute worker.
			BWAPI::Unit * newWorker = *resourceWorkers[resourceBack].begin();
			resourceWorkers[resourceBack].erase(newWorker);
			resourceWorkers[resource].insert(newWorker);
			workerTargets[newWorker] = resource;

			// Cycle priorities.
			resources->pop_back();
			resources->push_front(resourceBack);
		}
	}
}


/*
// Removes a worker from the resources.
//TODO Split resource distribution?
void Gatherer::removeWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (contains(worker))
	{
		// Remove worker.
		BWAPI::Unit * resource = workerTargets[worker];
		workerTargets.erase(worker);
		resourceWorkers[resource].erase(worker);
		utilUnit::UnitList * resources = workerResources[worker];
		workerResources.erase(worker);

		// Cycle priorities
		resources->remove(resource);
		resources->push_front(resource);

		// Verify distribution.
		BWAPI::Unit * resourceBack = resources->back();
		if (resourceWorkers[resource].size() + 1 < resourceWorkers[resourceBack].size())
		{
			// Redistribute miners.
			BWAPI::Unit * newWorker = *resourceWorkers[resourceBack].begin();
			resourceWorkers[resourceBack].erase(newWorker);
			resourceWorkers[resource].insert(newWorker);

			// Cycle priorities.
			resources->pop_back();
			resources->push_front(resourceBack);
		}
	}
}
*/


// Returns whether the region is in the pool.
bool Gatherer::contains(BWTA::Region * region)
{
	return regions.count(region) > 0;
}


// Returns whether the miner is in the pool.
bool Gatherer::contains(BWAPI::Unit * worker)
{
	return workerTargets.count(worker) > 0;
}


// Returns a pointer to the resources of the given type in that region.
utilUnit::UnitList * Gatherer::getResources(BWTA::Region * region, ResourceType resourceType)
{
	// Verify region.
	if (contains(region))
	{
		if (resourceType == RESOURCE_MINERAL)
			return &regionMinerals[region];
		else
			return &regionRefineries[region];
	}
	else
		return NULL;
		
}


// Returns a copy of worker pointers gathering from the resource.
utilUnit::UnitSet Gatherer::getResourceWorkers(BWAPI::Unit * resource)
{
	if (resourceWorkers.count(resource) > 0)
		return resourceWorkers[resource];
	else
		return utilUnit::UnitSet();
}


// Returns a copy of the sorted list of mineral pointers in the region.
utilUnit::UnitList Gatherer::getMinerals(BWTA::Region * region)
{
	if (contains(region))
		return regionMinerals[region];
	else
		return utilUnit::UnitList();
}


// Returns a the sorted list of refinery pointers in the region.
utilUnit::UnitList Gatherer::getRefineries(BWTA::Region * region)
{
	// Verify region.
	if (contains(region))
		return regionRefineries[region];
	else
		return utilUnit::UnitList();
}