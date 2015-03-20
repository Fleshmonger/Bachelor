#pragma once
#include <BWAPI.h>
#include <queue>
#include <boost/foreach.hpp>
#include "UtilUnit.h"

const int MAX_FIELD_HARVESTERS = 3; // Maximum harvesters on a single mineral field.

class WorkerManager
{
	//TODO Make the whole queue a struct?
	typedef std::pair<int, BWAPI::Unit*> Field;

	struct Field_Comp
	{
		bool operator()(const Field & lhs, const Field & rhs) const
		{
			return lhs.first > rhs.first;
		}
	};

	private:
		BWAPI::Unit * depot;
		std::vector<Field> fields;
		std::set<BWAPI::Unit*> idle;
		std::map<BWAPI::Unit*, BWAPI::Unit*> harvesters;

		void insertField(int amount, BWAPI::Unit * mineral);
		void popField();
		//int removeField(BWAPI::Unit * mineral);

	public:
		WorkerManager();
		~WorkerManager();
		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		void addMineral(BWAPI::Unit * mineral);
		void setDepot(BWAPI::Unit * depot);
		void update();
		bool assignWorker(BWAPI::Unit * worker);
		int harvesterMax();
		int workers();
		BWAPI::Unit * takeWorker();
};