#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"
#include "Producer.h"
#include "Architect.h"

using namespace BWAPI;
//using namespace Filter;

class Economist
{
private:
	// Constants
	const BWAPI::UnitType PROTOSS_WORKER = BWAPI::UnitTypes::Protoss_Probe;
	// Managers
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect; // This is currently unused! Planned for assimilator construction.
	// Local
	std::set<BWAPI::Unit*> minerals;
	BWAPI::Unit * depot;
public:
	Economist(WorkerManager * workerManager, Producer * producer, Architect * architect);
	~Economist();
	void setDepot(BWAPI::Unit * depot);
	void update();
};