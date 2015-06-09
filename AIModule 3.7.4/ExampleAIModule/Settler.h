#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Archivist.h"
#include "Landlord.h"
#include "Architect.h"


// Handles expansions.
class Settler
{
private:
	Archivist	* archivist;
	Architect	* architect;
	Landlord	* landlord;

public:
	Settler(Archivist * archivist, Landlord * landlord, Architect * architect);
	~Settler();

	bool buildExpansion();
	bool isOccupied(BWTA::Region * region);

	BWTA::Region * nextExpansion();
};