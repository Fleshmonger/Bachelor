#include "Settler.h"


// Constructor.
Settler::Settler(Landlord * landlord) :
	landlord(landlord)
{
}


// Deconstructor.
Settler::~Settler()
{
}


// Called after BWTA analysis. Initializes geyser data.
void Settler::initialize()
{
	// Iterate through regions.
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		// Initialize set.
		utilUnit::UnitSet geysers;

		// Iterate through base locations.
		BOOST_FOREACH(BWTA::BaseLocation * baseLocation, region->getBaseLocations())
		{
			// Add geysers.
			utilUnit::UnitSet baseGeysers = baseLocation->getGeysers();
			geysers.insert(baseGeysers.begin(), baseGeysers.end());
		}

		// Add entry.
		regionGeysers[region] = geysers;
	}
}


// Updates geyser data.
void Settler::update()
{
	// Iterate through entries.
	typedef std::pair<BWTA::Region*, utilUnit::UnitSet> geyserEntry;
	BOOST_FOREACH(geyserEntry entry, regionGeysers)
	{
		//TODO
	}
}


// Returns the next region for expansion.
BWTA::Region * Settler::nextExpansion()
{
	// Verify headquarters.
	Vassal * headquarters = landlord->getHeadquarters();
	if (headquarters &&
		headquarters->getRegion())
	{
		// Initialize queue.
		std::list<BWTA::Region*> queue;
		queue.push_front(headquarters->getRegion());

		// Iterate through queue.
		while (!queue.empty())
		{
			// Verify region.
			BWTA::Region * region = queue.front();
			if (region)
			{
				// Check availability.
				if (!landlord->contains(region) &&
					!region->getBaseLocations().empty())
					return region;
				else
				{
					// Add neighbors.
					BOOST_FOREACH(BWTA::Chokepoint * border, region->getChokepoints())
					{
						// Add neighbor.
						std::pair<BWTA::Region*, BWTA::Region*> neighbors = border->getRegions();
						if (neighbors.first == region)
							queue.push_back(neighbors.second);
						else
							queue.push_back(neighbors.first);
					}
				}
			}

			// Remove entry.
			queue.pop_front();
		}
	}

	// No expansion found.
	return NULL;
}


utilUnit::UnitSet getGeysers(BWTA::Region * region)
{

}