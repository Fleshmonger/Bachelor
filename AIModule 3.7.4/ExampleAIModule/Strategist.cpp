#include "Strategist.h"


// Constructor
Strategist::Strategist(Landlord * landlord, Recruiter * recruiter, Architect * architect) :
	landlord(landlord),
	recruiter(recruiter),
	architect(architect)
{
}


// Deconstructor
Strategist::~Strategist()
{
}


// Builds facilities and troops.
void Strategist::update()
{
	// Train new troops.
	recruiter->scheduleTraining(INFANTRY_UNIT);

	// Build gateways.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters &&
		headquarters->getDepot() &&
		architect->scheduled(INFANTRY_FACTORY) + recruiter->getFactories(INFANTRY_FACTORY).size() < MAX_FACTORIES)
		architect->scheduleBuilding(INFANTRY_FACTORY, headquarters->getDepot()->getTilePosition());
}