#include "Planner.h"


// Constructor.
Planner::Planner(Geologist * geologist, Landlord * landlord, Recruiter * recruiter, Settler * settler, Architect * architect) :
	geologist(geologist),
	landlord(landlord),
	recruiter(recruiter),
	settler(settler),
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
				BWTA::Region * region = settler->nextExpansion();
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
				// Iterate through vassals.
				BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
				{
					// Verify geysers.
					BWTA::Region * region = vassal->getRegion();
					utilUnit::UnitSet geysers = geologist->getGeysers(region);
					if (!geysers.empty())
					{
						// Schedule geyser.
						BWAPI::Unit * geyser = *geysers.begin();
						return architect->scheduleBuilding(unitType, geologist->getGeyserPosition(geyser));
					}
				}

				// Scheduling unsuccesful.
				return false;
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


// Returns a copy of the build order.
std::list<BWAPI::UnitType> Planner::getBuildOrder()
{
	return buildOrder;
}