#include "Vassal.h"


// Constructor
Vassal::Vassal(BWTA::Region * region) :
	depot(),
	region(region),
	workerManager(),
	harvester(&workerManager)
{
	// Designate minerals.
	//TODO Check with getMinerals instead of static.
	BOOST_FOREACH(BWTA::BaseLocation * location, region->getBaseLocations())
		BOOST_FOREACH(BWAPI::Unit * mineral, location->getStaticMinerals())
			harvester.addMineral(mineral);
}


// Destructor
Vassal::~Vassal()
{
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


// Designates the depot in the region.
void Vassal::setDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (utilUnit::isOwned(depot) &&
		depot->exists() &&
		depot->getType().isResourceDepot())
		this->depot = depot;
}


// Adds a refinery to the harvester.
void Vassal::addRefinery(BWAPI::Unit * refinery)
{
	BWAPI::Broodwar->sendText("Refinery added.");
	harvester.addRefinery(refinery);
}


// Removes a refinery from the harvester.
void Vassal::removeRefinery(BWAPI::Unit * refinery)
{
	harvester.removeRefinery(refinery);
}


// Adds a worker to the workerManager.
void Vassal::addWorker(BWAPI::Unit * worker)
{
	workerManager.addWorker(worker);
}


// Removes a worker from the workerManager.
void Vassal::removeWorker(BWAPI::Unit * worker)
{
	workerManager.removeWorker(worker);
	harvester.removeWorker(worker);
}


// Employs a worker in the workerManager.
void Vassal::employWorker(BWAPI::Unit * worker, Task task)
{
	workerManager.employWorker(worker, task);
}


// Returns the amount of mineral fields in the region.
unsigned int Vassal::minerals()
{
	return harvester.getMinerals().size();
}


// Returns the amount of refineries in the region.
unsigned int Vassal::refineries()
{
	return harvester.getRefineries().size();
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


// Returns the region the Vassal is situated in.
BWTA::Region * Vassal::getRegion()
{
	return region;
}