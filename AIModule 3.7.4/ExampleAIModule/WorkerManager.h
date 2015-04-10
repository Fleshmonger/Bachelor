#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


enum Task { TASK_BUILD, TASK_DEFEND, TASK_IDLE };


// Contains and handles all workers while commanding harvesting.
class WorkerManager
{
	private:
		//unsigned int minerQouta;
		BWAPI::Unit * depot;
		//utilUnit::UnitSet idle, miners;

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
		//void setMinerQouta(unsigned int qouta);
		void update();

		bool contains(BWAPI::Unit * worker);

		unsigned int workforce();

		BWAPI::Unit * getIdle();
		//BWAPI::Unit * takeWorker(); // TODO Rename 'take'
		//utilUnit::UnitSet getMiners();
		utilUnit::UnitSet getEmployed(Task task);
};