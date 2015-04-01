#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "Attacker.h"
#include "CombatJudge.h"
#include "Defender.h"


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

	Attacker	attacker;
	CombatJudge	combatJudge;
	Defender	defender;

public:
	ArmyManager(Archivist * archivist, WorkerManager * workerManager);
	~ArmyManager();

	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();

	bool canAttack(); // TODO rename

	utilUnit::UnitSet getArmy(); // TEMP
};