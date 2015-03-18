#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "archivist.h"
#include "workerManager.h"
#include "producer.h"
#include "architect.h"

const int
	DEFENDERS_PER_SCOUT = 1,
	DEFENDERS_PER_ATTACKER = 3;

//TODO Move somewhere else?
const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;

// Commands attacking and defending troops.
class ArmyManager
{
private:
	Archivist * archivist;
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect;

	//BWAPI::Unit * defender;
	std::set<BWAPI::Unit*> troops;
	std::map<BWAPI::Unit*, int> invaders;
	std::map<BWAPI::Unit*, BWAPI::Unit*> defenders;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void assignDefender(BWAPI::Unit * defender, BWAPI::Unit * invader);
	void update();
};