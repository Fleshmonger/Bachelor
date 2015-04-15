#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilUnit.h"
#include "Accountant.h"
#include "WorkerManager.h"
#include "Producer.h"
#include "Harvester.h"
#include "Architect.h"


// Manages a region, its base and its managers.
class Vassal
{
private:
	Accountant * accountant;

	BWAPI::Unit		* depot;
	BWTA::Region	* region;

	WorkerManager	workerManager;
	Producer		producer;
	Harvester		harvester;
	Architect		architect;

public:
	Vassal(BWAPI::Unit * depot, Accountant * accountant);
	~Vassal();

	void unitCompleted(BWAPI::Unit * unit);
	void unitCreated(BWAPI::Unit * unit);
	void unitDestroyed(BWAPI::Unit * unit);
	void update();

	bool needMiners();
	bool build(BWAPI::UnitType buildingType);
	bool train(BWAPI::UnitType unitType);

	unsigned int scheduled(BWAPI::UnitType unitType);
};

