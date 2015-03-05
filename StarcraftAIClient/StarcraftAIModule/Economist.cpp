#include "Economist.h"

//In the future needs access to Architect for refinery construction.
Economist::Economist(WorkerManager * workerManager, Producer * producer, Architect * architect)
{
	// Managers
	this->workerManager = workerManager;
	this->producer = producer;
	this->architect = architect;
}

//Unused deconstructor
Economist::~Economist()
{
}

// Designates the current depot used for cargo return.
void Economist::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}

void Economist::setMinerals(std::set<BWAPI::Unit*> minerals)
{
	this->minerals = minerals;
}

// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Order more workers;
	producer->trainUnit(PROTOSS_WORKER);

	// Get all available workers;
	std::set<Unit*> * workers = workerManager->getWorkers();

	// Harvest resources; This should be done at the end.
	if (depot)
	{
		// Iterate through all available workers.
		auto it = workers->begin();
		while (it != workers->end())
		{
			// Check if it exists.
			// This should be unecessary.
			BWAPI::Unit * harvester = *it;
			if (!harvester->exists())
				it = workers->erase(it);
			else
			{
				// Check if the worker is in a desirable state.
				if (!(harvester->isLockedDown() ||
					harvester->isMaelstrommed() ||
					harvester->isStasised() || // Ignore the unit if it has one of the following status ailments
					harvester->isLoaded() ||
					harvester->isStuck())) // Ignore the unit if it is in one of the following states
				{
					// if our worker is idle
					if (harvester->isIdle())
					{
						// Order workers carrying a resource to return them to the center,
						// otherwise find a mineral patch to harvest.
						if (harvester->isCarryingGas() || harvester->isCarryingMinerals())
						{
							harvester->returnCargo();
						}
						else if (!harvester->getPowerUp())  // The worker cannot harvest anything if it
						{                             // is carrying a powerup such as a flag
							// Harvest from the nearest mineral patch or gas refinery
							//harvester->gather(harvester->getClosestUnit(IsMineralField || IsRefinery));
						} // closure: has no powerup
					} // closure: if idle
				} // closure: is desirable
				++it;
			} // closure: exists
		} // closure: while end
	} // closure: has depot
}