#include "Reconnoiter.h"

// Constructor
Reconnoiter::Reconnoiter(WorkerManager * workerManager) :
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
	// Retrieving Target
	std::set<BWAPI::TilePosition> points = Broodwar->getStartLocations();
	std::set<BWAPI::TilePosition>::iterator it = points.begin();
	while ((!target || Broodwar->isExplored(target)) && it != points.end())
	{
		BWAPI::TilePosition tile = *it;
		if (!Broodwar->isExplored(tile))
			target = tile;
		++it;
	}
	// Scouting
	if (target)
	{
		// Retrieving Scout
		if ((!scout || !scout->exists()) && workerManager->workers() >= MINIMUM_WORKERS)
			scout = workerManager->takeWorker();
		// Commanding Scout
		if (scout)
			scout->move(BWAPI::Position(target));
	}
	else
	{
		// Return Scout
		if (scout && scout->exists())
			workerManager->addWorker(scout);
		scout = NULL;
	}
}