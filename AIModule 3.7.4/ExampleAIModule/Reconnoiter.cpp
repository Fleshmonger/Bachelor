#include "Reconnoiter.h"


// Constructor
Reconnoiter::Reconnoiter(Archivist * archivist, Landlord * landlord) :
	archivist(archivist),
	landlord(landlord),
	target(),
	scout()
{
}


// Deconstructor
Reconnoiter::~Reconnoiter()
{
}


// Scouts and harasses the enemy base.
//TODO Use command.
void Reconnoiter::update()
{
	// Enemy base spotted check.
	if (archivist->getDepots().empty())
	{
		// Finding Target.
		std::set<BWAPI::TilePosition> points = BWAPI::Broodwar->getStartLocations();
		std::set<BWAPI::TilePosition>::iterator it = points.begin();
		while ((!target || BWAPI::Broodwar->isExplored(BWAPI::TilePosition(target))) && it != points.end())
		{
			BWAPI::TilePosition tile = *it;
			if (!BWAPI::Broodwar->isExplored(tile))
				target = BWAPI::Position(tile);
			++it;
		}
		// Scouting
		if (target)
		{
			// Retrieve Scout
			if ((!scout || !scout->exists()) && landlord->getHeadquarters()->workforce() >= MIN_WORKFORCE)
			{
				scout = landlord->getIdleWorker(BWTA::getRegion(landlord->getHeadquarters()->getDepot()->getPosition()));
				landlord->removeWorker(scout);
			}

			// Commanding Scout
			if (scout && scout->getOrderTargetPosition() != target)
				//utilUnit::command(scout, BWAPI::UnitCommandTypes::Move, target);
				scout->move(target);
		}
	}
	else if (scout && scout->exists())
	{
		// Harassing
		utilUnit::UnitSet troops = archivist->getTroops();
		if (!troops.empty() && !scout->isAttacking())
			//utilUnit::command(scout, BWAPI::UnitCommandTypes::Attack_Move, archivist->getPosition(*troops.begin()));
			scout->attack(archivist->getPosition(*troops.begin()));
	}
	else
		scout = NULL;
}