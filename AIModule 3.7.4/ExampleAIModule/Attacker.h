#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "CombatJudge.h"
#include "ArmyManager.h"


// Commands units to attack.
// TODO Rename?
class Attacker
{
private:
	Archivist	* archivist;
	CombatJudge	* combatJudge;
	ArmyManager	* armyManager;

	double attackingStrength;
	BWAPI::Unit
		* target,
		* depot;

public:
	Attacker(Archivist * archivist, CombatJudge * combatJudge, ArmyManager * armyManager);
	~Attacker();

	void setDepot(BWAPI::Unit * depot);
	void update();

	bool enemyDetected(BWAPI::Unit * unit);
};