#pragma once
#include <BWAPI.h>
#include <queue>
#include <boost/foreach.hpp>

using namespace BWAPI;

const int MAX_FIELD_HARVESTERS = 10; // Maximum harvesters on a single mineral field.

class WorkerManager
{
	// Mineral Fields
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
		int testHarvesters();
		void setDepot(BWAPI::Unit * depot);
		int harvesterMax();
		int workers();
		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		//std::set<Unit*> * getWorkers();
		bool assignWorker(BWAPI::Unit * worker);
		BWAPI::Unit * takeWorker();
		void addMineral(BWAPI::Unit * mineral);
		void update();
};