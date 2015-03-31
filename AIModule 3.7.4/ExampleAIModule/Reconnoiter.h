#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"


const unsigned int MIN_WORKFORCE = 5; // Minimum amount of workers available before requesting a scout.


// Scouts the map while the enemy base is hidden.
//TODO Make the scouting activated by the future strategist manager.
class Reconnoiter
{
private:
	Archivist * archivist;
	WorkerManager * workerManager;

	BWAPI::Position target;
	BWAPI::Unit * scout;

public:
	Reconnoiter(Archivist * archivist, WorkerManager * workerManager);
	~Reconnoiter();

	void update();
};