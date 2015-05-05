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
	// Iterate through regions.
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		// Add region.
		regionGeysers[region] = utilUnit::UnitSet();

		// Iterate through base locations.
		BOOST_FOREACH(BWTA::BaseLocation * baseLocation, region->getBaseLocations())
		{
			// Iterate through geysers.
			BOOST_FOREACH(BWAPI::Unit * geyser, baseLocation->getGeysers())
			{
				// Add geyser.
				regionGeysers[region].insert(geyser);
				geyserPositions[geyser] = geyser->getInitialTilePosition();
			}
		}

	}
}


// Adds the geyser to the pool.
void Geologist::addGeyser(BWAPI::Unit * geyser)
{
	// Verify Geyser.
	if (!contains(geyser) &&
		geyser &&
		geyser->isVisible() &&
		utilUnit::isGeyser(geyser->getType()))
	{
		// Add geyser.
		BWAPI::Broodwar->sendText("A geyser was added");
		BWTA::Region * region = BWTA::getRegion(geyser->getTilePosition());
		regionGeysers[region].insert(geyser);
		geyserPositions[geyser] = geyser->getTilePosition();
	}
}


// Removes a geyser from the pool.
void Geologist::removeGeyser(BWAPI::Unit * geyser)
{
	// Verify geyser.
	if (contains(geyser))
	{
		// Remove geyser.
		BWTA::Region * region = BWTA::getRegion(geyserPositions[geyser]);
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