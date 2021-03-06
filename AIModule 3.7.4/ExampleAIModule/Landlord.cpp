#include "Landlord.h"


// Constructor.
Landlord::Landlord() :
	main(),
	natural(),
	vassals(),
	unitRegion(),
	regionVassal()
{
}


// Deconstructor.
//TODO Deconstruct vassals.
Landlord::~Landlord()
{
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

		// Assign main or natural if there is none.
		if (!main)
			main = vassal;
		else if (!natural)
			natural = vassal;
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

		// Add entry.
		unitRegion[depot] = region;
	}
}


// Adds a worker to the related vassal.
void Landlord::addWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (worker)
		addWorker(worker, BWTA::getRegion(worker->getPosition()));
}


// Adds a worker to the region's related vassal.
void Landlord::addWorker(BWAPI::Unit * worker, BWTA::Region * region)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker())
	{
		// Verify vassal or create new.
		if (!contains(region))
			newVassal(region);

		// Add worker.
		regionVassal[region]->addWorker(worker);

		// Add entry.
		unitRegion[worker] = region;
	}
}


// Adds a worker to the region's related vassal, and removes it from its current.
void Landlord::moveWorker(BWAPI::Unit * worker, BWTA::Region * region)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker())
	{
		// Verify vassal or create new.
		if (!contains(region))
			newVassal(region);

		// Remove worker from old vassal.
		if (contains(worker))
			regionVassal[unitRegion[worker]]->removeWorker(worker);

		// Add worker to new vassal.
		regionVassal[region]->addWorker(worker);
		unitRegion[worker] = region;
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
		BWTA::Region * region = unitRegion[worker];
		if (contains(region))
			regionVassal[region]->employWorker(worker, task);
	}
}


// Removes a depot from the related vassal.
void Landlord::removeDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (depot &&
		contains(depot))
	{
		// Verify vassal.
		BWTA::Region * region = unitRegion[depot];
		if (contains(region))
			regionVassal[region]->clearDepot();
	}
}


// Removes a worker from the related vassal.
void Landlord::removeWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (contains(worker))
	{
		// Verify vassal.
		BWTA::Region * region = unitRegion[worker];
		if (contains(region))
			regionVassal[region]->removeWorker(worker);

		// Remove entry.
		unitRegion.erase(worker);
	}
}


// Returns whether a unit is recorded.
bool Landlord::contains(BWAPI::Unit * unit)
{
	return unitRegion.count(unit) > 0;
}


// Returns whether a region is recorded.
bool Landlord::contains(BWTA::Region * region)
{
	return regionVassal.count(region) > 0;
}


// Returns the harvesting area in the region.
utilMap::Zone Landlord::getHarvestingZone(BWTA::Region * region)
{
	if (contains(region))
		return regionVassal[region]->getHarvestingZone();
	else
		return utilMap::Zone();
}


// Returns a pointer to the main base vassal.
Vassal * Landlord::getMain()
{
	return main;
}


// Returns a pointer to the natural expansion vassal.
Vassal * Landlord::getNatural()
{
	return natural;
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


// Returns the depot related to the region.
BWAPI::Unit * Landlord::getDepot(BWTA::Region * region)
{
	if (contains(region))
		return regionVassal[region]->getDepot();
	else
		return NULL;
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