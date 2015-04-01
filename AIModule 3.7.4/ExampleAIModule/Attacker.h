#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Archivist.h"
#include "CombatJudge.h"


// Commands attacking units.
// TODO Implement squads.
// TODO Rename?
class Attacker
{
private:
	Archivist * archivist;
	CombatJudge * combatJudge;

	double attackingStrength;
	BWAPI::Unit
		* target,
		* depot;
	utilUnit::UnitSet fighters, transit, waiting;

public:
	Attacker(Archivist * archivist, CombatJudge * combatJudge);
	~Attacker();

	void commandAttack(utilUnit::UnitSet attackers);
	void setDepot(BWAPI::Unit * depot);

	bool enemyVisible(BWAPI::Unit * unit); //TODO Rename
};