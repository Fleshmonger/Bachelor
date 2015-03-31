#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


// Contains and handles all workers while commanding harvesting.
class WorkerManager
{
	private:
		unsigned int minerQouta;
		BWAPI::Unit * depot;
		utilUnit::UnitSet idle, miners;

	public:
		WorkerManager();
		~WorkerManager();

		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void setDepot(BWAPI::Unit * depot);
		void setMinerQouta(unsigned int qouta);
		void update();

		unsigned int workforce();

		BWAPI::Unit * takeWorker(); // TODO Rename 'take'
		utilUnit::UnitSet getMiners();
};