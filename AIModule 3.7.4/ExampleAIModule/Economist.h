#pragma once
#include <BWAPI.h>
#include "Accountant.h"
#include "Producer.h"
#include "Vassal.h"


const unsigned int
	MIN_SUPPLY = 10,
	MINERAL_SATURATION = 2;
const BWAPI::UnitType
		UNIT_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon,
		UNIT_WORKER = BWAPI::UnitTypes::Protoss_Probe;


// Manages all vassals and controls economy related actions.
//TODO Split into vassal master and economy manager.
class Economist
{
private:
	Accountant * accountant;
	Producer * producer;

	Vassal * homeBase;
	std::set<Vassal*> vassals;
	std::map<BWAPI::Unit*, BWTA::Region*> unitRegion;
	std::map<BWTA::Region*, Vassal*> regionVassal; //TODO Vassals are dynamic memory.

public:
	Economist(Accountant * accountant, Producer * producer);
	~Economist();

	//void analyzed(); //TODO Move this to constructor.
	void addHomeBase(BWAPI::Unit * depot);
	void addExpansion(BWAPI::Unit * depot);
	void unitCompleted(BWAPI::Unit * unit);
	void unitCreated(BWAPI::Unit * unit);
	void unitDestroyed(BWAPI::Unit * unit);
	void update();
};