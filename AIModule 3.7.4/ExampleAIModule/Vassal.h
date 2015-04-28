#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilMap.h"
#include "utilUnit.h"
#include "Taskmaster.h"


// Manages a regions workers and harvesting.
class Vassal
{
private:
	BWTA::Region* region;

	BWAPI::Unit		* depot;
	Taskmaster		taskmaster;

public:
	Vassal(BWTA::Region * region);
	~Vassal();

	void setDepot(BWAPI::Unit * depot);
	void addWorker(BWAPI::Unit * worker);
	void removeWorker(BWAPI::Unit * worker);
	void employWorker(BWAPI::Unit * worker, Task task);
	void employWorkers(utilUnit::UnitSet workers, Task task);

	unsigned int workforce();

	utilMap::Zone getHarvestingZone();

	BWAPI::Unit * getDepot();
	BWAPI::Unit * getIdleWorker();
	utilUnit::UnitSet getEmployed(Task task);

	BWTA::Region * getRegion();
};

