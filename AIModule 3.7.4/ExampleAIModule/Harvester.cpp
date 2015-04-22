#include "Harvester.h"


// Constructor.
Harvester::Harvester(WorkerManager * workerManager) :
	workerManager(workerManager),
	minerals(),
	refineries(),
	workerTargets(),
	resourceWorkers()
{
}


// Deconstructor.
Harvester::~Harvester()
{
}


// Adds a mineral for harvesting.
// TODO Verify type, but the mineral is not always visible.
// TODO redesignate workers.
// TODO Merge with addRefinery?
void Harvester::addMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral)
	{
		// Add mineral.
		minerals.push_front(mineral);
		resourceWorkers[mineral] = utilUnit::UnitSet();
	}
}


// Adds a refinery for harvesting.
// TODO redesignate workers.
// TODO Merge with addMineral?
void Harvester::addRefinery(BWAPI::Unit * refinery)
{
	// Verify mineral.
	if (utilUnit::isOwned(refinery) &&
		refinery->exists() &&
		refinery->getType().isRefinery())
	{
		// Add refinery.
		refineries.push_front(refinery);
		resourceWorkers[refinery] = utilUnit::UnitSet();
	}
}


// Adds a worker to the miner pool or the harvesting pool.
void Harvester::addWorker(BWAPI::Unit * worker, bool mining)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker))
	{
		// Get resources
		utilUnit::UnitList * resources;
		if (mining)
			resources = &minerals;
		else
			resources = &refineries;

		// Verify resources.
		if (!resources->empty())
		{
			// Set target resource.
			BWAPI::Unit * resource = resources->front();
			workerTargets[worker] = resource;
			resourceWorkers[resource].insert(worker);

			// Rotate mineral priorities.
			resources->pop_front();
			resources->push_back(resource);
		}
	}
}


// Removes a worker from the pool.
void Harvester::removeWorker(BWAPI::Unit * worker)
{
	// Verify miner.
	if (contains(worker))
	{
		// Remove miner.
		BWAPI::Unit * resource = workerTargets[worker];
		workerTargets.erase(worker);
		resourceWorkers[resource].erase(worker);

		// Aquire resource list.
		utilUnit::UnitList * resources;
		if (resource->getType().isMineralField())
			resources = &minerals;
		else
			resources = &refineries;

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


// Removes a mineral and its related miners from the pool.
//TODO Unused?
//TODO Merge with removeRefinery?
void Harvester::removeMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral)
	{
		// Search through minerals.
		utilUnit::UnitList::iterator it = minerals.begin(), end = minerals.end();
		while (it != end)
		{
			// Verify mineral.
			if (*it == mineral)
			{
				// Remove miners.
				BOOST_FOREACH(BWAPI::Unit * miner, resourceWorkers[mineral])
					workerTargets.erase(miner);

				// Remove the mineral.
				minerals.erase(it);
				resourceWorkers.erase(mineral);

				return;
			}
			else
				it++;
		}
	}
}


// Removes a refinery and its related miners from the pool.
//TODO Merge with removeMineral?
void Harvester::removeRefinery(BWAPI::Unit * refinery)
{
	// Verify refinery.
	if (refinery)
	{
		// Search through refineries.
		utilUnit::UnitList::iterator it = refineries.begin(), end = refineries.end();
		while (it != end)
		{
			// Verify entry.
			if (*it == refinery)
			{
				// Remove miners.
				BOOST_FOREACH(BWAPI::Unit * miner, resourceWorkers[refinery])
					workerTargets.erase(miner);

				// Remove the refinery.
				refineries.erase(it);
				resourceWorkers.erase(refinery);

				return;
			}
			else
				it++;
		}
	}
}


// Commands workers to gather resources
void Harvester::harvest()
{
	// Command workers.
	BOOST_FOREACH(BWAPI::Unit * worker, workerManager->getEmployed(TASK_IDLE))
	{
		// Verify miner.
		if (worker &&
			worker->exists())
		{
			// Aquire resource.
			if (!contains(worker))
			{
				if (refineries.empty())
					addWorker(worker, true);
				else
					addWorker(worker, false);
			}
			BWAPI::Unit * resource = workerTargets[worker];

			// Verify mineral.
			if (resource &&
				resource->exists())
			{
				// Command miner.
				if (worker->isCarryingGas() || worker->isCarryingMinerals())
					utilUnit::command(worker, BWAPI::UnitCommandTypes::Return_Cargo);
				else
					utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, resource);
			}
		}
	}
}


// Returns whether the miner is in the miner pool.
bool Harvester::contains(BWAPI::Unit * worker)
{
	return workerTargets.count(worker) > 0;
}


// Returns a copy of the sorted list of mineral pointers.
utilUnit::UnitList Harvester::getMinerals()
{
	return minerals;
}


// Returns a copy of the sorted list of refinery pointers.
utilUnit::UnitList Harvester::getRefineries()
{
	return refineries;
}