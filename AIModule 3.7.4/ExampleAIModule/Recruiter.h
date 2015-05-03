#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"


// Trains units and monitors their construction.
class Recruiter
{
private:
	Accountant * accountant;

	utilUnit::UnitSet busyFactories;
	std::map<BWAPI::UnitType, int>
		trainingSchedule,
		constructionSchedule;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> typeFactories;

public:
	Recruiter(Accountant * accountant);
	~Recruiter();

	void update();
	void addFactory(BWAPI::Unit * factory);
	void removeFactory(BWAPI::Unit * factory);
	void addConstruction(BWAPI::Unit * unit);
	void removeConstruction(BWAPI::Unit * unit);

	bool scheduleTraining(BWAPI::UnitType unitType);
	bool scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory);

	unsigned int scheduled(BWAPI::UnitType unitType);

	utilUnit::UnitSet getFactories(BWAPI::UnitType factoryType);
};