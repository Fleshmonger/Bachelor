#pragma once
#include <BWAPI.h>
#include <queue> //TODO Remove?
#include <boost/foreach.hpp>
#include "UtilUnit.h"

const int MINERAL_SATURATION_MAX = 3; // Maximum harvesters on a single mineral field.

// Contains and handles all workers, assigning idle ones to harvesting.
// TODO Split harvesting to separate class?
// TODO Needs a cleanup.
class WorkerManager
{
	private:
		BWAPI::Unit * depot;
		utilUnit::UnitList minerals;
		utilUnit::UnitSet idle, miners;
		std::map<BWAPI::Unit*, BWAPI::Unit*> mining;
		std::map<BWAPI::Unit*, utilUnit::UnitSet> mineralMiners;

	public:
		WorkerManager();
		~WorkerManager();

		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void removeMiner(BWAPI::Unit * miner);
		void addMineral(BWAPI::Unit * mineral);
		void setDepot(BWAPI::Unit * depot);
		void update();

		bool assignWorker(BWAPI::Unit * worker);

		int maxMiners();
		int workers();

		BWAPI::Unit * takeWorker(); // TODO Rename 'take'
		utilUnit::UnitList getMinerals();
		std::map<BWAPI::Unit*, utilUnit::UnitSet> getMineralMiners();
};