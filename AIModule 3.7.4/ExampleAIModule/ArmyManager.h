#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
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
// TODO Rename to non-manager.
class ArmyManager
{
private:
	Archivist * archivist;
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect;

	double armyStrength;

	std::set<BWAPI::Unit*> army, attackers, idle;
	std::map<BWAPI::Unit*, int> invaderDefense;
	std::map<BWAPI::Unit*, BWAPI::Unit*> defenderTargets;

public:
	double strength(BWAPI::Unit* unit);
	double strength(std::set<BWAPI::Unit*> units);
	//int toughness(std::set<BWAPI::Unit*> units); // TODO Rename; move; make private
	//double TTK(std::set<BWAPI::Unit*> attackers, std::set<BWAPI::Unit*> defenders); // TODO Rename; move; make private
	//double damage(std::set<BWAPI::Unit*> units); // TODO Rename; move; make private

	ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();

	bool canAttack(); // TODO rename

	std::set<BWAPI::Unit*> getArmy();
};