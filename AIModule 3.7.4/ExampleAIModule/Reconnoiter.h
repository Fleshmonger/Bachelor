#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"

using namespace BWAPI;

const unsigned int MINIMUM_WORKERS = 5; // Minimum amount of workers available before requesting a scout.

class Reconnoiter
{
private:
	// Managers
	WorkerManager * workerManager;
	// Local
	BWAPI::TilePosition target;
	BWAPI::Unit * scout;
public:
	Reconnoiter(WorkerManager * workerManager);
	~Reconnoiter();
	void update();
};