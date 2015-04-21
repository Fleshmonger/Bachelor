#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"


// Trains units and monitors incomplete ones.
class Producer
{
private:
	Accountant * accountant;

	std::map<BWAPI::UnitType, int>
		trainingSchedule,
		productionSchedule;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> factories;

public:
	Producer(Accountant * accountant);
	~Producer();

	void addFactory(BWAPI::Unit * factory);
	void addProduction(BWAPI::Unit * unit);
	void removeFactory(BWAPI::Unit * factory);
	void removeProduction(BWAPI::Unit * unit);

	bool scheduleTraining(BWAPI::UnitType unitType);
	bool scheduleTraining(BWAPI::UnitType unitType, BWAPI::Unit * factory);

	unsigned int scheduled(BWAPI::UnitType unitType);

	utilUnit::UnitSet getFactories(BWAPI::UnitType factoryType);
};