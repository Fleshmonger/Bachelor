#include "Vassal.h"


// Constructor
Vassal::Vassal() :
	depot(),
	workerManager(),
	harvester(&workerManager)
{
}


// Destructor
Vassal::~Vassal()
{
}


// Adds a worker to the workerManager.
void Vassal::addWorker(BWAPI::Unit * worker)
{
	workerManager.addWorker(worker);
}


// Employs a worker in the workerManager.
void Vassal::employWorker(BWAPI::Unit * worker, Task task)
{
	workerManager.employWorker(worker, task);
}


// Removes a worker from the workerManager.
void Vassal::removeWorker(BWAPI::Unit * worker)
{
	workerManager.removeWorker(worker);
	harvester.removeMiner(worker);
}


// Designates the depot in the region.
void Vassal::setDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (utilUnit::isOwned(depot) &&
		depot->exists() &&
		depot->getType().isResourceDepot())
	{
		// Set depot.
		this->depot = depot;

		// Designate minerals.
		BWTA::Region * region = BWTA::getRegion(depot->getPosition());
		BOOST_FOREACH(BWAPI::Unit * mineral, BWAPI::Broodwar->getStaticMinerals())
			if (utilUnit::inRegion(mineral->getPosition(), region))
				harvester.addMineral(mineral);
	}
}


// Commands idle workers to harvest.
void Vassal::harvest()
{
	harvester.harvest();
}


// Updates local managers.
void Vassal::update()
{
	workerManager.update();
}


// Returns the amount of minerals in the region.
unsigned int Vassal::mineralFields()
{
	return harvester.getMinerals().size();
}


// Returns the amount of workers.
unsigned int Vassal::workforce()
{
	return workerManager.workforce();
}


// Returns a zone containing the harvesting area.
utilMap::Zone Vassal::getHarvestingZone()
{
	// Verify depot.
	if (depot &&
		depot->exists())
	{
		// Calculate zone.
		BWAPI::TilePosition depotPos = depot->getTilePosition();
		BWAPI::UnitType depotType = depot->getType();
		int left = depotPos.x(),
			top = depotPos.y(),
			right = depotPos.x() + depotType.tileWidth(),
			bottom = depotPos.y() + depotType.tileHeight();
		BOOST_FOREACH(BWAPI::Unit * mineral, harvester.getMinerals())
		{
			BWAPI::TilePosition mineralPos = mineral->getTilePosition();
			BWAPI::UnitType mineralType = mineral->getType();
			left = std::min(left, mineralPos.x() + mineralType.tileWidth());
			top = std::min(top, mineralPos.y() + mineralType.tileHeight());
			right = std::max(right, mineralPos.x());
			bottom = std::max(bottom, mineralPos.y());
		}
		return utilMap::Zone(left, top, right, bottom);
	}
	else
		return utilMap::Zone();
}


// Returns the designated depot.
BWAPI::Unit * Vassal::getDepot()
{
	return depot;
}


// Returns an idle worker in the region if one exists.
BWAPI::Unit * Vassal::getIdleWorker()
{
	// Search through idle.
	BOOST_FOREACH(BWAPI::Unit * worker, workerManager.getEmployed(TASK_IDLE))
	{
		// Verify worker.
		if (worker->exists() &&
			!worker->isCarryingGas() &&
			!worker->isCarryingMinerals())
			return worker;
	}

	// No idle worker found.
	return NULL;
}


// Returns a copy of pointers of workers employed with the specified task in the region.
utilUnit::UnitSet Vassal::getEmployed(Task task)
{
	return workerManager.getEmployed(task);
}