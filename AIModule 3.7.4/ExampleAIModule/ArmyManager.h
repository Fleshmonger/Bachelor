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
// TODO Rename to non-manager.
class ArmyManager
{
private:
	Archivist * archivist;
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect;

	std::set<BWAPI::Unit*> troops, attackers, idle;
	std::map<BWAPI::Unit*, int> invaders;
	std::map<BWAPI::Unit*, BWAPI::Unit*> defenders;

public:
	int toughness(std::set<BWAPI::Unit*> units); // TODO Rename; move; make private
	double TTK(std::set<BWAPI::Unit*> attackers, std::set<BWAPI::Unit*> defenders); // TODO Rename; move; make private
	double damage(std::set<BWAPI::Unit*> units); // TODO Rename; move; make private

	ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();

	bool canAttack(); // TODO rename

	std::set<BWAPI::Unit*> getTroops();
};