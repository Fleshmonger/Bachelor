#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class WorkerManager
{
private:
	std::set<Unit*> * workers;
public:
	WorkerManager();
	~WorkerManager();
	void addWorker(BWAPI::Unit * worker);
	void removeWorker(BWAPI::Unit * worker);
	BWAPI::Unit * takeWorker();
	std::set<Unit*> * getWorkers();
};