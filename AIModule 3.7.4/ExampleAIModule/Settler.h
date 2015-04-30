#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Landlord.h"


// Handles expanding logic.
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