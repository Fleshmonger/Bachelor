#include "Primary.h"


// Constructor
Primary::Primary() :
	accountant(),
	archivist(),
	landlord(),
	recruiter(&accountant),
	combatJudge(&archivist),
	gatherer(&landlord),
	settler(&landlord),
	architect(&accountant, &landlord),
	reconnoiter(&archivist, &landlord),
	planner(&landlord, &recruiter, &settler, &architect),
	economist(&accountant, &landlord, &recruiter, &gatherer, &architect),
	armyManager(&archivist, &combatJudge),
	defender(&archivist, &landlord, &combatJudge, &armyManager),
	attacker(&archivist, &landlord, &combatJudge, &armyManager),
	strategist(&landlord, &recruiter, &architect),
	despot(&landlord, &recruiter, &gatherer, &architect, &planner, &economist, &strategist)
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
	BWAPI::Broodwar->enableFlag(Flag::UserInput);
	BWAPI::Broodwar->setLocalSpeed(0);

	// Read map information.
	BWTA::readMap();
	BWTA::analyze();

	// Update managers
	// TODO Move this to designator class?
	archivist.analyzed();
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


	// Debugging display.
	Broodwar->drawTextScreen(200, 0, "FPS: %d", BWAPI::Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "APM: %d", BWAPI::Broodwar->getAPM());
	Broodwar->drawTextScreen(200, 40, "Nexus scheduled: %s", architect.scheduled(BWAPI::UnitTypes::Protoss_Nexus) > 0 ? "Yes" : "No");
	drawVassals();
	drawGatherer();
	drawLandlord();
	drawPlanner();

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0)
		return;

	// Manager updatíng
	archivist.update();
	reconnoiter.update();
	architect.update();
	planner.update();
	armyManager.update();
	defender.update(); //TODO Defender must be before attacker and economist, to ensure defenders are available. Fix this.
	attacker.update();
	despot.update();
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
	// Record unit.
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


// Fired when a unit is trained or built.
void Primary::onUnitCreate(BWAPI::Unit* unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit))
	{
		// Type check.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect.completeBuild(unit);
		else if (!utilUnit::isMisc(unitType))
			recruiter.addConstruction(unit);
	}
}


// Fired when a unit dies or is destroyed.
void Primary::onUnitDestroy(BWAPI::Unit* unit)
{
	// Clear unit record.
	archivist.clearUnit(unit);

	// Verify unit.
	if (unit)
	{
		// Allegience check.
		BWAPI::UnitType unitType = unit->getType();
		if (utilUnit::isOwned(unit))
		{
			// Completion check.
			if (unit->isCompleted())
			{
				// Undesignate.
				if (unitType.isWorker())
				{
					landlord.removeWorker(unit);
					gatherer.removeWorker(unit);
				}
				else if (utilUnit::isFighter(unitType))
					armyManager.removeUnit(unit);
				else if (unitType.canProduce())
					recruiter.addFactory(unit);
				else if (unitType.isRefinery())
					gatherer.removeRefinery(unit);
			}
			else
			{
				// Remove monitoring.
				if (unitType.isBuilding())
					architect.removeConstruct(unit);
				else
					recruiter.removeConstruction(unit);
			}
		}
		else if (unitType.isMineralField())
			gatherer.removeMineral(unit);
	}

}


// Fired when a unit changes type including built refineries.
void Primary::onUnitMorph(BWAPI::Unit* unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit) &&
		unit->getType().isRefinery())
		economist.addRefinery(unit);
}


void Primary::onUnitRenegade(BWAPI::Unit* unit)
{
}


void Primary::onSaveGame(std::string gameName)
{
}


// Fired when a unit is completed or constructed.
void Primary::onUnitComplete(BWAPI::Unit *unit)
{
	// Verify unit.
	if (utilUnit::isOwned(unit))
	{
		// Notify architect.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect.removeConstruct(unit);
		else
			recruiter.removeConstruction(unit);

		// Check if expansion.
		if (unitType.isResourceDepot())
			landlord.addDepot(unit);

		// Designate.
		if (unitType.isWorker())
			landlord.addWorker(unit);
		else if (utilUnit::isFighter(unitType))
			armyManager.addUnit(unit);
		else if (unitType.canProduce())
			recruiter.addFactory(unit);
	}
}


// Draws gatherer data for debugging.
void Primary::drawGatherer()
{
	// Iterate through vassals.
	BOOST_FOREACH(Vassal * vassal, landlord.getVassals())
	{
		// Draw minerals.
		BWTA::Region * region = vassal->getRegion();
		BOOST_FOREACH(BWAPI::Unit * mineral, gatherer.getMinerals(region))
		{
			// Draw mineral.
			drawBuildingOutline(mineral);

			// Draw data.
			BWAPI::TilePosition tile = mineral->getTilePosition();
			BWAPI::Broodwar->drawTextMap(
				tile.x() * TILE_SIZE,
				tile.y() * TILE_SIZE,
				"Miners: %d",
				gatherer.getResourceWorkers(mineral).size()
				);
		}

		// Draw refineries.
		BOOST_FOREACH(BWAPI::Unit * refinery, gatherer.getRefineries(region))
		{
			// Draw mineral.
			drawBuildingOutline(refinery);

			// Draw data.
			BWAPI::TilePosition tile = refinery->getTilePosition();
			BWAPI::Broodwar->drawTextMap(
				tile.x() * TILE_SIZE,
				tile.y() * TILE_SIZE,
				"Harvesters: %d",
				gatherer.getResourceWorkers(refinery).size()
				);
		}
	}
}


// Draws landlord data for debugging.
void Primary::drawLandlord()
{
	// Iterate through vassals.
	BOOST_FOREACH(Vassal * vassal, landlord.getVassals())
	{
		// Iterate through builders.
		BOOST_FOREACH(BWAPI::Unit * worker, vassal->getEmployed(TASK_BUILD))
		{
			// Draw worker.
			drawUnitOutline(worker);

			// Draw worker data.
			BWAPI::Position pos = worker->getPosition();
			BWAPI::Broodwar->drawTextScreen(
				pos.x(),
				pos.y(),
				"Builder"
				);
		}
	}
}


// Draws planner data for debugging.
void Primary::drawPlanner()
{
	// Iterate through build order.
	int line = 0;
	std::list<BWAPI::UnitType> test;
	BOOST_FOREACH(BWAPI::UnitType unitType, planner.getBuildOrder())
	{
		// Draw build entry.
		BWAPI::Broodwar->drawTextScreen(
			0,
			line,
			unitType.getName().c_str()
			);
		line += 20;
	}
}


// Draws vassal data for debugging.
void Primary::drawVassals()
{
	// Iterate through vassals.
	BOOST_FOREACH(Vassal * vassal, landlord.getVassals())
	{
		// Verify depot.
		BWAPI::Unit * depot = vassal->getDepot();
		if (depot)
		{
			// Draw harvesting zone.
			utilMap::Zone zone = vassal->getHarvestingZone();
			BWAPI::Broodwar->drawBoxMap(
				zone.left * TILE_SIZE,
				zone.top * TILE_SIZE,
				zone.right * TILE_SIZE,
				zone.bottom * TILE_SIZE,
				BWAPI::Colors::Purple,
				false);

			// Draw depot.
			drawBuildingOutline(depot);

			// Draw data.
			BWAPI::TilePosition tile = depot->getTilePosition();
			BWAPI::Broodwar->drawTextMap(
				tile.x() * TILE_SIZE,
				tile.y() * TILE_SIZE,
				"Workforce: %d",
				vassal->workforce()
				);
		}
	}
}


// Draws an outline around the unit.
void Primary::drawBuildingOutline(BWAPI::Unit * building)
{
	// Verify unit.
	if (building &&
		building->isVisible())
	{
		// Draw unit.
		BWAPI::TilePosition tile = building->getTilePosition();
		BWAPI::UnitType buildingType = building->getType();
		BWAPI::Broodwar->drawBoxMap(
			tile.x() * TILE_SIZE,
			tile.y() * TILE_SIZE,
			(tile.x() + buildingType.tileWidth()) * TILE_SIZE,
			(tile.y() + buildingType.tileHeight()) * TILE_SIZE,
			BWAPI::Colors::Blue,
			false);
	}
}


// Draws an outline around the unit.
void Primary::drawUnitOutline(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->isVisible())
	{
		// Draw unit.
		BWAPI::Position pos = unit->getPosition();
		BWAPI::UnitType unitType = unit->getType();
		BWAPI::Broodwar->drawBoxMap(
			pos.x() + unitType.dimensionLeft(),
			pos.y() + unitType.dimensionUp(),
			pos.x() + unitType.dimensionRight(),
			pos.y() + unitType.dimensionDown(),
			BWAPI::Colors::Blue,
			false);
	}
}


/*
// Draws BWTA regions and resources.
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

		//if this is a start location, draw a green circle around the base location
		if ((*i)->isStartLocation())
			Broodwar->drawCircle(CoordinateType::Map, c.x(), c.y(), 100, Colors::Green, false);
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
*/