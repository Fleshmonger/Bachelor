#include "Reconnoiter.h"

// Constructor
Reconnoiter::Reconnoiter(Archivist * archivist, WorkerManager * workerManager) :
	archivist(archivist),
	workerManager(workerManager),
	scout(NULL),
	target(BWAPI::TilePositions::None)
{
}

// Unused deconstructor
Reconnoiter::~Reconnoiter()
{
}

// Simulate the reconnoiter AI.
// Retrieves, returns and commands the scout.
void Reconnoiter::update()
{
	if (archivist->getDepots().empty())
	{
		// Finding Target.
		std::set<BWAPI::TilePosition> points = Broodwar->getStartLocations();
		std::set<BWAPI::TilePosition>::iterator it = points.begin();
		while ((!target || Broodwar->isExplored(BWAPI::TilePosition(target))) && it != points.end())
		{
			BWAPI::TilePosition tile = *it;
			if (!Broodwar->isExplored(tile))
				target = BWAPI::Position(tile);
			++it;
		}
		// Scouting
		if (target)
		{
			// Retrieving Scout
			if ((!scout || !scout->exists()) && workerManager->workers() >= MINIMUM_WORKERS)
				scout = workerManager->takeWorker();
			// Commanding Scout
			if (scout && scout->getOrderTargetPosition() != target)
			{
				scout->move(target);
			}
		}
	}
	else if (scout && scout->exists())
	{
		// Harassing
		std::set<BWAPI::Unit*> troops = archivist->getTroops();
		if (!troops.empty() && !scout->isAttacking())
			scout->attack(archivist->getPosition(*troops.begin()));
	}
	else
		scout = NULL;
}