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

	std::multiset<BWAPI::UnitType> scheduled; //TODO Remove this
	utilUnit::UnitSet incompleteUnits;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> factories;

public:
	Producer(Accountant * accountant);
	~Producer();

	void addFactory(BWAPI::Unit * factory);
	void removeFactory(BWAPI::Unit * factory);
	void addProduction(BWAPI::Unit * unit);

	bool train(BWAPI::UnitType unitType);

	utilUnit::UnitSet getFactories(BWAPI::UnitType factoryType);
};