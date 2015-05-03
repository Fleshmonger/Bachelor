#include "Geologist.h"


// Constructor.
Geologist::Geologist() :
	regionGeysers(),
	geyserPositions()
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
	if (geyser &&
		utilUnit::isGeyser(geyser->getType()) || utilUnit::isGeyser(geyser->getInitialType()))
	{
		// Aquire position.
		BWAPI::TilePosition pos;
		if (geyser->isVisible())
			pos = geyser->getTilePosition();
		else
			pos = geyser->getInitialTilePosition();

		// Verify position.
		if (pos)
		{
			// Check region.
			BWTA::Region * region = BWTA::getRegion(pos);
			if (!contains(region))
				regionGeysers[region] = utilUnit::UnitSet();

			// Add geyser.
			regionGeysers[region].insert(geyser);
			geyserPositions[geyser] = pos;
		}
	}
}


// Removes a geyser from the pool.
void Geologist::removeGeyser(BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (contains(geyser))
	{
		// Aquire position.
		BWAPI::TilePosition pos = geyserPositions[geyser] = pos;
		BWTA::Region * region = BWTA::getRegion(pos);

		// Remove geyser.
		regionGeysers[region].erase(geyser);
		geyserPositions.erase(geyser);
	}
}


// Returns whether the region is recorded.
bool Geologist::contains(BWTA::Region * region)
{
	return regionGeysers.count(region) > 0;
}


// Returns whether the geyser is recorded.
bool Geologist::contains(BWAPI::Unit * geyser)
{
	return geyserPositions.count(geyser) > 0;
}


// Returns a set of pointers og recorded geysers in the region.
utilUnit::UnitSet Geologist::getGeysers(BWTA::Region * region)
{
	if (contains(region))
		return regionGeysers[region];
	else
		return utilUnit::UnitSet();
}


// Returns the position of the geyser.
BWAPI::TilePosition Geologist::getGeyserPosition(BWAPI::Unit * geyser)
{
	if (contains(geyser))
		return geyserPositions[geyser];
	else
		return BWAPI::TilePositions::None;
}