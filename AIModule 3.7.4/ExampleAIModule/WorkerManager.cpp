#include "WorkerManager.h"

// Constructor
WorkerManager::WorkerManager() :
	depot(NULL),
	minerals(utilUnit::UnitList()),
	idle(utilUnit::UnitSet()),
	miners(utilUnit::UnitSet()),
	mining(std::map<BWAPI::Unit*, BWAPI::Unit*>()),
	mineralMiners(std::map<BWAPI::Unit*, utilUnit::UnitSet>())
{
}

// Deconstructor
WorkerManager::~WorkerManager()
{
}

// Adds a worker to the pool.
// TODO merge with assign worker.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	idle.insert(worker);
}

// Removes a worker from the pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (worker)
	{
		// Find container.
		if (idle.count(worker) > 0)
			idle.erase(worker);
		else if (miners.count(worker) > 0)
			removeMiner(worker);
	}
}

// Adds a miner to the pool and relevant containers.
void WorkerManager::addMiner(BWAPI::Unit * miner)
{
	// Validate miner and minerals.
	if (miner &&
		miner->exists() &&
		!minerals.empty())
	{
		// Assign miner.
		BWAPI::Unit * mineral = minerals.front();
		minerals.pop_front();
		minerals.push_back(mineral);
		miners.insert(miner);
		mining[miner] = mineral;
		mineralMiners[mineral].insert(miner);
	}
}

// Removes a miner from the pool and relevant containers.
// TODO Cleanup.
void WorkerManager::removeMiner(BWAPI::Unit * miner)
{
	// Validate miner.
	if (miner)
	{
		miners.erase(miner);
		// Validate mining.
		if (mining.count(miner) > 0)
		{
			BWAPI::Unit * mineral = mining[miner];
			mining.erase(miner);
			// Validate mineral.
			if (mineral)
			{
				// Update minerals.
				utilUnit::UnitList::iterator it = minerals.begin();
				while (it != minerals.end())
				{
					if (*it == mineral)
					{
						minerals.erase(it);
						minerals.push_front(mineral);
						break;
					}
					else
						++it;
				}
				// Validate mineral miners.
				if (mineralMiners.count(mineral) > 0)
				{
					mineralMiners[mineral].erase(miner);
					// Even miner distribution.
					BWAPI::Unit * mineralBack = minerals.back();
					if (mineralBack &&
						mineralMiners.count(mineralBack) > 0 &&
						mineralMiners[mineral].size() < mineralMiners[mineralBack].size() - 1)
					{
						BWAPI::Unit * newMiner = *mineralMiners[mineralBack].begin();
						mineralMiners[mineralBack].erase(newMiner);
						mineralMiners[mineral].insert(newMiner);
						minerals.pop_back();
						minerals.push_front(mineralBack);
					} // Closure: Even distribution.
				} // Closure: Valid mineral miners.
			} // Closure: Valid mineral.
		} // Closure: Valid mining.
	} // Closure: Valid miner.
}

// Adds a mineral to the mineral pool and related containers.
void WorkerManager::addMineral(BWAPI::Unit * mineral)
{
	// Verify mineral.
	if (mineral &&
		mineral->exists())
	{
		// Add mineral.
		minerals.push_front(mineral);
		mineralMiners[mineral] = utilUnit::UnitSet();
	}
}

// Removes a mineral at the position from the mineral pool and related containers.
// TODO Remove unecessary check.
// TODO Rename second iterator.
void WorkerManager::removeMineral(utilUnit::UnitList::iterator it)
{
	BWAPI::Unit * mineral = *it;
	std::map<BWAPI::Unit*, utilUnit::UnitSet>::iterator mineralMinersIt = mineralMiners.find(mineral);
	// Update containers.
	minerals.erase(it);
	if (mineralMinersIt != mineralMiners.end())
	{
		BOOST_FOREACH(BWAPI::Unit * miner, (*mineralMinersIt).second)
		{
			miners.erase(miner);
			mining.erase(miner);
			idle.insert(miner);
		}
		mineralMiners.erase(mineralMinersIt);
	}
}

// Removes a mineral from the mineral pool and related containers.
void WorkerManager::removeMineral(BWAPI::Unit * mineral)
{
	// Find mineral.
	utilUnit::UnitList::iterator it = minerals.begin();
	while (it != minerals.end())
	{
		if (*it == mineral)
			removeMineral(it);
		else
			it++;
	}
}

// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists())
		this->depot = depot;
}

// Updates mining operations.
void WorkerManager::update()
{
	updateMinerals();
	// Validate cargo return.
	if (depot &&
		depot->exists())
	{
		updateIdle();
		updateMiners();
	}
}

// Verifies and assigns idle workers.
void WorkerManager::updateIdle()
{
	utilUnit::UnitSet::iterator it = idle.begin();
	while (it != idle.end())
	{
		BWAPI::Unit * worker = *it;
		// Verify worker.
		if (worker &&
			worker->exists())
		{
			// Assign worker.
			if (miners.size() < maxMiners())
			{
				// Assign worker as miner.
				it = idle.erase(it);
				addMiner(worker);
			}
			else
				it++;
		}
		else
			it = idle.erase(it);
	}
}

// Verifies minerals.
void WorkerManager::updateMinerals()
{
	// Verify minerals.
	utilUnit::UnitList::iterator it = minerals.begin();
	while (it != minerals.end())
	{
		BWAPI::Unit * mineral = *it;
		++it;
		// Verify mineral.
		if (!mineral ||
			!mineral->exists())
			removeMineral(mineral);
	}
}

// Verifies and commands miners.
// TODO Remove superfloues miners?
void WorkerManager::updateMiners()
{
	// Verify and command miners.
	utilUnit::UnitSet::iterator it = miners.begin();
	while (it != miners.end())
	{
		BWAPI::Unit * miner = *it;
		BWAPI::Unit * mineral = mining[miner];
		it++;
		// Verify miner.
		if (miner &&
			miner->exists() &&
			mineral &&
			mineral->exists())
		{
			// Command miner.
			if (miner->isIdle())
			{
				if (miner->isCarryingMinerals() || miner->isCarryingGas())
					//utilUnit::command(miner, BWAPI::UnitCommandTypes::Return_Cargo, depot);
					miner->returnCargo();
				else
					//utilUnit::command(miner, BWAPI::UnitCommandTypes::Gather, mineral);
					miner->gather(mineral);
			}
		}
		else
			removeMiner(miner);
	}
}

// Returns the maximum amount of miners.
unsigned int WorkerManager::maxMiners()
{
	return MINERAL_SATURATION_MAX * minerals.size();
}

// Returns the amount of workers.
unsigned int WorkerManager::workers()
{
	return idle.size() + mining.size();
}

// Returns a valid worker or a null pointer if no valid workers are in the pool.
// TODO: Remove code repetition.
BWAPI::Unit * WorkerManager::takeWorker()
{
	// Find the worker in the idle pool.
	{
		utilUnit::UnitSet::iterator it = idle.begin();
		while (it != idle.end())
		{
			BWAPI::Unit * worker = *it;
			if (worker &&
				worker->exists() &&
				!worker->isCarryingMinerals() &&
				!worker->isCarryingGas())
			{
				idle.erase(it);
				return worker;
			}
			else
				++it;
		}
	}
	// Find the worker in the harvester pool.
	{
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = mining.begin();
		while (it != mining.end())
		{
			BWAPI::Unit * worker = (*it).first;
			if (worker &&
				worker->exists() &&
				!worker->isCarryingMinerals() &&
				!worker->isCarryingGas())
			{
				removeMiner(worker);
				return worker;
			}
			else
				++it;
		}
	}
	// No available workers was found.
	return NULL;
}

// Returns a copy of the min-saturation sorted list of recorded minerals.
utilUnit::UnitList WorkerManager::getMinerals()
{
	return minerals;
}

// Returns a copy of the mineral miner mapping.
std::map<BWAPI::Unit*, utilUnit::UnitSet> WorkerManager::getMineralMiners()
{
	return mineralMiners;
}