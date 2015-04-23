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

	//TODO Early gas harvesting implementation.
	utilUnit::UnitSet geysers = BWTA::getStartLocation(BWAPI::Broodwar->self())->getGeysers();
	if (!geysers.empty())
	{
		BWAPI::Unit * geyser = *geysers.begin();
		architect->scheduleRefinery(UNIT_REFINERY, geyser);
	}

	// Command vassals.
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
	{
		unsigned int
			minimumMiners = vassal->minerals(),
			desiredMiners = vassal->minerals() * MINERAL_SATURATION,
			desiredHarvesters = vassal->refineries() * REFINERY_SATURATION;

		// Train miners as needed.
		if (vassal->workforce() < desiredMiners + desiredHarvesters)
			producer->scheduleTraining(UNIT_WORKER, vassal->getDepot());

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

		// Command mining and harvesting.
		vassal->gather();
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