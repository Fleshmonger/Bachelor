#include "WorkerManager.h"

WorkerManager::WorkerManager() :
	depot(NULL),
	fields(std::vector<Field>()),
	idle(std::set<BWAPI::Unit*>()),
	harvesters(std::map<BWAPI::Unit*, BWAPI::Unit*>())
{
	std::make_heap(fields.begin(), fields.end(), Field_Comp());
}

// Unused deconstructor
WorkerManager::~WorkerManager()
{
}

// Private
// Inserts the field into the heap.
void WorkerManager::insertField(int amount, BWAPI::Unit * mineral)
{
	fields.push_back(std::make_pair(amount, mineral));
	std::push_heap(fields.begin(), fields.end(), Field_Comp());
}

// Private
// Removes the top field.
void WorkerManager::popField()
{
	std::pop_heap(fields.begin(), fields.end(), Field_Comp());
	fields.pop_back();
}

// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}

// Returns the maximum amount of harvesters
int WorkerManager::harvesterMax()
{
	return fields.size() * MAX_FIELD_HARVESTERS;
}

// Returns the amount of workers.
int WorkerManager::workers()
{
	return idle.size() + harvesters.size();
}

// Adds a harvestable mineral to the list of fields.
void WorkerManager::addMineral(BWAPI::Unit * mineral)
{
	insertField(0, mineral);
}

// Adds a new worker and assigns it.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	if (!assignWorker(worker))
		idle.insert(worker);
}

// Removes a worker from the worker pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	if (idle.count(worker) == 1)
		idle.erase(worker);
	else if (harvesters.count(worker) == 1)
	{
		BWAPI::Unit * mineral = harvesters[worker];
		harvesters.erase(worker);
		// Update the fields.
		std::vector<Field>::iterator it = fields.begin(), end = fields.end();
		while (it != end)
		{
			if ((*it).second = mineral)
			{
				int amount = (*it).first;
				fields.erase(it);
				std::sort_heap(fields.begin(), fields.end(), Field_Comp());
				insertField(amount - 1, mineral);
				return;
			}
			else
				++it;
		}
	}
}

/*
// Returns the entire worker pool.
// TODO: Remove this.
std::set<Unit*> * WorkerManager::getWorkers()
{
return workers;
}
*/

// Attempts to assign a worker to harvest and returns whether successful.
// TODO remove field if it does not exist.
// TODO Search through until valid field is found.
bool WorkerManager::assignWorker(BWAPI::Unit * worker)
{
	if (worker &&
		worker->exists() &&
		!fields.empty())
	{
		Field field = fields.front();
		if (field.first < MAX_FIELD_HARVESTERS)
		{
			// Add the worker to harvesters.
			BWAPI::Unit * mineral = field.second;
			harvesters.insert(std::make_pair(worker, mineral));
			// Update the field
			int amount = field.first;
			popField();
			insertField(amount + 1, mineral);
			worker->gather(mineral);
			return true;
		}
	}
	return false;
}

// Returns a valid worker or a null pointer if no valid workers are in the pool.
// TODO: Remove code repetition.
BWAPI::Unit * WorkerManager::takeWorker()
{
	/*
	if (idle.size() > 0)
	{
		BWAPI::Unit * worker = *idle.begin();
		idle.erase(worker);
		//removeWorker(worker);
		return worker;
	}
	else if (harvesters.size() > 0)
	{
		BWAPI::Unit * worker = (*harvesters.begin()).first;
		removeWorker(worker);
		return worker;
	}
	else
		return NULL;
	*/
	// Find the worker in the idle pool.
	{
		std::set<BWAPI::Unit*>::iterator it = idle.begin(), end = idle.end();
		while (it != end)
		{
			BWAPI::Unit * worker = *it;
			if (worker->exists() && // Should be unecessary
				worker->isCompleted() && // Should be unecessary
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
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = harvesters.begin(), end = harvesters.end();
		while (it != end)
		{
			BWAPI::Unit * worker = (*it).first, * mineral = (*it).second;
			if (worker->exists() && // Should be unecessary
				worker->isCompleted() && // Should be unecessary
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

// Gathers resources with all available workers.
void WorkerManager::update()
{
	// Verify minerals.
	{
		std::vector<Field>::iterator it = fields.begin(), end = fields.end();
		while (it != end)
		{
			BWAPI::Unit * mineral = (*it).second;
			if (mineral->exists())
				++it;
			else
			{
				it = fields.erase(it);
				std::sort_heap(fields.begin(), fields.end(), Field_Comp()); // TODO This may be unneeded?
			}
		}
	}
	// Harvest
	if (depot)
	{
		// Update harvesters
		{
			std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = harvesters.begin(), end = harvesters.end();
			while (it != end)
			{
				BWAPI::Unit * harvester = (*it).first;
				BWAPI::Unit * mineral = (*it).second;
				++it;
				// Verify the harvester
				if (harvester &&
					harvester->exists() &&
					mineral &&
					mineral->exists())
				{
					// Harvester AI
					if (harvester->isIdle())
					{
						if (harvester->isCarryingMinerals() || harvester->isCarryingGas())
							harvester->returnCargo();
						else
							harvester->gather(mineral);
					}
				}
				else
					removeWorker(harvester);
			}
		}
		// Assign new harvesters.
		{
			std::set<BWAPI::Unit*>::iterator it = idle.begin(), end = idle.end();
			while (it != end)
			{
				BWAPI::Unit * worker = *it;
				if (worker &&
					worker->exists() &&
					!assignWorker(worker))
					++it;
				else
					it = idle.erase(it);
			}
		}
	}
	// TODO: Else unassign harvesters?
}