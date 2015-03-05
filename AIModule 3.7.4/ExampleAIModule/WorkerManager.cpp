#include "WorkerManager.h"

// Private

void WorkerManager::insertField(int amount, BWAPI::Unit * mineral)
{
	fields.push_back(std::make_pair(amount, mineral));
	std::push_heap(fields.begin(), fields.end(), Field_Comp());
}

void WorkerManager::popField()
{
	std::pop_heap(fields.begin(), fields.end(), Field_Comp());
	fields.pop_back();
}

// Public

WorkerManager::WorkerManager()
{
	// Local
	depot = NULL;
	fields = std::vector<Field>();
	std::make_heap(fields.begin(), fields.end(), Field_Comp()); // Possibly unneeded?
	idle = std::set<Unit*>();
	harvesters = std::map<BWAPI::Unit*, BWAPI::Unit*>();
	//fields = new std::priority_queue<Field, std::vector<Field>, Field_Comp>();
}

//Unused deconstructor
WorkerManager::~WorkerManager()
{
}

// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
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
	idle.erase(worker);
	harvesters.erase(worker);
	// TODO: Update minerals
}

// Attempts to assign a worker to harvest and returns whether successful.
// TODO remove field if it does not exist.
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
			harvesters.insert(std::make_pair(worker, field.second));
			// Update the container.
			int amount = field.first + 1;
			BWAPI::Unit * mineral = field.second;
			popField();
			insertField(amount, mineral);
			return true;
		}
	}
	return false;
}

// Returns the amount of workers.
int WorkerManager::workers()
{
	return idle.size() + harvesters.size();
}

// Returns the maximum amount of harvesters
int WorkerManager::harvesterMax()
{
	return fields.size() * MAX_FIELD_HARVESTERS;
}

/*
// Returns the entire worker pool.
// TODO: Remove this.
std::set<Unit*> * WorkerManager::getWorkers()
{
	return workers;
}
*/

// Returns a valid worker or a null pointer if no valid workers are in the pool.
// TODO: Remove code repetition.
BWAPI::Unit * WorkerManager::takeWorker()
{
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
			BWAPI::Unit * worker = (*it).first;
			if (worker->exists() && // Should be unecessary
				worker->isCompleted() && // Should be unecessary
				!worker->isCarryingMinerals() &&
				!worker->isCarryingGas())
			{
				harvesters.erase(it);
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
				it = fields.erase(it); // TODO Does this ruin the heap sorting?
		}
	}
	// Harvest
	if (depot)
	{
		// Verify harvesters.
		{
			std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = harvesters.begin(), end = harvesters.end();
			while (it != end)
			{
				BWAPI::Unit * harvester = (*it).first;
				BWAPI::Unit * mineral = (*it).second;
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
					++it;
				}
				else
				{
					// Harvester is not valid.
					if (harvester && harvester->exists())
						idle.insert(harvester);
					it = harvesters.erase(it);
				}
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