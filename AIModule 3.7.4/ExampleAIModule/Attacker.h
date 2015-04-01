#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Archivist.h"


// Commands attacking units.
// TODO Implement squads.
// TODO Rename?
class Attacker
{
private:
	Archivist * archivist;

public:
	Attacker(Archivist * archivist);
	~Attacker();

	void commandAttack(utilUnit::UnitSet attackers);
};