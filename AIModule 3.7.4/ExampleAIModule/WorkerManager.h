#pragma once
#include <BWAPI.h>
#include <queue> //TODO Remove?
#include <boost/foreach.hpp>
#include "UtilUnit.h"

const int MINERAL_SATURATION_MAX = 10; // Maximum harvesters on a single mineral field.

//typedef std::pair<int, BWAPI::Unit*> Field;

// Contains and handles all workers, assigning idle ones to harvesting.
// TODO Split harvesting to separate class?
// TODO Harvesting assignment is broken!
// TODO Needs a cleanup.
class WorkerManager
{
	/*
	//TODO Make the whole queue a struct?
	struct Field_Comp
	{
		bool operator()(const Field & lhs, const Field & rhs) const
		{
			return lhs.first > rhs.first;
		}
	};
	*/

	private:
		BWAPI::Unit * depot;
		//std::vector<Field> fields;
		utilUnit::UnitList minerals;
		std::map<BWAPI::Unit*, int> mineralSaturation;
		utilUnit::UnitSet idle;
		std::map<BWAPI::Unit*, BWAPI::Unit*> harvesters;

		//void insertField(int amount, BWAPI::Unit * mineral);
		//void popField();

	public:
		WorkerManager();
		~WorkerManager();

		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void addMineral(BWAPI::Unit * mineral);
		void setDepot(BWAPI::Unit * depot);
		void update();

		bool assignWorker(BWAPI::Unit * worker);

		int saturationMax();
		int getWorkersSize();

		BWAPI::Unit * takeWorker(); // TODO Rename 'take'

		//std::vector<Field> getFields();
		utilUnit::UnitList getMinerals(); //TEMP
		std::map<BWAPI::Unit*, int> getMineralSaturation(); //TEMP

};