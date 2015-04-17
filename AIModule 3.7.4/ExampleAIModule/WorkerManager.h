#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


enum Task { TASK_BUILD, TASK_DEFEND, TASK_IDLE };


// Contains and handles all workers while commanding harvesting.
//TODO Rename to taskmaster.
class WorkerManager
{
	private:
		BWAPI::Unit * depot;
		utilUnit::UnitSet					workers;
		std::map<BWAPI::Unit*, Task>		assignments;
		std::map<Task, utilUnit::UnitSet>	employed;

	public:
		WorkerManager();
		~WorkerManager();

		void addWorker(BWAPI::Unit * worker);
		void employWorker(BWAPI::Unit * worker, Task task);
		void removeWorker(BWAPI::Unit * worker);
		void setDepot(BWAPI::Unit * depot);
		void update();

		bool contains(BWAPI::Unit * worker);
		unsigned int workforce();

		utilUnit::UnitSet getEmployed(Task task);
};