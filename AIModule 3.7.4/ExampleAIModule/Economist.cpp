#include "Economist.h"


// Constructor.
Economist::Economist() :
	accountant(),
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
void Economist::addExpansion(BWAPI::Unit * depot)
{
	// Verify depot.
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
	{
		BWTA::Region * region = BWTA::getRegion(depot->getPosition());
		regionVassal[region] = new Vassal(depot, &accountant);
		vassals.insert(regionVassal[region]);
	}
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


void Economist::unitCompleted(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		unitRegion.count(unit) > 0)
	{
		BWTA::Region * region = unitRegion[unit];

		// Verify region.
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
		BWTA::Region * region = BWTA::getRegion(unit->getPosition());
		unitRegion[unit] = region;

		// Verify region.
		if (regionVassal.count(region) > 0)
			regionVassal[region]->unitCreated(unit);
	}
}


void Economist::unitDestroyed(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		unitRegion.count(unit) > 0)
	{
		BWTA::Region * region = unitRegion[unit];

		// Verify region.
		if (regionVassal.count(region) > 0)
			regionVassal[region]->unitDestroyed(unit);

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
		if (vassal->needMiners())
			vassal->train(PROTOSS_WORKER);
	}

	if (homeBase)
	{
		// Order a supply building if more supply is needed and none are being constructed.
		if (accountant.supply() < MIN_SUPPLY &&
			homeBase->scheduled(BUILD_SUPPLY) == 0)
			homeBase->build(BUILD_SUPPLY);
	}



}