#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Producer
{
private:
	BWAPI::Unit depot;
	BWAPI::Unitset * infantryFacilities, * idleInfantryFacilities;
public:
	Producer();
	~Producer();
	bool orderInfantry(BWAPI::UnitType unitType); //Change to some general mandate function? //Order is ambigious.
	bool orderWorker();
	bool canAfford(BWAPI::UnitType unitType);
	int totalInfantryFacilities();
	void addInfantryFacility(BWAPI::Unit facility);
	void removeInfantryFacility(BWAPI::Unit facility);
	void setDepot(BWAPI::Unit depot);
	void update();
};