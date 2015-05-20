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


// Returns the next region for expansion.
BWTA::Region * Settler::nextExpansion()
{
	// Verify main base.
	Vassal * main = landlord->getMain();
	if (main &&
		main->getRegion())
	{
		// Initialize queue.
		std::list<BWTA::Region*> queue;
		queue.push_front(main->getRegion());

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