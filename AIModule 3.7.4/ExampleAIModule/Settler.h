#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Landlord.h"


// Handles expansions and resources.
//TODO Maynarding.
class Settler
{
private:
	Landlord * landlord;

	std::map<BWTA::Region*, utilUnit::UnitSet> regionGeysers;

public:
	Settler(Landlord * landlord);
	~Settler();

	void initialize();
	void update();
	void addGeyser(BWAPI::Unit * unit, BWTA::Region * region);
	void removeGeyser(BWAPI::Unit * unit, BWTA::Region * region);
	
	BWTA::Region * nextExpansion();
	utilUnit::UnitSet getGeysers();
};