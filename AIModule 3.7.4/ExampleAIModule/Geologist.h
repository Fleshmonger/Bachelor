#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


// Records vespene geyser positions.
class Geologist
{
private:
	std::map<BWTA::Region*, utilUnit::UnitSet> regionGeysers;
	std::map<BWAPI::Unit*, BWAPI::TilePosition> geyserPositions;

public:
	Geologist();
	~Geologist();

	void initialize();
	void addGeyser(BWAPI::Unit * geyser);
	void removeGeyser(BWAPI::Unit * geyser);

	bool contains(BWTA::Region * region);
	bool contains(BWAPI::Unit * geyser);

	utilUnit::UnitSet getGeysers(BWTA::Region * region);

	BWAPI::TilePosition getGeyserPosition(BWAPI::Unit * geyser);
};

