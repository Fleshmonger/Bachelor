#include "Reconnoiter.h"

// Constructor
Reconnoiter::Reconnoiter(Archivist * archivist, WorkerManager * workerManager) :
	archivist(archivist),
	workerManager(workerManager),
	scout(NULL),
	target(BWAPI::TilePositions::None)
{
}

// Deconstructor
Reconnoiter::~Reconnoiter()
{
}

// Simulate the reconnoiter AI.
// Retrieves, returns and commands the scout.
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
			// Retrieving Scout
			if ((!scout || !scout->exists()) && workerManager->workers() >= MINIMUM_WORKERS)
				scout = workerManager->takeWorker();
			// Commanding Scout
			if (scout && scout->getOrderTargetPosition() != target)
				utilUnit::command(scout, BWAPI::UnitCommandTypes::Move, target);
		}
	}
	else if (scout && scout->exists())
	{
		// Harassing
		std::set<BWAPI::Unit*> troops = archivist->getTroops();
		if (!troops.empty() && !scout->isAttacking())
			utilUnit::command(scout, BWAPI::UnitCommandTypes::Attack_Move, archivist->getPosition(*troops.begin()));
	}
	else
		scout = NULL;
}