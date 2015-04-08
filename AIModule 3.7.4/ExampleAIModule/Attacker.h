#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "CombatJudge.h"
#include "ArmyManager.h"


const int DETECTION_DISTANCE = 400; // 384 is the longest range in the game.


// Commands units to attack.
// TODO Rename?
class Attacker
{
private:
	Archivist	* archivist;
	CombatJudge	* combatJudge;
	ArmyManager	* armyManager;

	BWAPI::Unit
		* depot,
		* target;

public:
	Attacker(Archivist * archivist, CombatJudge * combatJudge, ArmyManager * armyManager);
	~Attacker();

	void setDepot(BWAPI::Unit * depot);
	void update();

	bool enemyDetected(BWAPI::Unit * unit);
};