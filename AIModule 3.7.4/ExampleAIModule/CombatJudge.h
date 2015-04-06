#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Archivist.h"


// Manager that contains general combat functions
// TODO Rename
class CombatJudge
{
private:
	Archivist * archivist;

public:
	CombatJudge(Archivist * archivist);
	~CombatJudge();

	double strength(BWAPI::Unit * unit);
	double strength(utilUnit::UnitSet units);
};

