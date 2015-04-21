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
		// Aquire target
		if ((!target || BWAPI::Broodwar->isExplored(target->getTilePosition())))
		{
			target = NULL;
			std::set<BWTA::BaseLocation*> locations = BWTA::getBaseLocations();
			std::set<BWTA::BaseLocation*>::const_iterator
				it = locations.begin(),
				end = locations.end();
			while (it != end)
			{
				if ((*it)->isStartLocation() && !BWAPI::Broodwar->isExplored((*it)->getTilePosition()))
				{
					target = *it;
					break;
				}
				else
					it++;
			}
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
			if (scout)
				scout->move(target->getPosition());
				//utilUnit::command(scout, BWAPI::UnitCommandTypes::Move, target);
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