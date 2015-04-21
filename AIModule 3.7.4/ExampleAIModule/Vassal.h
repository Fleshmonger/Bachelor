#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilMap.h"
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
	Vassal();
	~Vassal();

	void addWorker(BWAPI::Unit * worker);
	void employWorker(BWAPI::Unit * worker, Task task);
	void removeWorker(BWAPI::Unit * worker);
	void setDepot(BWAPI::Unit * depot);
	void harvest();
	void update();

	unsigned int mineralFields();
	unsigned int workforce();

	utilMap::Zone getHarvestingZone();

	BWAPI::Unit * getDepot();
	BWAPI::Unit * getIdleWorker();
	utilUnit::UnitSet getEmployed(Task task);
};

