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
	assignWorker(worker);
}

// Removes a worker from the worker pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	if (idle.count(worker) == 1)
		idle.erase(worker);
	else if (miners.count(worker) == 1)
		removeMiner(worker);
}

void WorkerManager::removeMiner(BWAPI::Unit * miner)
{
	// Validate miner.
	if (miner)
	{
		// Validate mineral.
		BWAPI::Unit * mineral = mining[miner];
		if (mineral)
		{
			// Update minerals.
			utilUnit::UnitList::iterator it = minerals.begin();
			while (it != minerals.end())
			{
				if (*it == mineral)
				{
					minerals.erase(it);
					break;
				}
				else
					++it;
			}
			minerals.push_front(mineral);
			mineralMiners[mineral].erase(miner);
			//--mineralSaturation[mineral];
			// Even miner distribution.
			if (mineralMiners[mineral].size() < mineralMiners[minerals.back()].size() - 1)
			{
				BWAPI::Unit * newMiner = *mineralMiners[minerals.back()].begin();
				mineralMiners[minerals.back()].erase(newMiner);
				mineralMiners[mineral].insert(newMiner);
			} // Closure: Even distribution.
		} // Closure: Valid mineral.
		// Remove miner.
		miners.erase(miner);
		mining.erase(miner);
	} // Closure: Valid miner.
}

// Adds a harvestable mineral to the list of fields.
void WorkerManager::addMineral(BWAPI::Unit * mineral)
{
	minerals.push_front(mineral);
	//mineralSaturation[mineral] = 0;
	mineralMiners[mineral] = utilUnit::UnitSet();
}

// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}

// Gathers resources with all available workers.
void WorkerManager::update()
{
	// Validate minerals.
	{
		utilUnit::UnitList::iterator it = minerals.begin();
		while (it != minerals.end())
		{
			BWAPI::Unit * mineral = *it;
			if (!mineral ||
				!mineral->exists())
				it = minerals.erase(it);
			else
				++it;
		}
	}

	// Command mining.
	if (depot)
	{
		// Command miners.
		{
			utilUnit::UnitSet::iterator it = miners.begin();
			while (it != miners.end())
			{
				BWAPI::Unit * miner = *it;
				BWAPI::Unit * mineral = mining[miner];
				++it;
				// Validate miner.
				if (miner &&
					miner->exists() &&
					mineral &&
					mineral->exists())
				{
					// Command miner.
					if (miner->isCarryingMinerals() || miner->isCarryingGas())
						utilUnit::command(miner, BWAPI::UnitCommandTypes::Return_Cargo, depot);
					else
						utilUnit::command(miner, BWAPI::UnitCommandTypes::Gather, mineral);
				}
				else
				{
					removeMiner(miner);
				}
			}
		}
		// Assign idle workers.
		{
			utilUnit::UnitSet::iterator it = idle.begin();
			while (it != idle.end())
			{
				BWAPI::Unit * worker = *it;
				++it;
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
		}
	}
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
		worker->exists())
	{
		// Validate prioritized mineral.
		BWAPI::Unit * mineral = minerals.front();
		if (mineral &&
			mineral->exists() &&
			//mineralSaturation[mineral] < MINERAL_SATURATION_MAX)
			mineralMiners[mineral].size() < MINERAL_SATURATION_MAX)
		{
			minerals.pop_front();
			minerals.push_back(mineral);
			miners.insert(worker);
			mining[worker] = mineral;
			//++mineralSaturation[mineral];
			mineralMiners[mineral].insert(worker);
			utilUnit::command(worker, BWAPI::UnitCommandTypes::Gather, mineral);
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
			++it;
		}
	}
	// Find the worker in the harvester pool.
	{
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = mining.begin();
		while (it != mining.end())
		{
			BWAPI::Unit * worker = (*it).first, * mineral = (*it).second;
			if (worker &&
				worker->exists() &&
				!worker->isCarryingMinerals() &&
				!worker->isCarryingGas())
			{
				removeWorker(worker);
				return worker;
			}
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