#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Producer * producer, Architect * architect) :
	accountant(accountant),
	landlord(landlord),
	producer(producer),
	architect(architect)
{
}


// Deconstructor
Economist::~Economist()
{
}


/*
//TODO Move to economist.
// Fired when the map is analyzed.
void Economist::analyzed()
{
// Code for finding natural expansion.
std::set<BWTA::Region*> regions = BWTA::getRegions();
BWTA::Region * region = *regions.begin();
std::set<BWTA::Region*> reach = region->getReachableRegions();
}
*/


// Expands workforce and supply.
void Economist::update()
{
	// Command vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Train miners as needed.
		//TODO Make train workers in nearby depots if necessary.
		if (vassal->workforce() < vassal->mineralFields() * MINERAL_SATURATION)
			producer->scheduleTraining(UNIT_WORKER, vassal->getDepot());

		// Command harvest.
		vassal->harvest();
	}

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

		// Expand.
		//TODO Cleanup.
		if (landlord->getVassals().size() < 2 &&
			architect->scheduled(UNIT_DEPOT) == 0)
		{
			std::set<BWTA::Chokepoint*> borders = headquarters->getRegion()->getChokepoints();
			if (!borders.empty())
			{
				BWTA::Chokepoint * border = *borders.begin();
				BWTA::Region * region;
				if (border->getRegions().first == headquarters->getRegion())
					region = border->getRegions().second;
				else
					region = border->getRegions().first;
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