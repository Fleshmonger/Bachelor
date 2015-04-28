#include "Gatherer.h"


// Constructor.
Gatherer::Gatherer(Landlord * landlord) :
	landlord(landlord),
	regions(),
	regionMinerals(),
	regionRefineries(),
	regionGeysers(),
	workerTargets(),
	resourceWorkers()
{
}


// Deconstructor.
Gatherer::~Gatherer()
{
}


// Verifies resources and commands workers to gather them.
//TODO Transform into update.
//TODO Cleanup.
void Gatherer::gather()
{
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Aquire resources.
		BWTA::Region * region = vassal->getRegion();
		utilUnit::UnitList
			* minerals = &regionMinerals[region],
			* refineries = &regionRefineries[region];

		// Mine minerals.
		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_MINE))
		{
			// Verify worker.
			if (worker &&
				worker->exists())
			{
				// Aquire resource.
				if (contains(worker))
				{
					// Verify resource type.
					BWAPI::Unit * resource = workerTargets[worker];
					if (!resource ||
						!resource->exists())
					{
						// Redesignate.
						removeMineral(resource);
						addMiner(worker, region);
					}
					else if (!resource->getType().isMineralField())
					{
						removeWorker(worker, region);
						addMiner(worker, region);
					}
				}
				else if (!minerals->empty())
					addMiner(worker, region);

				// Verify resource.
				BWAPI::Unit * resource = workerTargets[worker];
				if (resource &&
					resource->exists())
				{
					// Command worker.
					if (worker->isCarryingGas() || worker->isCarryingMinerals())
						utilUnit::command(worker, BWAPI::UnitCommandTypes::Return_Cargo);
					else
						utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, resource);
				}
			}
		}

		// Harvest gas.
		BOOST_FOREACH(BWAPI::Unit * harvester, vassal->getEmployed(TASK_HARVEST))
		{
			//TODO
		}
	}

	/*
	// Command gathering.
	typedef std::pair<BWAPI::Unit*, BWAPI::Unit*> WorkerTarget;
	BOOST_FOREACH(WorkerTarget workerTarget, workerTargets)
	{
		// Verify worker-target.
		BWAPI::Unit
			* worker = workerTarget.first,
			*resource = workerTarget.second;
		if (worker &&
			worker->exists() &&
			resource &&
			resource->exists())
		{
			// Command worker.
			if (worker->isCarryingGas() || worker->isCarryingMinerals())
				utilUnit::command(worker, BWAPI::UnitCommandTypes::Return_Cargo);
			else
				utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, resource);
		}
	}
	*/
}


// Adds a region to the pool.
void Gatherer::addRegion(BWTA::Region * region)
{
	// Verify region.
	if (region &&
		!contains(region))
	{
		// Add region.
		regions.insert(region);
		regionGeysers[region] = utilUnit::UnitSet();
		regionMinerals[region] = utilUnit::UnitList();
		regionRefineries[region] = utilUnit::UnitList();
	}
}


// Adds a geyser to the pool.
void Gatherer::addGeyser(BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (geyser &&
		geyser->exists() &&
		geyser->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(geyser->getPosition());
		if (!contains(region))
			addRegion(region);

		// Add geyser.
		regionGeysers[region].insert(geyser);
	}
}


// Removes a geyser from the pool.
void Gatherer::removeGeyser(BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (geyser)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(geyser->getPosition());
		if (contains(region))
			regionGeysers[region].erase(geyser);
	}
}

// Adds a mineral for mining.
void Gatherer::addMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral &&
		mineral->exists() &&
		mineral->getType().isMineralField())
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(mineral->getPosition());
		if (!contains(region))
			addRegion(region);
		
		// Add mineral.
		regionMinerals[region].push_front(mineral);
		resourceWorkers[mineral] = utilUnit::UnitSet();

		//TODO even saturation.
	}
}


// Removes a mineral and its miners from the pool.
void Gatherer::removeMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(mineral->getPosition());
		if (contains(region))
		{
			// Remove mineral.
			regionMinerals[region].remove(mineral);

			// Remove workers.
			BOOST_FOREACH(BWAPI::Unit * worker, resourceWorkers[mineral])
				workerTargets.erase(worker);
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


// Adds a worker to the harvester pool in the region.
//TODO Code duplication with addMiner.
void Gatherer::addHarvester(BWAPI::Unit * worker, BWTA::Region * region)
{
	// Verify input.
	if (utilUnit::isOwned(worker) &&	// Verify worker.
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker) &&
		contains(region))				// Verify region.
	{
		// Verify refineries.
		utilUnit::UnitList * refineries = &regionRefineries[region];
		if (!refineries->empty())
		{
			// Set target resource.
			BWAPI::Unit * refinery = refineries->front();
			workerTargets[worker] = refinery;
			resourceWorkers[refinery].insert(worker);

			// Rotate mineral priorities.
			refineries->pop_front();
			refineries->push_back(refinery);
		}
	}
}


// Adds a worker to the miner pool or the harvesting pool.
//TODO Code duplication with addHarvester.
void Gatherer::addMiner(BWAPI::Unit * worker, BWTA::Region * region)
{
	// Verify input.
	if (utilUnit::isOwned(worker) &&	// Verify worker.
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker) &&
		contains(region))				// Verify region.
	{
		// Verify minerals.
		utilUnit::UnitList * minerals = &regionMinerals[region];
		if (!minerals->empty())
		{
			// Set target resource.
			BWAPI::Unit * mineral = minerals->front();
			workerTargets[worker] = mineral;
			resourceWorkers[mineral].insert(worker);

			// Rotate mineral priorities.
			minerals->pop_front();
			minerals->push_back(mineral);
		}
	}
}


// Removes a worker from the pool.
//TODO Cleanup.
void Gatherer::removeWorker(BWAPI::Unit * worker, BWTA::Region * region)
{
	// Verify input.
	if (contains(worker) &&	// Verify worker.
		contains(region))				// Verify region.
	{
		// Remove worker.
		BWAPI::Unit * resource = workerTargets[worker];
		workerTargets.erase(worker);
		resourceWorkers[resource].erase(worker);

		// Aquire resource list.
		utilUnit::UnitList * resources;
		if (resource->getType().isMineralField())
			resources = &regionMinerals[region];
		else
			resources = &regionRefineries[region];

		// Reprioritize.
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

			// Rotate priorities.
			resources->pop_back();
			resources->push_front(resourceBack);
		}
	}
}


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


// Returns a zone containing the harvesting area.
//TODO Move back to landlord.
utilMap::Zone Gatherer::harvestingZone(BWTA::Region * region)
{
	// Verify depot.
	BWAPI::Unit * depot = landlord->getDepot(region);
	if (depot &&
		depot->exists())
	{
		// Calculate zone.
		BWAPI::TilePosition depotPos = depot->getTilePosition();
		BWAPI::UnitType depotType = depot->getType();
		int left = depotPos.x(),
			top = depotPos.y(),
			right = depotPos.x() + depotType.tileWidth(),
			bottom = depotPos.y() + depotType.tileHeight();
		BOOST_FOREACH(BWAPI::Unit * mineral, regionMinerals[region])
		{
			BWAPI::TilePosition mineralPos = mineral->getTilePosition();
			BWAPI::UnitType mineralType = mineral->getType();
			left = std::min(left, mineralPos.x() + mineralType.tileWidth());
			top = std::min(top, mineralPos.y() + mineralType.tileHeight());
			right = std::max(right, mineralPos.x());
			bottom = std::max(bottom, mineralPos.y());
		}
		return utilMap::Zone(left, top, right, bottom);
	}
	else
		return utilMap::Zone();
}


// Returns the set of geyser pointers in the region.
utilUnit::UnitSet Gatherer::getGeysers(BWTA::Region * region)
{
	// Verify region.
	if (contains(region))
		return regionGeysers[region];
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