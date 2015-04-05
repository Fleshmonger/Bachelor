#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "CombatJudge.h"
#include "Defender.h"
#include "Attacker.h"


// Commands attacking and defending troops.
// TODO Rename to non-manager.
class ArmyManager
{
private:
	Archivist		* archivist;
	WorkerManager	* workerManager;

	utilUnit::UnitSet
		army, // TEMP Uneeded, remove this.
		attackers,
		defenders,
		idle;

	CombatJudge	combatJudge;
	Defender	defender;
	Attacker	attacker;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void setDepot(BWAPI::Unit * depot);
	void update();

	utilUnit::UnitSet getArmy(); // TEMP
};