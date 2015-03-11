#pragma once
#include <BWAPI.h>
#include "Archivist.h"
#include "WorkerManager.h"

using namespace BWAPI; // TODO Unneeded?

const unsigned int MINIMUM_WORKERS = 5; // Minimum amount of workers available before requesting a scout.

//TODO Make the scouting activated by the future strategist manager.
// Scouts the map while the enemy base is hidden.
class Reconnoiter
{
private:
	// Managers
	Archivist * archivist;
	WorkerManager * workerManager;

	// Local
	BWAPI::Position target;
	BWAPI::Unit * scout;

public:
	Reconnoiter(Archivist * archivist, WorkerManager * workerManager);
	~Reconnoiter();
	void update();
};