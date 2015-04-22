#include "Landlord.h"


// Constructor.
Landlord::Landlord() :
	headquarters(),
	vassals(),
	workerRegion(),
	regionVassal()
{
}


// Deconstructor.
//TODO Deconstruct vassals.
Landlord::~Landlord()
{
}


// Updates all vassals.
//TODO Move harvesting to separate economist class.
//TODO distribute workers between regions.
void Landlord::update()
{
	// Update vassals.
	BOOST_FOREACH(Vassal * vassal, vassals)
		vassal->update();
}


// Instantiates a new vassal for the region.
void Landlord::newVassal(BWTA::Region * region)
{
	// Verify region.
	if (!contains(region))
	{
		// Instantiate vassal.
		Vassal * vassal = new Vassal(region);
		regionVassal[region] = vassal;
		vassals.insert(vassal);

		// Assign headquarters if there is none.
		if (!headquarters)
			headquarters = vassal;
	}
}


// Designates a depot to the related vassal.
void Landlord::addDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (utilUnit::isOwned(depot) &&
		depot->exists() &&
		depot->getType().isResourceDepot())
	{
		// Verify vassal or create new.
		BWTA::Region * region = BWTA::getRegion(depot->getPosition());
		if (!contains(region))
			newVassal(region);

		// Designate depot.
		regionVassal[region]->setDepot(depot);
	}
}


// Adds a refinery to the related vassal.
void Landlord::addRefinery(BWAPI::Unit * refinery)
{
	// Verify refinery.
	if (utilUnit::isOwned(refinery) &&
		refinery->exists() &&
		refinery->getType().isRefinery())
	{
		// Verify vassal or create new.
		BWTA::Region * region = BWTA::getRegion(refinery->getPosition());
		if (!contains(region))
			newVassal(region);

		// Add refinery.
		regionVassal[region]->addRefinery(refinery);
	}
}


// Removes a refinery from the related vassal.
void Landlord::removeRefinery(BWAPI::Unit * refinery)
{
	// Verify refinery.
	if (utilUnit::isOwned(refinery))
	{
		// Verify vassal.
		BWTA::Region * region = BWTA::getRegion(refinery->getPosition());
		if (contains(region))
			regionVassal[region]->removeRefinery(refinery);
	}
}


// Adds a worker to the related vassal.
void Landlord::addWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker())
	{
		// Verify vassal or create new.
		BWTA::Region * region = BWTA::getRegion(worker->getPosition());
		if (!contains(region))
			newVassal(region);

		// Add worker.
		regionVassal[region]->addWorker(worker);

		// Add entry.
		workerRegion[worker] = region;
	}
}


// Unemploys a worker from the related vassal.
void Landlord::dismissWorker(BWAPI::Unit * worker)
{
	employWorker(worker, TASK_IDLE);
}


// Employs a worker from the related vassal.
void Landlord::employWorker(BWAPI::Unit * worker, Task task)
{
	// Verify worker.
	if (contains(worker))
	{
		// Verify vassal.
		BWTA::Region * region = workerRegion[worker];
		if (contains(region))
			regionVassal[region]->employWorker(worker, task);
	}
}


// Removes a worker from the related vassal.
void Landlord::removeWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (contains(worker))
	{
		// Verify vassal.
		BWTA::Region * region = workerRegion[worker];
		if (contains(region))
			regionVassal[region]->removeWorker(worker);

		// Remove entry.
		workerRegion.erase(worker);
	}
}


// Returns whether a worker is recorded.
bool Landlord::contains(BWAPI::Unit * worker)
{
	return workerRegion.count(worker) > 0;
}


// Returns whether a region is recorded.
bool Landlord::contains(BWTA::Region * region)
{
	return regionVassal.count(region) > 0;
}


// Returns the harvesting zone related to the specified region.
utilMap::Zone Landlord::getHarvestingZone(BWTA::Region * region)
{
	// Verify region.
	if (contains(region))
		return regionVassal[region]->getHarvestingZone();
	else
		return utilMap::Zone();
}


// Returns a pointer to the headquarter vassal.
Vassal * Landlord::getHeadquarters()
{
	return headquarters;
}


// Returns a pointer to the related vassal.
Vassal * Landlord::getVassal(BWTA::Region * region)
{
	if (contains(region))
		return regionVassal[region];
	else
		return NULL;
}


// Returns a set of pointers of all vassals.
std::set<Vassal*> Landlord::getVassals()
{
	return vassals;
}


// Returns an idle worker from the region if one exists.
BWAPI::Unit * Landlord::getIdleWorker(BWTA::Region * region)
{
	// Verify region.
	if (contains(region))
		return regionVassal[region]->getIdleWorker();
	else
		return NULL;
}


// Returns a set of pointers of workers employed with the specified task in the region.
utilUnit::UnitSet Landlord::getEmployed(BWTA::Region * region, Task task)
{
	if (contains(region))
		return regionVassal[region]->getEmployed(task);
	else
		return utilUnit::UnitSet();
}