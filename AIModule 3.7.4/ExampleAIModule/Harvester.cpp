#include "Harvester.h"


// Constructor.
Harvester::Harvester(WorkerManager * workerManager) :
	workerManager(workerManager),
	minerals(),
	refineries(),
	minerTargets(),
	resourceMiners()
{
}


// Deconstructor.
Harvester::~Harvester()
{
}


// Adds a mineral for harvesting.
// TODO Verify type, but the mineral is not always visible.
// TODO redesignate workers.
void Harvester::addMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral)
	{
		// Add mineral.
		minerals.push_front(mineral);
		resourceMiners[mineral] = utilUnit::UnitSet();
	}
}


// Adds a refinery for harvesting.
// TODO redesignate workers.
void Harvester::addRefinery(BWAPI::Unit * refinery)
{
	// Verify mineral.
	if (utilUnit::isOwned(refinery) &&
		refinery->exists() &&
		refinery->getType().isRefinery())
	{
		// Add refinery.
		refineries.push_front(refinery);
		resourceMiners[refinery] = utilUnit::UnitSet();
	}
}

// Removes a miner from the pool.
void Harvester::removeMiner(BWAPI::Unit * miner)
{
	// Verify miner.
	if (contains(miner))
	{
		// Remove miner.
		BWAPI::Unit * mineral = minerTargets[miner];
		minerTargets.erase(miner);
		resourceMiners[mineral].erase(miner);

		// Reprioritize.
		//TODO Remove is slow.
		minerals.remove(mineral);
		minerals.push_front(mineral);

		// Verify distribution.
		BWAPI::Unit * mineralBack = minerals.back();
		if (resourceMiners[mineral].size() + 1 < resourceMiners[mineralBack].size())
		{
			// Redistribute miners.
			BWAPI::Unit * newMiner = *resourceMiners[mineralBack].begin();
			resourceMiners[mineralBack].erase(newMiner);
			resourceMiners[mineral].insert(newMiner);

			// Rotate priorities.
			minerals.pop_back();
			minerals.push_front(mineralBack);
		}
	}
}


// Removes a mineral and its related miners from the pool.
//TODO Unused?
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
				BOOST_FOREACH(BWAPI::Unit * miner, resourceMiners[mineral])
					minerTargets.erase(miner);

				// Remove the mineral.
				minerals.erase(it);
				resourceMiners.erase(mineral);

				return;
			}
			else
				it++;
		}
	}
}


// Removes a refinery and its related miners from the pool.
//TODO Unused?
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
				BOOST_FOREACH(BWAPI::Unit * miner, resourceMiners[refinery])
					minerTargets.erase(miner);

				// Remove the refinery.
				refineries.erase(it);
				resourceMiners.erase(refinery);

				return;
			}
			else
				it++;
		}
	}
}


// Verifies and commands workers to mine minerals.
void Harvester::harvest()
{
	// Aquire miners.
	utilUnit::UnitSet miners = workerManager->getEmployed(TASK_IDLE);

	// Command miners.
	BOOST_FOREACH(BWAPI::Unit * miner, miners)
	{
		// Verify miner.
		if (miner &&
			miner->exists())
		{
			// Aquire target.
			BWAPI::Unit * resource;
			if (contains(miner))
				resource = minerTargets[miner];
			else if (!minerals.empty())
			{
				// Set target mineral.
				resource = minerals.front();
				minerTargets[miner] = resource;
				resourceMiners[resource].insert(miner);

				// Rotate mineral priorities.
				minerals.pop_front();
				minerals.push_back(resource);
			}

			// Verify mineral.
			if (resource &&
				resource->exists())
			{
				// Command miner.
				if (miner->isCarryingGas() || miner->isCarryingMinerals())
					utilUnit::command(miner, BWAPI::UnitCommandTypes::Return_Cargo);
				else
					utilUnit::command(miner, BWAPI::UnitCommandTypes::Gather, resource);
			}
		}
	}
}


// Returns whether the miner is in the miner pool.
bool Harvester::contains(BWAPI::Unit * miner)
{
	return minerTargets.count(miner) > 0;
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