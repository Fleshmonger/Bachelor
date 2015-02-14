#pragma once
#include <BWAPI.h>
#include "Producer.h"
#include "WorkerManager.h"
#include "Architect.h"

using namespace BWAPI;
using namespace Filter;

class Economist
{
private:
	Producer * producer;
	WorkerManager * workerManager;
	Architect * architect; // This is currently unused!
	BWAPI::Unit depot;
public:
	Economist(Producer * producer, WorkerManager * workerManager, Architect * architect);
	~Economist();
	void setDepot(BWAPI::Unit depot);
	void update();
};