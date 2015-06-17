#include "Despot.h"


// Constructor
Despot::Despot(Accountant * accountant, Archivist * archivist, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Strategist * strategist, Settler * settler, Planner * planner, Economist * economist) :
	accountant(accountant),
	archivist(archivist),
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
	architect(architect),
	settler(settler),
	planner(planner),
	economist(economist),
	strategist(strategist),
	expand(false)
{
	// Load default opening build-order.
	planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner->enqueue(BWAPI::UnitTypes::Protoss_Pylon);

	// Load opponent specific build-order.
	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();
	if (enemyRace == BWAPI::Races::Terran)
	{
		/*
		// Terran.
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Nexus);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Gateway);
		*/
	}
	else if (enemyRace == BWAPI::Races::Protoss)
	{
		/*
		// Protoss.
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Nexus);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Gateway);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Gateway);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Probe);
		planner->enqueue(BWAPI::UnitTypes::Protoss_Pylon);
		*/
	}
	else if (enemyRace == BWAPI::Races::Zerg)
	{
		// Zerg.
	}
	else
	{
		// Random.
	}
}


// Deconstructor
Despot::~Despot()
{
}


// Executes build-order or auto-pilot.
void Despot::update()
{
	// Build-order end check.
	if (planner->empty())
	{
		// Execute auto-pilot.
		strategist->update();
		economist->update();
	}

	// Expand logic check.
	if (!expand)
	{
		// Iterate through enemy buildings
		BOOST_FOREACH(BWAPI::Unit * enemy, archivist->getBuildings())
		{
			// Defense check.
			BWAPI::UnitType unitType = archivist->getType(enemy);
			if (utilUnit::isTurret(unitType) ||
				unitType == BWAPI::UnitTypes::Terran_Bunker ||
				unitType == BWAPI::UnitTypes::Zerg_Evolution_Chamber)
			{
				// Activate expansion behavior.
				expand = true;
				break;
			}
		}
	}

	// Expansion check.
	if (expand &&												// Verify expansion behavior activated.
		economist->isSaturated() &&								// Verify saturation.
		!planner->contains(BWAPI::UnitTypes::Protoss_Nexus) &&	// Verify not currently expanding.
		!accountant->isScheduled(BWAPI::UnitTypes::Protoss_Nexus) &&
		strategist->isDefended() &&								// Verify bases are defended.
		settler->nextExpansion())								// Verify expansion is possible.
		planner->enqueue(BWAPI::UnitTypes::Protoss_Nexus);

	// Update vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		BWTA::Region * region = vassal->getRegion();
		unsigned int
			minerals = gatherer->getMinerals(region).size(),
			minMiners = minerals * MIN_MINERAL_SATURATION,
			desiredHarvesters = gatherer->getRefineries(region).size() * REFINERY_SATURATION;

		// Harvesters saturation check.
		//TODO Cleanup/Simplify.
		utilUnit::UnitSet harvesters = vassal->getEmployed(TASK_HARVEST);
		if (harvesters.size() < desiredHarvesters)
		{
			// Verify minimum miner criteria.
			if (minMiners < vassal->workforce() - harvesters.size())
			{
				// Assign excess miners to harvesting.
				int excess = std::min(vassal->workforce() - harvesters.size() - minMiners, desiredHarvesters - harvesters.size());
				for (; excess > 0; excess--)
					vassal->employWorker(vassal->getIdleWorker(), TASK_HARVEST);
			}
		}
		else if (harvesters.size() > desiredHarvesters)
		{
			// Dismiss excess harvesters to mining.
			int excess = harvesters.size() - desiredHarvesters;
			utilUnit::UnitSet::iterator it = harvesters.begin();
			for (; excess > 0; excess--, it++)
				vassal->employWorker(*it, TASK_MINE);
		}

		// Assign idle workers to mining.
		vassal->employWorkers(vassal->getEmployed(TASK_IDLE), TASK_MINE);
	}

	// Command gather.
	gatherer->gather();
}


// Returns whether expanding behavior is activated.
bool Despot::isExpanding()
{
	return expand;
}