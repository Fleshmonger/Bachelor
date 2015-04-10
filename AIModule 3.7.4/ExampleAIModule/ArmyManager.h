#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "CombatJudge.h"



enum Duty { DUTY_ATTACK_FIGHT, DUTY_ATTACK_TRANSIT, DUTY_DEFEND, DUTY_IDLE };



// Commands attacking and defending troops.
// TODO Rename to non-manager.
class ArmyManager
{
private:
	Archivist		* archivist;
	WorkerManager	* workerManager;
	CombatJudge		* combatJudge;

	utilUnit::UnitSet					army;
	std::map<BWAPI::Unit*, Duty>		assignments;
	std::map<Duty, utilUnit::UnitSet>	enlisted;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge);
	~ArmyManager();

	void update();
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void assignUnit(BWAPI::Unit * unit, Duty duty);

	bool contains(BWAPI::Unit * unit);

	utilUnit::UnitSet getArmy();
	utilUnit::UnitSet getEnlisted(Duty duty);
};