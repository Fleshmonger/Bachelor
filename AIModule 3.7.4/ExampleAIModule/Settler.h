#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Landlord.h"
#include "Architect.h"


// Handles expansions.
//TODO Maynarding.
class Settler
{
private:
	Architect	* architect;
	Landlord	* landlord;

public:
	Settler(Landlord * landlord, Architect * architect);
	~Settler();

	bool buildExpansion();

	BWTA::Region * nextExpansion();
};