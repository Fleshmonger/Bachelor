#include "Geologist.h"


// Constructor.
Geologist::Geologist()
{
}


// Deconstructor.
Geologist::~Geologist()
{
}


// Called after BWTA analysis. Adds initial geysers.
void Geologist::initialize()
{
	// Iterate through base locations.
	BOOST_FOREACH(BWTA::BaseLocation * baseLocation, BWTA::getBaseLocations())
		BOOST_FOREACH(BWAPI::Unit * geyser, baseLocation->getGeysers())
			addGeyser(geyser);
}


// Adds the geyser to the pool.
void Geologist::addGeyser(BWAPI::Unit * geyser)
{
	// Verify Geyser.
	if (geyser)
	{
		// Aquire tile-position.
		BWAPI::TilePosition tile;
		if (geyser->isVisible())
			tile = geyser->getTilePosition();
		else
			tile = geyser->getInitialTilePosition();

		// Verify position.
		if (tile)
		{
			// Check region.
			BWTA::Region * region = BWTA::getRegion(tile);
			if (!contains(region))
				regionGeysers[region] = utilUnit::UnitSet();

			// Add geyser.
			regionGeysers[region].insert(geyser);
		}
	}
}


// Removes a geyser from the pool.
void Geologist::removeGeyser(BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (geyser)
	{
		// Verify region.
		BWTA::Region * region = BWTA::getRegion(geyser->getInitialPosition());
		if (contains(region))
			regionGeysers[region].erase(geyser);
	}
}


// Returns whether the region is recorded.
bool Geologist::contains(BWTA::Region * region)
{
	return regionGeysers.count(region) > 0;
}


// Returns a set of pointers og recorded geysers in the region.
utilUnit::UnitSet Geologist::getGeysers(BWTA::Region * region)
{
	if (contains(region))
		return regionGeysers[region];
	else
		return utilUnit::UnitSet();
}