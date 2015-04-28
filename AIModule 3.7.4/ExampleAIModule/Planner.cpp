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
				return false;
				//TODO Build depot
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