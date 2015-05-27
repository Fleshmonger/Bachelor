#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Settler * settler, Planner * planner) :
	accountant(accountant),
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
	architect(architect),
	settler(settler),
	planner(planner),
	refineries()
{
}


// Deconstructor
Economist::~Economist()
{
}


// Expands workforce and supply.
//TODO Cleanup.
void Economist::update()
{
	// Verify main base.
	Vassal * main = landlord->getMain();
	if (main)
	{
		// Verify mains depot.
		BWAPI::Unit * depot = main->getDepot();
		if (depot &&
			depot->exists())
		{
			// Build supply as needed.
			if (accountant->supply() < MIN_SUPPLY &&
				accountant->scheduled(UNIT_SUPPLY) == 0)
				architect->scheduleBuilding(UNIT_SUPPLY, main);
		}
	}

	// Iterate through vassals.
	bool expand = true;
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Verify vassal.
		if (vassal->getDepot())
		{
			// Calculate values.
			BWTA::Region * region = vassal->getRegion();
			unsigned int
				minerals = gatherer->getMinerals(region).size(),
				refineries = gatherer->getRefineries(region).size(),
				workers = vassal->workforce();
			if (vassal->getDepot()->isTraining())
				workers++;

			// Saturation check.
			if (workers < minerals * MAX_MINERAL_SATURATION + refineries * REFINERY_SATURATION)
			{
				// Train additional workers.
				recruiter->scheduleTraining(UNIT_WORKER, vassal->getDepot());

				// Expansion check.
				if (workers < minerals * MIN_MINERAL_SATURATION + refineries * REFINERY_SATURATION)
					expand = false;
			}
		}
	}

	// Expansion check.
	if (expand &&
		!planner->contains(BWAPI::UnitTypes::Protoss_Nexus) &&
		!accountant->isScheduled(BWAPI::UnitTypes::Protoss_Nexus))
		planner->enqueue(BWAPI::UnitTypes::Protoss_Nexus);
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