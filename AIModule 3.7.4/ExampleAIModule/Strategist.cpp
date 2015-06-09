#include "Strategist.h"


// Constructor
Strategist::Strategist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, CombatJudge * combatJudge, Architect * architect, ArmyManager * armyManager) :
	accountant(accountant),
	landlord(landlord),
	recruiter(recruiter),
	combatJudge(combatJudge),
	architect(architect),
	armyManager(armyManager)
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
	unsigned int desiredFactories = std::min(FACTORY_DEFAULT + FACTORY_BASE * bases, FACTORY_MAX);
	Vassal * main = landlord->getMain();
	if (main &&
		main->getDepot() &&
		accountant->scheduled(INFANTRY_FACTORY) + recruiter->getFactories(INFANTRY_FACTORY).size() < desiredFactories)
		architect->scheduleBuilding(INFANTRY_FACTORY, main);
}


// Returns whether there are enough troops to defend in case of attack.
//TODO Rename.
bool Strategist::isDefended()
{
	return combatJudge->strength(armyManager->getArmy()) >= combatJudge->strength(archivist->getTroops());
}