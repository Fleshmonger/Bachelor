#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "Landlord.h"
#include "CombatJudge.h"
#include "ArmyManager.h"


const int DETECTION_DISTANCE = 400; // 384 is the longest range in the game.


// Commands units to attack.
// TODO Rename?
class Attacker
{
private:
	Archivist	* archivist;
	Landlord	* landlord;
	CombatJudge	* combatJudge;
	ArmyManager	* armyManager;

	BWAPI::Unit * target;

public:
	Attacker(Archivist * archivist, Landlord * landlord, CombatJudge * combatJudge, ArmyManager * armyManager);
	~Attacker();

	void update();

	bool enemyDetected(BWAPI::Unit * unit);
};