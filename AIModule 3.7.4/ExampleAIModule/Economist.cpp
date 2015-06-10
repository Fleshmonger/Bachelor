#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Settler * settler, Strategist * strategist, Planner * planner) :
	accountant(accountant),
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
	architect(architect),
	settler(settler),
	strategist(strategist),
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
			if (BWAPI::Broodwar->self()->supplyTotal() < MAX_SUPPLY &&
				accountant->supply() < MIN_SUPPLY &&
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
		!accountant->isScheduled(BWAPI::UnitTypes::Protoss_Nexus) &&
		strategist->isDefended() &&
		settler->nextExpansion()) // Verify expansion is possible.
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


// Moves workers to the target region.
//TODO Check recursively neighbors instead.
//TODO Should not move if only 6 or less are available.
void Economist::maynardSlide(BWTA::Region * target)
{
	// Update gatherer.
	if (!gatherer->contains(target))
		gatherer->addRegion(target);

	// Move workers.
	unsigned int current = 0, max = gatherer->getMinerals(target).size() * MIN_MINERAL_SATURATION;
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Verify region.
		BWTA::Region * region = vassal->getRegion();
		if (region != target)
		{
			// Move free workers.
			int freeMiners = vassal->getEmployed(TASK_MINE).size() + vassal->getEmployed(TASK_IDLE).size() - gatherer->getMinerals(region).size() * MIN_MINERAL_SATURATION;
			while (freeMiners > 0 && current < max)
			{
				// Verify worker.
				BWAPI::Unit * worker = vassal->getIdleWorker();
				if (worker)
				{
					// Move worker.
					vassal->removeWorker(worker);
					gatherer->removeWorker(worker);
					landlord->addWorker(worker, target);
					current++;
					freeMiners--;
				}
				else
					break;
			}
		}

		if (current >= max)
			break;
	}
}
