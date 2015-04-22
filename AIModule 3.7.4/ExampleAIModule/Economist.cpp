#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Producer * producer, Architect * architect) :
	accountant(accountant),
	landlord(landlord),
	producer(producer),
	architect(architect),
	refineries()
{
}


// Deconstructor
Economist::~Economist()
{
}


// Expands workforce and supply.
void Economist::update()
{
	// Monitor refineries.
	utilUnit::UnitList::iterator it = refineries.begin(), end = refineries.end();
	while (it != end)
	{
		// Verify refinery.
		BWAPI::Unit * refinery = *it;
		if (refinery->isCompleted() ||
			!refinery->exists())
		{
			// Completion check.
			if (refinery->isCompleted())
				landlord->addRefinery(refinery);

			// Remove refinery.
			it = refineries.erase(it);
		}
		else
			it++;
	}

	/*
	// Command vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Train miners as needed.
		//TODO Make train workers in nearby depots if necessary.
		if (vassal->workforce() < vassal->minerals() * MINERAL_SATURATION)
			producer->scheduleTraining(UNIT_WORKER, vassal->getDepot());

		// Command harvest.
		vassal->harvest();
	}
	*/

	// Verify headquarters.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters)
	{
		// Verify headquarters depot.
		BWAPI::Unit * depot = headquarters->getDepot();
		if (depot &&
			depot->exists())
		{
			// Build supply as needed.
			if (accountant->supply() < MIN_SUPPLY &&
				architect->scheduled(UNIT_SUPPLY) == 0)
				architect->scheduleBuilding(UNIT_SUPPLY, headquarters->getDepot()->getTilePosition());
		}

		// Expansion check.
		if (landlord->getVassals().size() < 2 &&
			architect->scheduled(UNIT_DEPOT) == 0)
		{
			// Verify expansion.
			BWTA::Region * region = nextExpansion();
			if (region)
			{
				std::set<BWTA::BaseLocation*> locations = region->getBaseLocations();
				if (!locations.empty())
				{
					BWTA::BaseLocation * location = *locations.begin();
					architect->scheduleBuilding(UNIT_DEPOT, location->getTilePosition(), landlord->getIdleWorker(headquarters->getRegion()));
				}
			}
		}
	}
}


// Adds a refinery for monitoring or the landlord.
void Economist::addRefinery(BWAPI::Unit * refinery)
{
	// Verify refinery.
	if (utilUnit::isOwned(refinery) &&
		refinery->exists() &&
		refinery->getType().isRefinery())
	{
		// Completion check.
		if (refinery->isCompleted())
			landlord->addRefinery(refinery);
		else
			refineries.push_back(refinery);
	}
}


// Returns the next region for expansion.
BWTA::Region * Economist::nextExpansion()
{
	// Verify headquarters.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters &&
		headquarters->getRegion())
	{
		// Search through chokepoints.
		BOOST_FOREACH(BWTA::Chokepoint * border, landlord->getHeadquarters()->getRegion()->getChokepoints())
		{
			// Verify chokepoint.
			BWTA::Region * region;
			if (border->getRegions().first == headquarters->getRegion())
				region = border->getRegions().second;
			else
				region = border->getRegions().first;
			if (!region->getBaseLocations().empty())
				return region;
		}
	}

	// No candidate found.
	return NULL;
}