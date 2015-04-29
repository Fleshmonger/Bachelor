#include "Planner.h"


// Constructor.
Planner::Planner(Landlord * landlord, Recruiter * recruiter, Architect * architect) :
	landlord(landlord),
	recruiter(recruiter),
	architect(architect),
	buildOrder()
{
}


// Deconstructor.
Planner::~Planner()
{
}


// Updates and executes a build-order.
void Planner::update()
{
	// Verify headquarters.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters &&
		headquarters->getDepot())
	{
		// Execute build-order.
		while (!buildOrder.empty() &&
			produce(buildOrder.front()))
			buildOrder.pop_front();
	}
}


// Enqueues an order at the end of the build-order.
void Planner::enqueue(BWAPI::UnitType unitType)
{
	buildOrder.push_back(unitType);
}


// Returns whether the build-order is empty.
bool Planner::empty()
{
	return buildOrder.empty();
}


// Attempts to schedule the production of a unit and returns whether successful.
bool Planner::produce(BWAPI::UnitType unitType)
{
	// Verify type.
	if (unitType &&
		!utilUnit::isMisc(unitType))
	{
		// Building check.
		if (unitType.isBuilding())
		{
			// Type check.
			if (unitType.isResourceDepot())
			{
				// Build expansion.
				BWTA::Region * region = nextExpansion();
				if (region)
					return architect->scheduleBuilding(
						unitType,
						(*region->getBaseLocations().begin())->getTilePosition(),
						landlord->getHeadquarters()->getIdleWorker());
				else
					return false;
			}
			else if (unitType.isRefinery())
			{
				return false;
				//TODO Build refinery.
			}
			else
				return architect->scheduleBuilding(unitType, landlord->getHeadquarters());
		}
		else
			return recruiter->scheduleTraining(unitType);
	}
	else
		return false;
}

// Returns the next region for expansion.
//TODO Move to separate expander class.
BWTA::Region * Planner::nextExpansion()
{
	// Verify headquarters.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters &&
		headquarters->getRegion())
	{
		// Initialize queue.
		std::list<BWTA::Region*> queue;
		queue.push_front(headquarters->getRegion());

		// Iterate through queue.
		while (!queue.empty())
		{
			// Verify region.
			BWTA::Region * region = queue.front();
			if (region)
			{
				// Check availability.
				if (!landlord->contains(region) &&
					!region->getBaseLocations().empty())
					return region;
				else
				{
					// Add neighbors.
					BOOST_FOREACH(BWTA::Chokepoint * border, region->getChokepoints())
					{
						// Add neighbor.
						std::pair<BWTA::Region*, BWTA::Region*> neighbors = border->getRegions();
						if (neighbors.first == region)
							queue.push_back(neighbors.second);
						else
							queue.push_back(neighbors.first);
					}
				}
			}

			// Remove entry.
			queue.pop_front();
		}
	}

	// No expansion found.
	return NULL;
}


// Returns a copy of the build order.
std::list<BWAPI::UnitType> Planner::getBuildOrder()
{
	return buildOrder;
}