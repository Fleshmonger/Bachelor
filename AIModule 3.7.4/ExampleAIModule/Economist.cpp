#include "Economist.h"


// Constructor
Economist::Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Settler * settler, Strategist * strategist) :
	accountant(accountant),
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
	architect(architect),
	settler(settler),
	strategist(strategist),
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

			// Check saturation.
			if (workers < minerals * MAX_MINERAL_SATURATION + refineries * REFINERY_SATURATION)
				recruiter->scheduleTraining(UNIT_WORKER, vassal->getDepot());
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


// Moves workers to the target region.
//TODO Check recursively neighbors instead.
//TODO Equalize as much as possible between regions.
//TODO Should not move if only 6 or less are available.
void Economist::maynardSlide(BWTA::Region * target)
{
	// Update gatherer.
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
					landlord->moveWorker(worker, target);
					gatherer->removeWorker(worker);
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


bool Economist::isSaturated()
{
	//Iterate through vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		// Check saturation.
		BWTA::Region * region = vassal->getRegion();
		if (vassal->workforce() < gatherer->getMinerals(region).size() * MIN_MINERAL_SATURATION + gatherer->getRefineries(region).size() * REFINERY_SATURATION)
			return false;
	}
	return true;
}