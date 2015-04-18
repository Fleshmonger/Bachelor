#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "UtilUnit.h"
#include "Archivist.h"
#include "Landlord.h"


const unsigned int MIN_WORKFORCE = 5; // Minimum amount of workers available before requesting a scout.


// Scouts the map while the enemy base is hidden.
//TODO Make the scouting activated by the strategist manager.
//TODO Cleanup.
class Reconnoiter
{
private:
	Archivist * archivist;
	Landlord * landlord;

	BWAPI::Position target;
	BWAPI::Unit * scout;

public:
	Reconnoiter(Archivist * archivist, Landlord * landlord);
	~Reconnoiter();

	void update();
};