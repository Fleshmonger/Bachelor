#include "Harvester.h"


// Constructor.
Harvester::Harvester(WorkerManager * workerManager) :
	workerManager(workerManager),
	depot(),
	minerals(),
	minerTargets(),
	mineralMiners()
{
}


// Deconstructor.
Harvester::~Harvester()
{
}


// Fired when the map is analyzed. Reads mineral positions.
void Harvester::analyzed()
{
	BOOST_FOREACH(BWAPI::Unit * mineral, BWTA::getStartLocation(BWAPI::Broodwar->self())->getStaticMinerals())
	{
		minerals.push_front(mineral);
		mineralMiners[mineral] = utilUnit::UnitSet();
	}
}


// Removes a miner from the pool.
void Harvester::removeMiner(BWAPI::Unit * miner)
{
	// Verify miner.
	if (miner &&
		minerTargets.count(miner) > 0)
	{
		// Remove the miner.
		BWAPI::Unit * mineral = minerTargets[miner], * mineralBack = minerals.back();
		minerTargets.erase(miner);
		mineralMiners[mineral].erase(miner);

		// Verify distribution.
		if (mineralMiners[mineral].size() < mineralMiners[mineralBack].size() - 1)
		{
			// Redistribute miners.
			BWAPI::Unit * newMiner = *mineralMiners[mineralBack].begin();
			mineralMiners[mineralBack].erase(newMiner);
			mineralMiners[mineral].insert(newMiner);

			// Rotate prioties.
			minerals.pop_back();
			minerals.push_front(mineralBack);
		}
	}
}


// Removes a mineral and its related miners from the pool.
void Harvester::removeMineral(BWAPI::Unit * mineral)
{
	// Find the mineral.
	utilUnit::UnitList::iterator it = minerals.begin(), end = minerals.end();
	while (it != end)
	{
		if (*it == mineral)
		{
			// Remove the mineral.
			minerals.erase(it);

			// Remove all the miners targets.
			std::map<BWAPI::Unit*, utilUnit::UnitSet>::iterator mineralMinersIt = mineralMiners.find(mineral);
			utilUnit::UnitSet miners = (*mineralMinersIt).second;
			utilUnit::UnitSet::iterator minersIt = miners.begin();
			while (minersIt != miners.end())
			{
				BWAPI::Unit * miner = *minersIt;
				minersIt++;
				minerTargets.erase(miner);
			}

			// Remove the mineral miners.
			mineralMiners.erase(mineralMinersIt);
			return;
		}
		else
			++it;
	}
}


// Designates the current depot for cargo returns.
void Harvester::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
		this->depot = depot;
}

// Verifies and commands workers to mine minerals.
void Harvester::update()
{
	// Verify minerals.
	if (!minerals.empty())
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
				// Aquire mineral.
				BWAPI::Unit * mineral;
				if (minerTargets.count(miner) > 0)
					mineral = minerTargets[miner];
				else
				{
					// Set target mineral.
					mineral = minerals.front();
					minerTargets[miner] = mineral;
					mineralMiners[mineral].insert(miner);

					// Rotate mineral priorities.
					minerals.pop_front();
					minerals.push_back(mineral);
				}

				// Verify mineral.
				if (mineral &&
					mineral->exists())
				{
					// Command miner.
					if (miner->isCarryingGas() || miner->isCarryingMinerals())
						utilUnit::command(miner, BWAPI::UnitCommandTypes::Return_Cargo);
					else
						utilUnit::command(miner, BWAPI::UnitCommandTypes::Gather, mineral);
				}
			}
		}
	}
}


// Returns the maximum amount of active miners.
unsigned int Harvester::minersMax()
{
	return minerals.size() * MINERS_PER_MINERAL;
}