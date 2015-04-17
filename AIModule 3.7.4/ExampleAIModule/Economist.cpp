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
	std::set<Vassal*> vassals = landlord->getVassals();
	BOOST_FOREACH(Vassal * vassal, vassals)
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
		if (depot)
		{
			// Build supply as needed.
			if (accountant->supply() < MIN_SUPPLY &&
				architect->scheduled(UNIT_SUPPLY) == 0)
				architect->scheduleBuild(UNIT_SUPPLY, headquarters->getDepot()->getTilePosition());
		}
	}
}