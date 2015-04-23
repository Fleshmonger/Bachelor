#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


enum Task { TASK_BUILD, TASK_DEFEND, TASK_HARVEST, TASK_IDLE, TASK_MINE };


// Contains and handles all workers while commanding harvesting.
class Taskmaster
{
	private:
		utilUnit::UnitSet					workers;
		std::map<BWAPI::Unit*, Task>		assignments;
		std::map<Task, utilUnit::UnitSet>	employed;

	public:
		Taskmaster();
		~Taskmaster();

		//void update();
		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void employWorker(BWAPI::Unit * worker, Task task);

		bool contains(BWAPI::Unit * worker);

		unsigned int workforce();

		utilUnit::UnitSet getEmployed(Task task);
};