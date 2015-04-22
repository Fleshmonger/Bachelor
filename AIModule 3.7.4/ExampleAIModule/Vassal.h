#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilMap.h"
#include "utilUnit.h"
#include "Taskmaster.h"
#include "Harvester.h"


// Manages a regions workers and harvesting.
class Vassal
{
private:
	BWTA::Region* region;

	BWAPI::Unit		* depot;
	Taskmaster		taskmaster;
	Harvester		harvester;

public:
	Vassal(BWTA::Region * region);
	~Vassal();

	void harvest();
	//void update();
	void setDepot(BWAPI::Unit * depot);
	void addRefinery(BWAPI::Unit * refinery);
	void removeRefinery(BWAPI::Unit * refinery);
	void addWorker(BWAPI::Unit * worker);
	void removeWorker(BWAPI::Unit * worker);
	void employWorker(BWAPI::Unit * worker, Task task);

	unsigned int minerals();
	unsigned int refineries();
	unsigned int workforce();

	utilMap::Zone getHarvestingZone();

	BWAPI::Unit * getDepot();
	BWAPI::Unit * getIdleWorker();
	utilUnit::UnitSet getEmployed(Task task);

	BWTA::Region * getRegion();
};

