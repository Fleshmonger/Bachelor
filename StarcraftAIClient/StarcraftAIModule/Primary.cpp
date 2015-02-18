#include "Primary.h"
#include <iostream>

void Primary::onStart()
{
	// BWAPI Settings
	Broodwar->enableFlag(Flag::UserInput);
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	Broodwar->setCommandOptimizationLevel(2);

	// Managers Initialization
	accountant = new Accountant();
	producer = new Producer(accountant);
	workerManager = new WorkerManager();
	architect = new Architect(accountant, workerManager);
	reconnoiter = new Reconnoiter(workerManager);
	economist = new Economist(producer, workerManager, architect);
	armyManager = new ArmyManager(producer, architect);

	// Designate all starting units.
	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getPlayer() == Broodwar->self()) // Make this a method
			designateUnit(u);
	}

	// BWTA initialization
	/*
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;
	*/
}

void Primary::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void Primary::onFrame()
{
	// Display
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 20, "Minerals: %d", accountant->minerals());
	Broodwar->drawTextScreen(200, 40, "APM: %d", Broodwar->getAPM());

	// BWTA draw
	/*
	if (analyzed)
	drawTerrainData();
	if (analysis_just_finished)
	{
	Broodwar << "Finished analyzing map." << std::endl;;
	analysis_just_finished = false;
	}
	*/

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// Manager updat�ng
	producer->update();
	architect->update();
	reconnoiter->update();
	economist->update();
	armyManager->update();

	//OLD
	/*
	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;


		// Finally make the unit do some stuff!

		// If the unit is a worker unit
		if (u->getType().isWorker())
		{
			// if our worker is idle
			if (u->isIdle())
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (u->isCarryingGas() || u->isCarryingMinerals())
				{
					u->returnCargo();
				}
				else if (!u->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
					if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
					{
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle

		}
		else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
		{

			// Order the depot to construct more workers! But only when it is idle.
			if (u->isIdle() && !u->train(u->getType().getRace().getWorker()))
			{
				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame
				// so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
					nullptr,    // condition
					Broodwar->getLatencyFrames());  // frames to run

				// Retrieve the supply provider type in the case that we have run out of supplies
				UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (lastErr == Errors::Insufficient_Supply &&
					lastChecked + 400 < Broodwar->getFrameCount() &&
					Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
				{
					lastChecked = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed
					Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
						(IsIdle || IsGatheringMinerals) &&
						IsOwned);
					// If a unit was found
					if (supplyBuilder)
					{
						if (supplyProviderType.isBuilding())
						{
							TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
							if (targetBuildLocation)
							{
								// Register an event that draws the target build location
								Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
								{
									Broodwar->drawBoxMap(Position(targetBuildLocation),
										Position(targetBuildLocation + supplyProviderType.tileSize()),
										Colors::Blue);
								},
									nullptr,  // condition
									supplyProviderType.buildTime() + 100);  // frames to run

								// Order the builder to construct the supply structure
								supplyBuilder->build(supplyProviderType, targetBuildLocation);
							}
						}
						else
						{
							// Train the supply provider (Overlord) if the provider is not a structure
							supplyBuilder->train(supplyProviderType);
						}
					} // closure: supplyBuilder is valid
				} // closure: insufficient supply
			} // closure: failed to train idle unit

		}

	} // closure: unit iterator
	*/
}

//Much of below is unchanged from the original module example!

void Primary::onSendText(std::string text)
{
	/*
	if (text == "/analyze")
	{
		if (analyzed = false)
		{
			Broodwar << "Analyzing map... this may take a minute" << std::endl;;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
	}
	else*/
	{
		// Send the text to the game if it is not being processed.
		Broodwar->sendText("%s", text.c_str());
	}
	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void Primary::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Primary::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void Primary::onNukeDetect(BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void Primary::onUnitDiscover(BWAPI::Unit unit)
{
}

void Primary::onUnitEvade(BWAPI::Unit unit)
{
}

// Fired when a unit previously obscured is seen by the bot.
//Notice, this also fires when a unit is being constructed!
void Primary::onUnitShow(BWAPI::Unit unit)
{
	//Broodwar->sendText("Unit Show");
	if (Broodwar->self()->isEnemy(unit->getPlayer())) //Use isOwned somehow
	{
		if (unit->getType().isBuilding())
			armyManager->addEnemyBuilding(unit);
		else
			armyManager->addEnemyTroop(unit);
	}
}

void Primary::onUnitHide(BWAPI::Unit unit)
{
}

void Primary::onUnitCreate(BWAPI::Unit unit)
{
	Broodwar << "Unit Create: " << unit->getType().getName() << std::endl;;
	if (unit->getPlayer() == Broodwar->self()) //Use isOwned somehow
	{
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect->updateBuildOrder(unit);
	}
}

// Fired when a visible unit is destroyed.
void Primary::onUnitDestroy(BWAPI::Unit unit) // Merge with onUnitCompleted somehow - code duplication!
{
	//Broodwar->sendText("Unit Destroy");
	if (unit->getPlayer() == Broodwar->self()) //Use isOwned somehow
	{
		// Undesignate the destroyed unit.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding()) // Check if unit is building.
		{
			if (unitType.isResourceDepot()) // Check if building is Command Center, Nexus or Hatchery.
			{
				// What should happen here?
				architect->setDepot(nullptr);
				economist->setDepot(nullptr);
				producer->setDepot(nullptr);
			}
			else if (unitType == BWAPI::UnitTypes::Protoss_Pylon)
				architect->removePylon(unit);
			else if (unitType == BWAPI::UnitTypes::Protoss_Gateway)
				producer->removeInfantryFacility(unit);
			// Remove an order from the architect, if this building was part of it.
		}
		else if (unitType.isWorker()) // Check if worker
			workerManager->removeWorker(unit);
		else // Must be a combat unit // Could be some spawned unit, like fighters?
			armyManager->removeUnit(unit);
		// TODO If unit was under construction, resources should be deallocated!
	}
	else if (Broodwar->self()->isEnemy(unit->getPlayer()))
	{
		if (unit->getType().isBuilding())
			armyManager->removeEnemyBuilding(unit);
		else
			armyManager->removeEnemyTroop(unit);
	}
}

void Primary::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void Primary::onUnitRenegade(BWAPI::Unit unit)
{
}

void Primary::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void Primary::onUnitComplete(BWAPI::Unit unit)
{
	//Broodwar->sendText("Unit Complete");
	if (unit->getPlayer() == Broodwar->self()) //Use isOwned somehow
	{
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
		{
			architect->updateConstructOrder(unitType);
			if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
				architect->addPylon(unit);
		}
		designateUnit(unit);
	}
}

void Primary::designateUnit(BWAPI::Unit unit)
{
	// Designate the new unit.
	BWAPI::UnitType unitType = unit->getType();
	if (unitType.isBuilding()) // Check if unit is building.
	{
		if (unitType.isResourceDepot()) // Check if building is Command Center, Nexus or Hatchery.
		{
			architect->setDepot(unit);
			economist->setDepot(unit);
			producer->setDepot(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Protoss_Gateway)
			producer->addInfantryFacility(unit);
	}
	else if (unitType.isWorker()) // Check if worker
		workerManager->addWorker(unit);
	else // Must be a combat unit
		armyManager->addUnit(unit);
}

/*
DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;
	return 0;
}
*/

/*
void Primary::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (std::set<BWTA::BaseLocation*>::const_iterator i = BWTA::getBaseLocations().begin(); i != BWTA::getBaseLocations().end(); i++)
	{
		TilePosition p = (*i)->getTilePosition();
		Position c = (*i)->getPosition();

		//draw outline of center location
		Broodwar->drawBoxMap(p.x * 32, p.y * 32, p.x * 32 + 4 * 32, p.y * 32 + 3 * 32, Colors::Blue);

		//draw a circle at each mineral patch
		for (Unitset::iterator j = (*i)->getStaticMinerals().begin(); j != (*i)->getStaticMinerals().end(); j++)
		{
			Position q = j->getInitialPosition();
			Broodwar->drawCircleMap(q.x, q.y, 30, Colors::Cyan);
		}

		//draw the outlines of vespene geysers
		for (Unitset::iterator j = (*i)->getGeysers().begin(); j != (*i)->getGeysers().end(); j++)
		{
			TilePosition q = j->getInitialTilePosition();
			Broodwar->drawBoxMap(q.x * 32, q.y * 32, q.x * 32 + 4 * 32, q.y * 32 + 2 * 32, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if ((*i)->isIsland())
			Broodwar->drawCircleMap(c, 80, Colors::Yellow);
	}

	//we will iterate through all the regions and draw the polygon outline of it in green.
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		BWTA::Polygon p = (*r)->getPolygon();
		for (int j = 0; j<(int)p.size(); j++)
		{
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
	}

	//we will visualize the chokepoints with red lines
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		for (std::set<BWTA::Chokepoint*>::const_iterator c = (*r)->getChokepoints().begin(); c != (*r)->getChokepoints().end(); c++)
		{
			Position point1 = (*c)->getSides().first;
			Position point2 = (*c)->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}
*/