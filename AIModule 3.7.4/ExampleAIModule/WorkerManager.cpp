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

// Adds a new worker and assigns it.
// TODO merge with assign worker.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	if (worker &&
		worker->exists())
		assignWorker(worker);
}

// Removes a worker from the worker pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	if (worker)
	{
		if (idle.count(worker) == 1)
			idle.erase(worker);
		else if (miners.count(worker) == 1)
			removeMiner(worker);
	}
}

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

// Adds a harvestable mineral to the list of fields.
void WorkerManager::addMineral(BWAPI::Unit * mineral)
{
	if (mineral &&
		mineral->exists())
	{
		minerals.push_front(mineral);
		mineralMiners[mineral] = utilUnit::UnitSet();
	}
}

void WorkerManager::removeMineral(BWAPI::Unit * mineral)
{
	// Find mineral.
	utilUnit::UnitList::iterator it = minerals.begin();
	while (it != minerals.end())
	{
		if (*it == mineral)
		{
			// Update minerals.
			minerals.erase(it);
			// Update miners.
			if (mineralMiners.count(mineral) > 0)
			{
				BOOST_FOREACH(BWAPI::Unit * miner, mineralMiners[mineral])
				{
					miners.erase(miner);
					mining.erase(miner);
					idle.insert(miner);
				}
			}
			// Update mineralMiners.
			mineralMiners.erase(mineral);
			return;
		}
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

// Gathers resources with all available workers.
void WorkerManager::update()
{
	// Validate minerals.
	utilUnit::UnitList::iterator mineralIt = minerals.begin();
	while (mineralIt != minerals.end())
	{
		BWAPI::Unit * mineral = *mineralIt;
		++mineralIt;
		if (!mineral ||
			!mineral->exists())
			removeMineral(mineral);
	}

	// Validate cargo return.
	if (depot &&
		depot->exists())
	{
		// Command miners.
		utilUnit::UnitSet::iterator minerIt = miners.begin();
		while (minerIt != miners.end())
		{
			BWAPI::Unit * miner = *minerIt;
			BWAPI::Unit * mineral = mining[miner];
			++minerIt;
			// Validate miner.
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
			{
				removeMiner(miner);
			}
		}
		// Assign idle workers.
		utilUnit::UnitSet::iterator idleIt = idle.begin();
		while (idleIt != idle.end())
		{
			BWAPI::Unit * worker = *idleIt;
			++idleIt;
			// Validate idle.
			if (worker &&
				worker->exists())
			{
				// Assign to mining.
				if (miners.size() < MINERAL_SATURATION_MAX * minerals.size())
				{
					idle.erase(worker);
					assignWorker(worker);
				}
			}
			else
				idle.erase(worker);
		}
	} // Closure: Valid cargo return.
	else
		depot = NULL;
	// TODO: Else unassign harvesters?
}

// Returns the maximum amount of miners.
int WorkerManager::maxMiners()
{
	return MINERAL_SATURATION_MAX * minerals.size();
}

// Returns the amount of workers.
int WorkerManager::workers()
{
	return idle.size() + mining.size();
}

// Attempts to assign a worker and returns whether successful.
bool WorkerManager::assignWorker(BWAPI::Unit * worker)
{
	// Validate worker.
	if (worker &&
		worker->exists() &&
		!minerals.empty())
	{
		// Validate prioritized mineral.
		BWAPI::Unit * mineral = minerals.front();
		if (mineral &&
			mineral->exists() &&
			mineralMiners.count(mineral) > 0 &&
			mineralMiners[mineral].size() < MINERAL_SATURATION_MAX)
		{
			minerals.pop_front();
			minerals.push_back(mineral);
			miners.insert(worker);
			mining[worker] = mineral;
			mineralMiners[mineral].insert(worker);
			return true;
		}
		else
		{
			idle.insert(worker);
			return false;
		}
	}
	return false;
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