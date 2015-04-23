#include "Gatherer.h"


// Constructor.
Gatherer::Gatherer(Taskmaster * taskmaster) :
	taskmaster(taskmaster),
	minerals(),
	refineries(),
	workerTargets(),
	resourceWorkers()
{
}


// Deconstructor.
Gatherer::~Gatherer()
{
}


// Commands workers to gather resources
void Gatherer::gather()
{
	BOOST_FOREACH(BWAPI::Unit * miner, taskmaster->getEmployed(TASK_MINE))
		gather(miner, true);
	BOOST_FOREACH(BWAPI::Unit * harvester, taskmaster->getEmployed(TASK_HARVEST))
		gather(harvester, false);
}


// Commands the worker to gather resources.
void Gatherer::gather(BWAPI::Unit * worker, bool isMining)
{
	// Verify worker.
	if (worker &&
		worker->exists())
	{
		// Aquire resource.
		BWAPI::Unit * resource;
		if (!contains(worker))
			addWorker(worker, isMining);
		else
		{
			// Check resource type.
			resource = workerTargets[worker];
			if (!resource ||
				(isMining && !resource->getType().isMineralField()) ||
				(!isMining && !resource->getType().isRefinery()))
			{
				removeWorker(worker);
				addWorker(worker, isMining);
			}
		}
		resource = workerTargets[worker];

		// Verify resource.
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


// Adds a mineral for harvesting.
// TODO Verify type, but the mineral is not always visible.
// TODO redesignate workers.
// TODO Merge with addRefinery?
void Gatherer::addMineral(BWAPI::Unit * mineral)
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
void Gatherer::addRefinery(BWAPI::Unit * refinery)
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
void Gatherer::addWorker(BWAPI::Unit * worker, bool isMining)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker() &&
		!contains(worker))
	{
		// Get resources
		utilUnit::UnitList * resources;
		if (isMining)
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
void Gatherer::removeWorker(BWAPI::Unit * worker)
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
void Gatherer::removeMineral(BWAPI::Unit * mineral)
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
void Gatherer::removeRefinery(BWAPI::Unit * refinery)
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


// Returns whether the miner is in the miner pool.
bool Gatherer::contains(BWAPI::Unit * worker)
{
	return workerTargets.count(worker) > 0;
}


// Returns a copy of the sorted list of mineral pointers.
utilUnit::UnitList Gatherer::getMinerals()
{
	return minerals;
}


// Returns a copy of the sorted list of refinery pointers.
utilUnit::UnitList Gatherer::getRefineries()
{
	return refineries;
}