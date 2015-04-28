#include "Despot.h"


// Constructor
Despot::Despot(Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Economist * economist, Strategist * strategist) :
	landlord(landlord),
	recruiter(recruiter),
	gatherer(gatherer),
	architect(architect),
	economist(economist),
	strategist(strategist),
	planner(landlord, recruiter, architect)
{
	planner.enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner.enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner.enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner.enqueue(BWAPI::UnitTypes::Protoss_Probe);
	planner.enqueue(BWAPI::UnitTypes::Protoss_Pylon);
	planner.enqueue(BWAPI::UnitTypes::Protoss_Probe);
}


// Deconstructor
Despot::~Despot()
{
}


// Executes build-order or auto-pilot.
void Despot::update()
{
	// Update build-order.
	planner.update();

	// Build-order end check.
	if (planner.empty())
	{
		// Execute auto-pilot.
		economist->update();
		//strategist->update();
	}

	// Command harvest.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		BWTA::Region * region = vassal->getRegion();
		unsigned int
			minimumMiners = gatherer->getMinerals(region).size(),
			desiredMiners = minimumMiners * MINERAL_SATURATION,
			desiredHarvesters = gatherer->getRefineries(region).size() * REFINERY_SATURATION;

		// Harvesters saturation check.
		//TODO Cleanup/Simplify.
		utilUnit::UnitSet harvesters = vassal->getEmployed(TASK_HARVEST);
		if (harvesters.size() < desiredHarvesters)
		{
			// Verify minimum miner criteria.
			if (minimumMiners < vassal->workforce() - harvesters.size())
			{
				// Assign excess miners to harvesting.
				int excess = std::min(vassal->workforce() - harvesters.size() - minimumMiners, desiredHarvesters - harvesters.size());
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

	// Command harvest.
	gatherer->gather();
}