#include "Planner.h"


// Constructor.
Planner::Planner(Geologist * geologist, Landlord * landlord, Recruiter * recruiter, Architect * architect, Settler * settler) :
	geologist(geologist),
	landlord(landlord),
	recruiter(recruiter),
	architect(architect),
	settler(settler),
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
	// Verify main base.
	Vassal * main = landlord->getMain();
	if (main &&
		main->getDepot())
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
	enqueue(unitType, BASE_ANY);
}


// Enqueues an order at the end of the build-order.
void Planner::enqueue(BWAPI::UnitType unitType, Base base)
{
	buildOrder.push_back(std::make_pair(unitType, base));
}


// Returns whether the type is queued.
bool Planner::contains(BWAPI::UnitType unitType)
{
	BOOST_FOREACH(Build build, buildOrder)
		if (build.first = unitType)
			return true;
	return false;
}


// Returns whether the build-order is empty.
bool Planner::empty()
{
	return buildOrder.empty();
}


// Attempts to schedule the production of a unit and returns whether successful.
bool Planner::produce(Build build)
{
	// Verify type.
	BWAPI::UnitType unitType = build.first;
	if (unitType &&
		!utilUnit::isMisc(unitType))
	{
		// Building check.
		if (unitType.isBuilding())
		{
			// Type check.
			if (unitType.isResourceDepot())
				return settler->buildExpansion();
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
						return architect->scheduleBuilding(unitType, geologist->getGeyserPosition(geyser), vassal->getIdleWorker());
					}
				}

				// Scheduling unsuccesful.
				return false;
			}
			else
			{
				Vassal * vassal;
				if (build.second == BASE_NATURAL)
					vassal = landlord->getNatural();
				else
					vassal = landlord->getMain();
				return architect->scheduleBuilding(unitType, vassal);
			}
		}
		else
			return recruiter->scheduleTraining(unitType);
	}
	else
		return false;
}


// Returns a copy of the build order.
std::list<Build> Planner::getBuildOrder()
{
	return buildOrder;
}