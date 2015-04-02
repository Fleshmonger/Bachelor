#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"
#include "Producer.h"
#include "Architect.h"
#include "Harvester.h"


const int MAX_WORKERS = 16;
const BWAPI::UnitType PROTOSS_WORKER = BWAPI::UnitTypes::Protoss_Probe;


class Economist
{
private:
	WorkerManager	* workerManager;
	Producer		* producer;
	Architect		* architect; // TODO This is currently unused! Planned for assimilator construction.
	Harvester		* harvester;

	//BWAPI::Unit * depot;

public:
	Economist(WorkerManager * workerManager, Producer * producer, Architect * architect, Harvester * harvester);
	~Economist();

	//void setDepot(BWAPI::Unit * depot);
	void update();
};