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
		typedef std::pair<unsigned int, BWTA::Region*> Location;
		std::list<Location> queue;
		std::set<BWTA::Region*> visited;
		queue.push_front(std::make_pair(0, main->getRegion()));

		// Iterate through queue.
		while (!queue.empty())
		{
			// Verify Location.
			BWTA::Region * region = queue.front().second;
			if (region &&				// Verify region.
				!isOccupied(region))	// Check occupation.
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
						// Aquire neighbor.
						std::pair<BWTA::Region*, BWTA::Region*> neighbors = border->getRegions();
						BWTA::Region * neighbor;
						if (neighbors.first == region)
							neighbor = neighbors.second;
						else
							neighbor = neighbors.first;
						
						// Verify unvisited.
						if (visited.count(neighbor) == 0)
						{
							// Find location in queue.
							unsigned int distance = queue.front().first + region->getCenter().getApproxDistance(neighbor->getCenter());
							std::list<Location>::iterator it = queue.begin();
							while (it != queue.end() && (*it).first < distance)
								it++;
							
							// Add entry.
							queue.insert(it, std::make_pair(distance, neighbor));
						}
					}
				}
			}

			// Remove entry.
			queue.pop_front();
			visited.insert(region);
		}
	}

	// No expansion found.
	return NULL;
}