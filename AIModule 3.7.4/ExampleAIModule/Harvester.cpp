#include "Harvester.h"


// Constructor.
Harvester::Harvester(WorkerManager * workerManager) :
	workerManager(workerManager),
	minerals(),
	minerTargets(),
	mineralMiners()
{
}


// Deconstructor.
Harvester::~Harvester()
{
}


// Adds a mineral for harvesting.
void Harvester::addMineral(BWAPI::Unit * mineral)
{
	if (mineral &&
		mineral->exists() &&
		mineral->getType().isMineralField())
	{
		minerals.push_front(mineral);
		mineralMiners[mineral] = utilUnit::UnitSet();
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
		mineralMiners[mineral].erase(miner);

		// Reprioritize.
		//TODO Remove is slow.
		minerals.remove(mineral);
		minerals.push_front(mineral);

		// Verify distribution.
		BWAPI::Unit * mineralBack = minerals.back();
		if (mineralMiners[mineral].size() + 1 < mineralMiners[mineralBack].size())
		{
			// Redistribute miners.
			BWAPI::Unit * newMiner = *mineralMiners[mineralBack].begin();
			mineralMiners[mineralBack].erase(newMiner);
			mineralMiners[mineral].insert(newMiner);

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
	// Find the mineral.
	utilUnit::UnitList::iterator it = minerals.begin(), end = minerals.end();
	while (it != end)
	{
		if (*it == mineral)
		{
			// Remove miners.
			BOOST_FOREACH(BWAPI::Unit * miner, mineralMiners[mineral])
				minerTargets.erase(miner);

			// Remove the mineral.
			minerals.erase(it);
			mineralMiners.erase(mineral);

			return;
		}
		else
			++it;
	}
}


// Verifies and commands workers to mine minerals.
void Harvester::harvest()
{
	BWAPI::Broodwar->drawTextScreen(200, 80, "Miners: %d", minerTargets.size());

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
				if (contains(miner))
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


// Returns the amount of minerals.
unsigned int Harvester::mineralFields()
{
	return minerals.size();
}


// Returns whether the miner is in the miner pool.
bool Harvester::contains(BWAPI::Unit * miner)
{
	return minerTargets.count(miner) > 0;
}