#pragma once
#include <BWAPI.h>
#include "Accountant.h"

using namespace BWAPI;

class Producer
{
private:
	// Power Structure
	Accountant * accountant; // Unused
	// Local
	BWAPI::Unit depot;
	BWAPI::Unitset * infantryFacilities, * idleInfantryFacilities;
public:
	Producer(Accountant * accountant);
	~Producer();
	bool orderInfantry(BWAPI::UnitType unitType); //Change to some general mandate function? //Order is ambigious.
	bool orderWorker();
	int totalInfantryFacilities();
	void addInfantryFacility(BWAPI::Unit facility);
	void removeInfantryFacility(BWAPI::Unit facility);
	void setDepot(BWAPI::Unit depot);
	void update();
};