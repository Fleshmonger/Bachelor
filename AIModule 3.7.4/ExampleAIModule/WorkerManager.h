#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Harvester.h"


// Contains and handles all workers while commanding harvesting.
class WorkerManager
{
	private:
		Harvester harvester;

		BWAPI::Unit * depot;
		utilUnit::UnitSet idle, miners;

	public:
		WorkerManager();
		~WorkerManager();

		void analyzed();
		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void setDepot(BWAPI::Unit * depot);
		void update();

		bool needWorkers();

		unsigned int workers();

		BWAPI::Unit * takeWorker(); // TODO Rename 'take'
};