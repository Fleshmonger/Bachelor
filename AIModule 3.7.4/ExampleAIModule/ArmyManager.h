#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "archivist.h"
#include "workerManager.h"
#include "producer.h"
#include "architect.h"

class ArmyManager
{
private:
	// Managers
	Archivist * archivist;
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect;
	// Local
	BWAPI::Unit * defender;
	std::set<BWAPI::Unit*> troops;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();
};