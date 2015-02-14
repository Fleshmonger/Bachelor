#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class WorkerManager
{
private:
	BWAPI::Unitset * workers;
public:
	WorkerManager();
	~WorkerManager();
	void addWorker(BWAPI::Unit worker);
	void removeWorker(BWAPI::Unit worker);
	BWAPI::Unit takeWorker();
	BWAPI::Unitset * getWorkers();
};