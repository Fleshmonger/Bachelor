#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect) :
	accountant(accountant),
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
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
	// Iterate through vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		BWTA::Region * region = vassal->getRegion();
		unsigned int
			minimumMiners = gatherer->getMinerals(region).size(),
			desiredMiners = minimumMiners * MINERAL_SATURATION,
			desiredHarvesters = gatherer->getRefineries(region).size() * REFINERY_SATURATION;

		// Train miners as needed.
		if (vassal->workforce() + accountant->scheduled(UNIT_WORKER) < desiredMiners + desiredHarvesters)
			recruiter->scheduleTraining(UNIT_WORKER, vassal->getDepot());
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
				accountant->scheduled(UNIT_SUPPLY) == 0)
				architect->scheduleBuilding(UNIT_SUPPLY, headquarters);
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
			gatherer->addRefinery(refinery);
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