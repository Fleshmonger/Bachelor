#include "Primary.h"


// Constructor
Primary::Primary() :
	producer(&accountant),
	combatJudge(&archivist),
	architect(&accountant, &workerManager),
	reconnoiter(&archivist, &workerManager),
	armyManager(&archivist, &combatJudge),
	strategist(&producer, &architect),
	economist(&workerManager, &producer, &architect),
	attacker(&archivist, &combatJudge, &armyManager),
	defender(&archivist, &workerManager, &combatJudge, &armyManager)
{
}


// Deconstructor
Primary::~Primary()
{
}


// Fired on starting the game.
void Primary::onStart()
{
	// BWAPI settings.
	//BWAPI::Broodwar->enableFlag(Flag::UserInput);
	BWAPI::Broodwar->setLocalSpeed(0);

	// Read map information.
	BWTA::readMap();
	BWTA::analyze();

	// Update managers
	// TODO Move this to designator class?
	archivist.analyzed();
	architect.analyzed();
	economist.analyzed();
}


// Fired when the game is over.
void Primary::onEnd(bool isWinner)
{
	// Write output.
	std::ofstream outputFile("bwapi-data/write/Results.txt", std::ios::app);
	if (outputFile.is_open())
	{
		// Write to file.
		outputFile << (isWinner ? "Win " : "Loss ");						// Game result.
		outputFile << "\"" + BWAPI::Broodwar->enemy()->getName() + "\" ";	// Enemy name.
		outputFile << BWAPI::Broodwar->enemy()->getRace().getName() + " ";	// Enemy race.
		outputFile << BWAPI::Broodwar->mapFileName() + " ";					// Map name.
		outputFile << BWAPI::Broodwar->getFrameCount();						// Frame count.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->getUnitScore();				// Unit score.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->getBuildingScore();			// Building score.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->getKillScore();				// Kill score.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->getRazingScore();			// Razing score.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->gatheredMinerals();			// Total Minerals.
		outputFile << " ";
		outputFile << BWAPI::Broodwar->self()->gatheredGas();				// Total Minerals.

		// End stream.
		outputFile << "\n";
		outputFile.close();
	}
}


// Fired at the start of a new frame.
void Primary::onFrame()
{
	// Return if the game is a replay or is paused
	if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self())
		return;

	drawTerrainData();

	// Debugging display.
	double
		strength = combatJudge.strength(armyManager.getArmy()),
		enemyStrength = combatJudge.strength(archivist.getTroops()) + combatJudge.strength(archivist.getTurrets());
	DEBUG_SCREEN(200, 0, "FPS: %d", BWAPI::Broodwar->getFPS());
	DEBUG_SCREEN(200, 20, "APM: %d", BWAPI::Broodwar->getAPM());
	DEBUG_SCREEN(200, 40, "Strength: %f", strength);
	DEBUG_SCREEN(200, 60, "Enemy Strength: %f", enemyStrength);

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0)
		return;

	// Manager updatíng
	archivist.update();
	workerManager.update();
	architect.update();
	reconnoiter.update();
	armyManager.update();
	strategist.update();
	economist.update();
	defender.update(); //TODO Defender must be before attacker, to ensure defenders are available. Fix this.
	attacker.update();
}


void Primary::onSendText(std::string text)
{
}


void Primary::onReceiveText(BWAPI::Player* player, std::string text)
{
}


void Primary::onPlayerLeft(BWAPI::Player* player)
{
}


void Primary::onNukeDetect(BWAPI::Position target)
{
}


// Fired when a unit becomes visible for the first time.
void Primary::onUnitDiscover(BWAPI::Unit* unit)
{
	archivist.recordUnit(unit);
}


void Primary::onUnitEvade(BWAPI::Unit* unit)
{
}


// Fired when an invisible unit becomes visible.
void Primary::onUnitShow(BWAPI::Unit* unit)
{
}


void Primary::onUnitHide(BWAPI::Unit* unit)
{
}


// Fired when a unit is created.
// Units under construction triggers this.
void Primary::onUnitCreate(BWAPI::Unit* unit)
{
	//DEBUG_OUT("Unit Create: " + unit->getType().getName());
	// Monitor the construction of the unit.
	if (utilUnit::isOwned(unit))
	{
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect.completeBuild(unit);
		else
			producer.addProduction(unit);
	}
}


// Fired when a unit dies or is destroyed.
// TODO Do refineries trigger this?
void Primary::onUnitDestroy(BWAPI::Unit* unit)
{
	//Broodwar->printf("%s was destroyed", unit->getType().getName().c_str());
	//DEBUG_OUT("Unit Destroy: " + unit->getType().getName());
	// Determine owner.
	archivist.clearUnit(unit);
	if (utilUnit::isOwned(unit))
	{
		// Undesignate the destroyed unit.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
		{
			// Determine the type.
			if (unitType.isResourceDepot())
			{
				// TODO What should happen here?
				architect.setDepot(NULL);
				//producer.setDepot(NULL);
				workerManager.setDepot(NULL);
			}
			else if (unitType == BWAPI::UnitTypes::Protoss_Pylon)
				architect.removePylon(unit);
			else if (unitType == BWAPI::UnitTypes::Protoss_Gateway)
				producer.removeFactory(unit);
			// Remove constructing units from the architect.
			if (unit->isConstructing())
				architect.removeConstruct(unit);
		}
		else if (unitType.isWorker())
			workerManager.removeWorker(unit);
		else // Must be a combat unit
			armyManager.removeUnit(unit);
		// TODO if unit was incomplete, remove it from the producer.
	}
}


// Fires when a unit changes type.
// TODO Does building geyser emplacements trigger this?
void Primary::onUnitMorph(BWAPI::Unit* unit)
{
}


void Primary::onUnitRenegade(BWAPI::Unit* unit)
{
}


void Primary::onSaveGame(std::string gameName)
{
}


// Fires when a unit has finished construction.
// Both units and buildings trigger this.
void Primary::onUnitComplete(BWAPI::Unit *unit)
{
	//DEBUG_OUT("Unit Complete: %s", unit->getType().getName());
	// Determine owner.
	if (utilUnit::isOwned(unit))
	{
		// Update construction status.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect.completeConstruct(unit);
		//else
			//producer.completeUnit(unit);
		// Designate the new unit.
		designateUnit(unit);
	}
}


// Delivers a unit to managers who needs it.
// Assumes the unit is owned.
// TODO Remove this assumption.
// TODO Move to designator class.
void Primary::designateUnit(BWAPI::Unit * unit)
{
	// Determine type.
	BWAPI::UnitType unitType = unit->getType();
	if (unitType.isBuilding())
	{
		if (unitType.isResourceDepot())
		{
			architect.setDepot(unit);
			producer.addFactory(unit);
			workerManager.setDepot(unit);
			attacker.setDepot(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Protoss_Pylon)
			architect.addPylon(unit);
		else if (unitType == BWAPI::UnitTypes::Protoss_Gateway) // TODO Make generic
			producer.addFactory(unit);
	}
	else if (unitType.isWorker())
		workerManager.addWorker(unit);
	else // Must be a combat unit
		armyManager.addUnit(unit);
}

void Primary::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (std::set<BWTA::BaseLocation*>::const_iterator i = BWTA::getBaseLocations().begin(); i != BWTA::getBaseLocations().end(); i++)
	{
		TilePosition p = (*i)->getTilePosition();
		Position c = (*i)->getPosition();

		//draw outline of center location
		Broodwar->drawBox(CoordinateType::Map, p.x() * 32, p.y() * 32, p.x() * 32 + 4 * 32, p.y() * 32 + 3 * 32, Colors::Blue, false);

		//draw a circle at each mineral patch
		for (std::set<BWAPI::Unit*>::const_iterator j = (*i)->getStaticMinerals().begin(); j != (*i)->getStaticMinerals().end(); j++)
		{
			Position q = (*j)->getInitialPosition();
			Broodwar->drawCircle(CoordinateType::Map, q.x(), q.y(), 30, Colors::Cyan, false);
		}

		//draw the outlines of vespene geysers
		for (std::set<BWAPI::Unit*>::const_iterator j = (*i)->getGeysers().begin(); j != (*i)->getGeysers().end(); j++)
		{
			TilePosition q = (*j)->getInitialTilePosition();
			Broodwar->drawBox(CoordinateType::Map, q.x() * 32, q.y() * 32, q.x() * 32 + 4 * 32, q.y() * 32 + 2 * 32, Colors::Orange, false);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if ((*i)->isIsland())
			Broodwar->drawCircle(CoordinateType::Map, c.x(), c.y(), 80, Colors::Yellow, false);
	}

	//we will iterate through all the regions and draw the polygon outline of it in green.
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		BWTA::Polygon p = (*r)->getPolygon();
		for (int j = 0; j<(int)p.size(); j++)
		{
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map, point1.x(), point1.y(), point2.x(), point2.y(), Colors::Green);
		}
	}

	//we will visualize the chokepoints with red lines
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		for (std::set<BWTA::Chokepoint*>::const_iterator c = (*r)->getChokepoints().begin(); c != (*r)->getChokepoints().end(); c++)
		{
			Position point1 = (*c)->getSides().first;
			Position point2 = (*c)->getSides().second;
			Broodwar->drawLine(CoordinateType::Map, point1.x(), point1.y(), point2.x(), point2.y(), Colors::Red);
		}
	}
}