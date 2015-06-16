#include "Primary.h"


// Constructor
Primary::Primary() :
	accountant(),
	archivist(),
	baseManager(),
	geologist(),
	landlord(),
	recruiter(&accountant),
	combatJudge(&archivist),
	gatherer(&landlord),
	architect(&accountant, &baseManager, &landlord),
	reconnoiter(&archivist, &landlord),
	armyManager(&archivist, &combatJudge),
	morpher(&accountant, &gatherer),
	settler(&archivist, &landlord, &architect),
	attacker(&archivist, &landlord, &combatJudge, &armyManager),
	defender(&archivist, &landlord, &combatJudge, &armyManager),
	strategist(&accountant, &archivist, &landlord, &recruiter, &combatJudge, &architect, &armyManager),
	planner(&geologist, &landlord, &recruiter, &architect, &settler),
	economist(&accountant, &landlord, &recruiter, &gatherer, &architect, &settler, &strategist, &planner),
	despot(&landlord, &recruiter, &gatherer, &architect, &strategist, &planner, &economist)
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
	BWAPI::Broodwar->setLocalSpeed(2);

	// Read map information.
	BWTA::readMap();
	BWTA::analyze();

	// Initialize managers
	archivist.initialize();
	baseManager.initialize();
	geologist.initialize();
}


// Fired when the game is over.
void Primary::onEnd(bool isWinner)
{
	// Write output.
	/*
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
	*/
}


// Fired at the start of a new frame.
void Primary::onFrame()
{
	// Return if the game is a replay or is paused
	if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self())
		return;

	// Display.
	drawBaseManager();
	drawTerrainData();
	drawVassals();
	//drawGeologist();
	drawGatherer();
	drawLandlord();
	drawPlanner();
	Broodwar->drawTextScreen(200, 0, "Minerals: %d", accountant.minerals());
	Broodwar->drawTextScreen(200, 20, "Gas: %d", accountant.gas());
	Broodwar->drawTextScreen(200, 40, "Supply: %d", accountant.supply() / 2);
	//Broodwar->drawTextScreen(200, 0, "FPS: %d", BWAPI::Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "APM: %d", BWAPI::Broodwar->getAPM());
	//Broodwar->drawTextScreen(200, 40, "Scheduled refineries: %d", accountant.scheduled(BWAPI::UnitTypes::Protoss_Assimilator));
	//Broodwar->drawTextScreen(200, 60, "Scheduled pylons: %d", accountant.scheduled(BWAPI::UnitTypes::Protoss_Pylon));

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0)
		return;

	// Manager updatíng
	archivist.update();
	architect.update();
	reconnoiter.update();
	armyManager.update();
	morpher.update();
	planner.update();
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

	// Verify unit.
	if (unit)
	{
		// Check type.
		BWAPI::UnitType unitType = unit->getType();
		if (utilUnit::isGeyser(unitType))
			geologist.addGeyser(unit);
		else if (unitType.isRefinery())
			geologist.removeGeyser(unit);
	}
}


void Primary::onUnitEvade(BWAPI::Unit* unit)
{
}


// Fired when an invisible unit becomes visible.
void Primary::onUnitShow(BWAPI::Unit * unit)
{
}


void Primary::onUnitHide(BWAPI::Unit* unit)
{
}


// Fired when a unit is trained or built.
void Primary::onUnitCreate(BWAPI::Unit* unit)
{
	// Verify unit.
	if (unit)
	{
		// Check ownership.
		BWAPI::UnitType unitType = unit->getType();
		if (utilUnit::isOwned(unit))
		{
			// Check type.
			if (unitType.isBuilding())
			{
				baseManager.addBuilding(unit);
				architect.completeBuild(unit);
			}
			else if (!utilUnit::isMisc(unitType))
				recruiter.addConstruction(unit);
		}
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
				if (unitType.isBuilding())
					baseManager.removeBuilding(unit);
				if (unitType.isWorker())
				{
					landlord.removeWorker(unit);
					gatherer.removeWorker(unit);
				}
				if (utilUnit::isFighter(unitType))
					armyManager.removeUnit(unit);
				if (unitType.canProduce())
					recruiter.removeFactory(unit);
				if (unitType.isRefinery())
					gatherer.removeRefinery(unit);
				if (unitType.isResourceDepot())
					landlord.removeDepot(unit);
			}
			else
			{
				// Remove monitoring.
				morpher.removeMorph(unit);
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
	if (utilUnit::isOwned(unit))
	{
		// Monitor morpher.
		morpher.addMorph(unit);

		// Notify geologist.
		if (unit->getType().isRefinery())
			geologist.removeGeyser(unit);
	}
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
	if (unit)
	{
		// Check ownership.
		BWAPI::UnitType unitType = unit->getType();
		if (utilUnit::isOwned(unit))
		{
			// Notify producer.
			if (unitType.isBuilding())
				architect.removeConstruct(unit);
			else
				recruiter.removeConstruction(unit);

			// Check if expansion.
			if (unitType.isResourceDepot())
			{
				landlord.addDepot(unit);
				economist.maynardSlide(BWTA::getRegion(unit->getPosition()));
			}

			// Designate.
			if (unitType.isWorker())
				landlord.addWorker(unit);
			else if (utilUnit::isFighter(unitType))
				armyManager.addUnit(unit);
			else if (unitType.canProduce())
				recruiter.addFactory(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Resource_Vespene_Geyser)
			geologist.addGeyser(unit);
	}
}


// Draws baseManager data for debugging.
void Primary::drawBaseManager()
{
	for (int i = 0; i < BWAPI::Broodwar->mapWidth(); i++)
		for (int j = 0; j < BWAPI::Broodwar->mapHeight(); j++)
			if (baseManager.isReserved(i, j))
				BWAPI::Broodwar->drawLineMap(
				i * TILE_SIZE,
				j * TILE_SIZE,
				(i + 1) * TILE_SIZE,
				(j + 1) * TILE_SIZE,
				BWAPI::Colors::Yellow);
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


// Drawsd geologist data for debugging.
void Primary::drawGeologist()
{
	// Draw geysers' outlines.
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
		BOOST_FOREACH(BWAPI::Unit * geyser, geologist.getGeysers(region))
			drawBuildingOutline(geologist.getGeyserPosition(geyser), BWAPI::UnitTypes::Resource_Vespene_Geyser);
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
			BWAPI::Broodwar->drawTextMap(
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
	BOOST_FOREACH(Build build, planner.getBuildOrder())
	{
		// Draw build entry.
		BWAPI::Broodwar->drawTextScreen(
			0,
			line,
			build.first.getName().c_str()
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
void Primary::drawBuildingOutline(BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType)
{
	// Verify building.
	if (buildingLocation &&
		buildingType)
	{
		// Draw outline.
		BWAPI::Broodwar->drawBoxMap(
			buildingLocation.x() * TILE_SIZE,
			buildingLocation.y() * TILE_SIZE,
			(buildingLocation.x() + buildingType.tileWidth()) * TILE_SIZE,
			(buildingLocation.y() + buildingType.tileHeight()) * TILE_SIZE,
			BWAPI::Colors::Blue,
			false);
	}
}


// Draws an outline around the unit.
void Primary::drawBuildingOutline(BWAPI::Unit * building)
{
	// Verify building.
	if (building &&
		building->isVisible())
		drawBuildingOutline(building->getTilePosition(), building->getType());
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