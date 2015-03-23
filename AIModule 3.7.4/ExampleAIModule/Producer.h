#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Accountant.h"

// Constructs units and monitors incomplete ones.
class Producer
{
private:
	Accountant * accountant;

	BWAPI::Unit * depot;
	std::multiset<BWAPI::UnitType> scheduledUnits;
	utilUnit::UnitSet incompleteUnits, infantryFacilities, idleInfantryFacilities;

public:
	Producer(Accountant * accountant);
	~Producer();

	void incompleteUnit(BWAPI::Unit * unit);
	void completeUnit(BWAPI::Unit * unit);
	void addInfantryFacility(BWAPI::Unit * facility);
	void removeInfantryFacility(BWAPI::Unit * facility);
	void setDepot(BWAPI::Unit * depot);
	void update();

	bool trainUnit(BWAPI::UnitType unitType);
};