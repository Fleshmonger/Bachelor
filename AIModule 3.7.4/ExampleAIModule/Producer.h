#pragma once
#include <BWAPI.h>
#include "Accountant.h"

class Producer
{
private:
	// Managers
	Accountant * accountant;

	// Local
	BWAPI::Unit * depot;
	std::multiset<BWAPI::UnitType> scheduledUnits;
	std::set<Unit*> incompleteUnits, infantryFacilities, idleInfantryFacilities;

public:
	Producer(Accountant * accountant);
	~Producer();
	bool trainUnit(BWAPI::UnitType unitType);
	void incompleteUnit(BWAPI::Unit * unit);
	void completeUnit(BWAPI::Unit * unit);
	void addInfantryFacility(BWAPI::Unit * facility);
	void removeInfantryFacility(BWAPI::Unit * facility);
	void setDepot(BWAPI::Unit * depot);
	void update();
};