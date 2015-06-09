#include "Settler.h"


// Constructor.
Settler::Settler(Archivist * archivist, Landlord * landlord, Architect * architect) :
	archivist(archivist),
	landlord(landlord),
	architect(architect)
{
}


// Deconstructor.
Settler::~Settler()
{
}


// Attempts to schedule an expansion and returns true if successful.
bool Settler::buildExpansion()
{
	// Attempt schedule.
	BWTA::Region * region = nextExpansion();
	return
		region &&
		architect->scheduleBuilding(
			BWAPI::UnitTypes::Protoss_Nexus,
			(*region->getBaseLocations().begin())->getTilePosition(),
			landlord->getMain()->getIdleWorker());
}


// Returns whether the region is occupied by enemy units.
bool Settler::isOccupied(BWTA::Region * region)
{
	// Search through enemies.
	BOOST_FOREACH(BWAPI::Unit * enemy, archivist->getEnemies())
	{
		// Check occupation.
		if (utilMap::inRegion(archivist->getPosition(enemy), region))
			return true;
	}

	// No occupants found.
	return false;
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
				// Check occupation.
				if (!isOccupied(region))
				{
					// Check availability.
					if ((!landlord->contains(region) || !landlord->getVassal(region)->getDepot()) &&
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
			}

			// Remove entry.
			queue.pop_front();
		}
	}

	// No expansion found.
	return NULL;
}