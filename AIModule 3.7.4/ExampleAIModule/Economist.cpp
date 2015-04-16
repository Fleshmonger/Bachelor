#include "Economist.h"


// Constructor.
Economist::Economist(Accountant * accountant, Producer * producer) :
	accountant(accountant),
	producer(producer),
	homeBase(),
	vassals(),
	unitRegion(),
	regionVassal()
{
}


// Deconstructor.
Economist::~Economist()
{
}

/*
// Fired when the map is analyzed.
void Economist::analyzed()
{
// Code for finding natural expansion.
std::set<BWTA::Region*> regions = BWTA::getRegions();
BWTA::Region * region = *regions.begin();
std::set<BWTA::Region*> reach = region->getReachableRegions();
}
*/

// Creates a vassal region tied to the given depot, designated as the main base.
void Economist::addHomeBase(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
	{
		BWTA::Region * homeRegion = BWTA::getRegion(depot->getPosition());
		addExpansion(depot);
		homeBase = regionVassal[homeRegion];
	}
}


// Creates a new vassal region tied to the given depot.
// TODO Uses dynamic memory.
void Economist::addExpansion(BWAPI::Unit * depot)
{
	// Verify depot.
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
	{
		// Instantiate vassal.
		BWTA::Region * region = BWTA::getRegion(depot->getPosition());
		regionVassal[region] = new Vassal(depot, accountant, producer);
		vassals.insert(regionVassal[region]);
	}
}


void Economist::unitCompleted(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		unitRegion.count(unit) > 0)
	{
		// Verify region.
		BWTA::Region * region = unitRegion[unit];
		if (regionVassal.count(region) > 0)
			regionVassal[region]->unitCompleted(unit);
	}
}


void Economist::unitCreated(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		utilUnit::isOwned(unit))
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(unit->getPosition());
		if (regionVassal.count(region) > 0)
			regionVassal[region]->unitCreated(unit);

		// Add entry.
		unitRegion[unit] = region;
	}
}


void Economist::unitDestroyed(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		unitRegion.count(unit) > 0)
	{
		// Verify region.
		BWTA::Region * region = unitRegion[unit];
		if (regionVassal.count(region) > 0)
			regionVassal[region]->unitDestroyed(unit);

		// Remove entry.
		unitRegion.erase(unit);
	}
}


// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Update vassals.
	BOOST_FOREACH(Vassal * vassal, vassals)
	{
		// Update vassal.
		vassal->update();

		// Train workers.
		//TODO Make train workers in nearby depots if necessary.
		if (vassal->workforce() < vassal->mineralFields() * MINERAL_SATURATION)
			producer->scheduleTraining(UNIT_WORKER, vassal->getDepot());
	}

	if (homeBase)
	{
		// Order a supply building if more supply is needed and none are being constructed.
		if (accountant->supply() < MIN_SUPPLY &&
			homeBase->scheduled(UNIT_SUPPLY) == 0)
			homeBase->build(UNIT_SUPPLY);
	}



}