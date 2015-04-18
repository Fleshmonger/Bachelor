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
Landlord::~Landlord()
{
}


// Creates a vassal region tied to the given depot, designated as the main base.
void Landlord::addHeadquarters(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
	{
		BWTA::Region * homeRegion = BWTA::getRegion(depot->getPosition());
		addExpansion(depot);
		headquarters = regionVassal[homeRegion];
	}
}


// Creates a new vassal region tied to the given depot.
// TODO Uses dynamic memory.
void Landlord::addExpansion(BWAPI::Unit * depot)
{
	// Verify depot.
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
	{
		// Instantiate vassal.
		BWTA::Region * region = BWTA::getRegion(depot->getPosition());
		regionVassal[region] = new Vassal(depot);
		vassals.insert(regionVassal[region]);
	}
}


// Adds a worker to the related vassal.
void Landlord::addWorker(BWAPI::Unit * worker)
{
	// Verify unit.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker())
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(worker->getPosition());
		if (contains(region))
			regionVassal[region]->addWorker(worker);

		// Add entry.
		workerRegion[worker] = region;
	}
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


// Unemploys a worker from the related vassal.
void Landlord::dismissWorker(BWAPI::Unit * worker)
{
	employWorker(worker, TASK_IDLE);
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


// Updates all vassals.
//TODO Move harvesting to separate economist class.
//TODO distribute workers between regions.
void Landlord::update()
{
	// Update vassals.
	BOOST_FOREACH(Vassal * vassal, vassals)
		vassal->update();
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