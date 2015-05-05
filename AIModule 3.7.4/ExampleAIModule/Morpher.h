#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Accountant.h"
#include "Gatherer.h"


// Handles morphing units and notifies sub-managers on completion.
class Morpher
{
private:
	Accountant * accountant;
	Gatherer * gatherer;

	utilUnit::UnitSet morphs;

public:
	Morpher(Accountant * accountant, Gatherer * gatherer);
	~Morpher();

	void update();
	void addMorph(BWAPI::Unit * morph);
	void removeMorph(BWAPI::Unit * morph);

	bool contains(BWAPI::Unit * unit);
};

