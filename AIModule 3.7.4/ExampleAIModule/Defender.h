#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "utilUnit.h"
#include "Archivist.h"
#include "Landlord.h"
#include "CombatJudge.h"
#include "ArmyManager.h"


// Commands units to defend.
class Defender
{
private:
	Archivist		* archivist;
	Landlord		* landlord;
	CombatJudge		* combatJudge;
	ArmyManager		* armyManager;

public:
	Defender(Archivist * archivist, Landlord * landlord, CombatJudge * combatJudge, ArmyManager * armyManager);
	~Defender();

	void update();
};

