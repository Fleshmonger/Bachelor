#include "Strategist.h"


// Constructor
Strategist::Strategist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Architect * architect) :
	accountant(accountant),
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
	recruiter->scheduleTraining(BWAPI::UnitTypes::Protoss_Zealot);
	//recruiter->scheduleTraining(BWAPI::UnitTypes::Protoss_Dragoon);

	// Count valid vassals.
	int bases = 0;
	BOOST_FOREACH(Vassal * vassal, landlord->getVassals())
		if (vassal->getDepot() &&
			vassal->getDepot()->exists())
			bases++;

	// Build gateways.
	unsigned int desiredFactories = FACTORY_DEFAULT + FACTORY_BASE * bases;
	Vassal * main = landlord->getMain();
	if (main &&
		main->getDepot() &&
		accountant->scheduled(INFANTRY_FACTORY) + recruiter->getFactories(INFANTRY_FACTORY).size() < desiredFactories)
		architect->scheduleBuilding(INFANTRY_FACTORY, main);
}