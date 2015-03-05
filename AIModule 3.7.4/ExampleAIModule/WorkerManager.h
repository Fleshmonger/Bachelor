#pragma once
#include <BWAPI.h>
#include <queue>

using namespace BWAPI;

const int MAX_FIELD_HARVESTERS = 3; // Maximum harvesters on a single mineral field.

class WorkerManager
{
	// Mineral Fields
	typedef std::pair<int, BWAPI::Unit*> Field;
	struct Field_Comp{
		bool operator()(Field const & lhs, Field const & rhs){
			return lhs.first < rhs.first;
		}
	};
	private:
		std::set<int> test;
		// Local
		BWAPI::Unit * depot;
		std::vector<Field> fields;
		std::set<BWAPI::Unit*> idle;
		std::map<BWAPI::Unit*, BWAPI::Unit*> harvesters;
		//std::priority_queue<Field, std::vector<Field>, Field_Comp> * fields;
	public:
		WorkerManager();
		~WorkerManager();
		void setDepot(BWAPI::Unit * depot);
		void addWorker(BWAPI::Unit * worker);
		void removeWorker(BWAPI::Unit * worker);
		bool assignWorker(BWAPI::Unit * worker);
		int workers();
		int harvesterMax();
		//std::set<Unit*> * getWorkers();
		void addMineral(BWAPI::Unit * mineral);
		BWAPI::Unit * takeWorker();
		void update();
};