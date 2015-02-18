#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"

using namespace BWAPI;

class Reconnoiter
{
private:
	const unsigned int MINIMUM_WORKERS = 5; // Minimum amount of workers available before requesting a scout.
	WorkerManager * workerManager;
	BWAPI::TilePosition target;
	BWAPI::Unit scout;
public:
	Reconnoiter(WorkerManager * workerManager);
	~Reconnoiter();
	void update();
};

