#pragma once
#include <BWAPI.h>
#include "Accountant.h"
#include "Vassal.h"


static const int MIN_SUPPLY = 10;
static const BWAPI::UnitType BUILD_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;
const BWAPI::UnitType PROTOSS_WORKER = BWAPI::UnitTypes::Protoss_Probe;


class Economist
{
private:
	Accountant accountant;
	Vassal * homeBase;
	std::set<Vassal*> vassals;
	std::map<BWAPI::Unit*, BWTA::Region*> unitRegion;
	std::map<BWTA::Region*, Vassal*> regionVassal; //TODO Vassals are dynamic memory.

public:
	Economist();
	~Economist();

	void addHomeBase(BWAPI::Unit * depot);
	void addExpansion(BWAPI::Unit * depot);
	void unitCompleted(BWAPI::Unit * unit);
	void unitCreated(BWAPI::Unit * unit);
	void unitDestroyed(BWAPI::Unit * unit);
	//void analyzed(); //TODO Move this to constructor.
	void update();
};