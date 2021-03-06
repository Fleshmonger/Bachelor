#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "UtilUnit.h"
#include "Archivist.h"
#include "Vassal.h"
#include "Landlord.h"


const unsigned int MIN_WORKFORCE = 9; // Minimum amount of workers available before requesting a scout.


// Scouts the map while the enemy base is hidden.
//TODO Make the scouting activated by the planner?
//TODO Cleanup.
//TODO Rename to explorer
class Reconnoiter
{
private:
	Archivist * archivist;
	Landlord * landlord;

	BWAPI::Unit * scout;
	BWTA::BaseLocation * target;

public:
	Reconnoiter(Archivist * archivist, Landlord * landlord);
	~Reconnoiter();

	void update();
};