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
	if (headquarters)
	{
		// Verify depot.
		BWAPI::Unit * depot = headquarters->getDepot();
		if (depot)
		{
			// Execute build-order.
			while (!buildOrder.empty())
			{
				bool result = false;

				BWAPI::UnitType unitType = buildOrder.front();
				if (unitType.isBuilding())
					result = architect->scheduleBuilding(unitType, depot->getTilePosition());
				else
					result = recruiter->scheduleTraining(unitType);

				if (result)
					buildOrder.pop_front();
				else
					break;
			}
		}
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