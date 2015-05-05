#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"


// Trains units and monitors their construction.
//TODO Split into recruiter and trainer?
class Recruiter
{
private:
	Accountant * accountant;

	utilUnit::UnitSet constructions;
	std::map<BWAPI::UnitType, unsigned int> trainingSchedule;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> typeFactories;

public:
	Recruiter(Accountant * accountant);
	~Recruiter();

	void addFactory(BWAPI::Unit * factory);
	void removeFactory(BWAPI::Unit * factory);
	void addConstruction(BWAPI::Unit * unit);
	void removeConstruction(BWAPI::Unit * unit);

	bool contains(BWAPI::Unit * unit);
	bool scheduleTraining(BWAPI::UnitType unitType);
	bool scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory);

	utilUnit::UnitSet getFactories(BWAPI::UnitType factoryType);
};