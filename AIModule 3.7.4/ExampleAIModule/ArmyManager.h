#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "CombatJudge.h"


enum Task { IDLE, DEFEND, ATTACK_FIGHT, ATTACK_TRANSIT };


// Commands attacking and defending troops.
// TODO Rename to non-manager.
class ArmyManager
{
private:
	Archivist		* archivist;
	WorkerManager	* workerManager;
	CombatJudge		* combatJudge;

	utilUnit::UnitSet					army;
	std::map<BWAPI::Unit*, Task>		assignments;
	std::map<Task, utilUnit::UnitSet>	enlisted;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void assignUnit(BWAPI::Unit * unit, Task task);
	void update();

	bool contains(BWAPI::Unit * unit);

	utilUnit::UnitSet getArmy();
	utilUnit::UnitSet getEnlisted(Task task);
};