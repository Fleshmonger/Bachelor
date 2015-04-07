#include "Primary.h"


// Constructor
Primary::Primary() :
	producer(&accountant),
	combatJudge(&archivist),
	architect(&accountant, &workerManager),
	reconnoiter(&archivist, &workerManager),
	armyManager(&archivist, &workerManager, &combatJudge),
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
	BWAPI::Broodwar->enableFlag(Flag::UserInput);
	//BWAPI::Broodwar->setLocalSpeed(0);

	// Read map information.
	BWTA::readMap();
	BWTA::analyze();

	// Update managers
	// TODO Move this to designator class?
	archivist.analyzed();
	architect.analyzed();
	economist.analyzed();
}


void Primary::onEnd(bool isWinner)
{
}


// Fired at the start of a new frame.
void Primary::onFrame()
{
	// Debugging display.
	DEBUG_SCREEN(200, 0, "FPS: %d", BWAPI::Broodwar->getFPS());
	DEBUG_SCREEN(200, 20, "APM: %d", BWAPI::Broodwar->getAPM());

	// Testing
	double
		strength = combatJudge.strength(armyManager.getArmy()),
		enemyStrength = combatJudge.strength(archivist.getTroops()) + combatJudge.strength(archivist.getTurrets());
	DEBUG_SCREEN(200, 40, "Strength: %f", strength);
	DEBUG_SCREEN(200, 60, "Enemy Strength: %f", enemyStrength);

	// Return if the game is a replay or is paused
	if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self())
		return;

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


// Fired when a unit is shown for the first time.
void Primary::onUnitDiscover(BWAPI::Unit* unit)
{
	if (utilUnit::isEnemy(unit))
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
	else if (utilUnit::isEnemy(unit))
		archivist.clearUnit(unit);
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