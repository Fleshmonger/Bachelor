#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilUnit.h"
#include "WorkerManager.h"
#include "Harvester.h"


// Manages a regions workers and harvesting.
class Vassal
{
private:
	BWAPI::Unit	* depot;

	WorkerManager	workerManager;
	Harvester		harvester;

public:
	Vassal(BWAPI::Unit * depot);
	~Vassal();

	void addWorker(BWAPI::Unit * worker);
	void employWorker(BWAPI::Unit * worker, Task task);
	void removeWorker(BWAPI::Unit * worker);
	void harvest();
	void update();

	unsigned int mineralFields();
	unsigned int workforce();

	BWAPI::Unit * getDepot();
	BWAPI::Unit * getIdleWorker();
};

