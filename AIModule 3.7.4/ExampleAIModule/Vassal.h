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
//TODO Should it interpret sub-managers, or return sub-managers?
class Vassal
{
private:
	BWAPI::Unit	* depot;
	Accountant	* accountant;
	Producer	* producer;

	BWTA::Region	* region;
	WorkerManager	workerManager;
	Harvester		harvester;
	Architect		architect;

public:
	Vassal(BWAPI::Unit * depot, Accountant * accountant, Producer * producer);
	~Vassal();

	void unitCompleted(BWAPI::Unit * unit);
	void unitCreated(BWAPI::Unit * unit);
	void unitDestroyed(BWAPI::Unit * unit);
	void update();

	bool build(BWAPI::UnitType buildingType);

	unsigned int mineralFields();
	unsigned int workforce();
	unsigned int scheduled(BWAPI::UnitType unitType);

	BWAPI::Unit * getDepot();
};

