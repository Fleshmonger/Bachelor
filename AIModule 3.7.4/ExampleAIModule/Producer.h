#pragma once
#include <BWAPI.h>
#include "Accountant.h"

using namespace BWAPI;

class Producer
{
private:
	// Managers
	Accountant * accountant;
	// Local
	BWAPI::Unit * depot;
	std::set<Unit*> * incompleteUnits, *infantryFacilities, *idleInfantryFacilities;
public:
	Producer(Accountant * accountant);
	~Producer();
	bool trainUnit(BWAPI::UnitType unitType);
	void incompleteUnit(BWAPI::Unit * unit);
	void completeUnit(BWAPI::Unit * unit);
	//int totalInfantryFacilities();
	void addInfantryFacility(BWAPI::Unit * facility);
	void removeInfantryFacility(BWAPI::Unit * facility);
	void setDepot(BWAPI::Unit * depot);
	void update();
};