#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Landlord.h"


// Handles expansions.
//TODO Maynarding.
class Settler
{
private:
	Landlord * landlord;

public:
	Settler(Landlord * landlord);
	~Settler();
	
	BWTA::Region * nextExpansion();
};