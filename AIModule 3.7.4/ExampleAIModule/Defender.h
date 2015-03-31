#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "utilUnit.h"
#include "Archivist.h"


// Commands given units to defend.
//TODO Make independant of army manager?
class Defender
{
private:
	Archivist * archivist;

public:
	Defender(Archivist * archivist);
	~Defender();

	void commandDefense(utilUnit::UnitSet guards, utilUnit::UnitSet invaders);
};

