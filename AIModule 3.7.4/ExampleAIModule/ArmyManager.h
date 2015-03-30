#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "Producer.h"
#include "CombatJudge.h"
#include "Architect.h"


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

	CombatJudge combatJudge;

	utilUnit::UnitSet army, attackers, defenders, idle;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();

	bool canAttack(); // TODO rename

	utilUnit::UnitSet getArmy();
};