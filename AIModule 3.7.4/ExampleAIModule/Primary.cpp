#include "Primary.h"

// Local
/*
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
*/

void Primary::onStart()
{
	Broodwar->enableFlag(Flag::UserInput);

	//read map information into BWTA so terrain analysis can be done in another thread
	BWTA::readMap();
	analyzed = false;

	/*
	analysis_just_finished=false;

	show_bullets=false;
	show_visibility_data=false;
	*/

	// Managers Initialization
	accountant = new Accountant();
	workerManager = new WorkerManager();
	reconnoiter = new Reconnoiter(workerManager);
	producer = new Producer(accountant);
	architect = new Architect(workerManager, accountant);
	economist = new Economist(workerManager, producer, architect);
	armyManager = new ArmyManager(workerManager, producer, architect);


	// Designate all starting units.
	// This is already done; unitComplete is called on all starting units.
	/*
	BOOST_FOREACH (BWAPI::Unit * u, Broodwar->self()->getUnits())
	{
		if (isOwned(u))
			designateUnit(u);
	}
	*/
  /*
  // OLD
  if (Broodwar->isReplay())
  {
    Broodwar->printf("The following players are in this replay:");
    for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
    {
      if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
      {
        Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
      }
    }
  }
  else
  {
    Broodwar->printf("The match up is %s v %s",
      Broodwar->self()->getRace().getName().c_str(),
      Broodwar->enemy()->getRace().getName().c_str());


    //send each worker to the mineral field that is closest to it
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
      if ((*i)->getType().isWorker())
      {
        Unit* closestMineral=NULL;
        for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
        {
          if (closestMineral==NULL || (*i)->getDistance(*m)<(*i)->getDistance(closestMineral))
            closestMineral=*m;
        }
        if (closestMineral!=NULL)
          (*i)->rightClick(closestMineral);
      }
      else if ((*i)->getType().isResourceDepot())
      {
        //if this is a center, tell it to build the appropiate type of worker
        if ((*i)->getType().getRace()!=Races::Zerg)
        {
          (*i)->train(Broodwar->self()->getRace().getWorker());
        }
        else //if we are Zerg, we need to select a larva and morph it into a drone
        {
          std::set<Unit*> myLarva=(*i)->getLarva();
          if (myLarva.size()>0)
          {
            Unit* larva=*myLarva.begin();
            larva->morph(UnitTypes::Zerg_Drone);
          }
        }
      }
    }
  }
  */
}

void Primary::onEnd(bool isWinner)
{
	if (isWinner)
	{
		//log win to file
	}
}

void Primary::onFrame()
{
	// Debugging display.
	DEBUG_SCREEN(200, 0, "FPS: %d", Broodwar->getFPS());
	DEBUG_SCREEN(200, 20, "APM: %d", Broodwar->getAPM());
	//DEBUG_SCREEN(200, 40, "Average FPS: %f", Broodwar->getAverageFPS());
	//DEBUG_SCREEN(200, 40, "Unallocated Minerals: %d", accountant->minerals());
	//DEBUG_SCREEN(200, 40, "Scheduled Gateways: %d", architect->scheduled(BWAPI::UnitTypes::Protoss_Gateway));
	//DEBUG_SCREEN(200, 40, "Workers: %d", workerManager->workers());
	if (Broodwar->getFrameCount() == 0)
	{
		BWTA::analyze();
		// TODO Move this to the coming intelligence manager.
		// Add new information to managers.
		BOOST_FOREACH(BWAPI::Unit * mineral, BWTA::getStartLocation(Broodwar->self())->getStaticMinerals())
		{
			workerManager->addMineral(mineral);
			architect->includeResource(mineral);
		}
		BOOST_FOREACH(BWAPI::Unit * geyser, BWTA::getStartLocation(Broodwar->self())->getGeysers())
			architect->includeResource(geyser);

		armyManager->setHomeRegion(BWTA::getStartLocation(Broodwar->self())->getRegion());
	}

	// DEBUG
	Zone test = architect->resources;
	//Zone test = Zone(BWAPI::TilePosition(5, 5), 1, 2);
	Broodwar->drawBox(
		CoordinateType::Map,
		test.origin.x() * 32,
		test.origin.y() * 32,
		(test.origin.x() + test.width) * 32,
		(test.origin.y() + test.height) * 32,
		Colors::Blue,
		false);

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// Manager updatíng
	producer->update();
	architect->update();
	reconnoiter->update();
	economist->update();
	armyManager->update();
	workerManager->update();

	/*
  if (show_visibility_data)
    drawVisibilityData();

  if (show_bullets)
    drawBullets();

  if (Broodwar->isReplay())
    return;

  drawStats();
  if (analyzed && Broodwar->getFrameCount()%30==0)
  {
    //order one of our workers to guard our chokepoint.
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
      if ((*i)->getType().isWorker())
      {
        //get the chokepoints linked to our home region
        std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
        double min_length=10000;
        BWTA::Chokepoint* choke=NULL;

        //iterate through all chokepoints and look for the one with the smallest gap (least width)
        for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
        {
          double length=(*c)->getWidth();
          if (length<min_length || choke==NULL)
          {
            min_length=length;
            choke=*c;
          }
        }

        //order the worker to move to the center of the gap
        (*i)->rightClick(choke->getCenter());
        break;
      }
    }
  }
  if (analyzed)
    drawTerrainData();

  if (analysis_just_finished)
  {
    Broodwar->printf("Finished analyzing map.");
    analysis_just_finished=false;
  }*/
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

void Primary::onUnitDiscover(BWAPI::Unit* unit)
{
	if (isEnemy(unit))
		armyManager->addEnemy(unit);
}

void Primary::onUnitEvade(BWAPI::Unit* unit)
{
}

void Primary::onUnitShow(BWAPI::Unit* unit)
{
}

void Primary::onUnitHide(BWAPI::Unit* unit)
{
}

// Called on initially created units
void Primary::onUnitCreate(BWAPI::Unit* unit)
{
	//DEBUG_OUT("Unit Create: " + unit->getType().getName());
	// Monitor the construction of the unit.
	if (isOwned(unit))
	{
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect->completeBuild(unit);
		else
			producer->incompleteUnit(unit);
	}
}

void Primary::onUnitDestroy(BWAPI::Unit* unit)
{
	//DEBUG_OUT("Unit Destroy: " + unit->getType().getName());
	// Determine owner.
	if (isOwned(unit))
	{
		// Undesignate the destroyed unit.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
		{
			// Determine the type.
			if (unitType.isResourceDepot())
			{
				// TODO What should happen here?
				architect->setDepot(NULL);
				//economist->setDepot(NULL);
				producer->setDepot(NULL);
				workerManager->setDepot(NULL);
			}
			else if (unitType == BWAPI::UnitTypes::Protoss_Pylon)
				architect->removePylon(unit);
			else if (unitType == BWAPI::UnitTypes::Protoss_Gateway)
				producer->removeInfantryFacility(unit);
			// Remove constructing units from the architect.
			if (unit->isConstructing())
				architect->removeConstruct(unit);
		}
		else if (unitType.isWorker())
			workerManager->removeWorker(unit);
		else // Must be a combat unit // TODO Could be some spawned unit, like fighters or larva?
			armyManager->removeUnit(unit);
		// TODO if unit was incomplete, remove it from the producer.
	}
	else if (isEnemy(unit))
		armyManager->removeEnemy(unit);
}

void Primary::onUnitMorph(BWAPI::Unit* unit)
{
	/*
	if (!Broodwar->isReplay())
		Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		//if we are in a replay, then we will print out the build order
		//(just of the buildings, not the units).
		if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
		{
			int seconds=Broodwar->getFrameCount()/24;
			int minutes=seconds/60;
			seconds%=60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
		}
	}
	*/
}

void Primary::onUnitRenegade(BWAPI::Unit* unit)
{
}

void Primary::onSaveGame(std::string gameName)
{
}

void Primary::onUnitComplete(BWAPI::Unit *unit)
{
	//DEBUG_OUT("Unit Complete: %s", unit->getType().getName());
	// Determine owner.
	if (isOwned(unit))
	{
		// Update construction status.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect->completeConstruct(unit);
		else
			producer->completeUnit(unit);
		// Designate the new unit.
		designateUnit(unit);
	}
}

// Returns true if the unit is owned and false otherwise.
// TODO Move this to some unit monitor class?
// TODO This can probably be done cheaper.
bool Primary::isOwned(BWAPI::Unit * unit)
{
	return unit->getPlayer() == Broodwar->self();
}

// Returns true if the unit is owned by an enemy and false otherwise.
// TODO Move this to some unit monitor class?
// TODO This can probably be done cheaper.
bool Primary::isEnemy(BWAPI::Unit * unit)
{
	return Broodwar->self()->isEnemy(unit->getPlayer());
}

// Delivers a unit to managers who needs it.
// Assumes the unit is owned.
void Primary::designateUnit(BWAPI::Unit * unit)
{
	// Determine type.
	BWAPI::UnitType unitType = unit->getType();
	if (unitType.isBuilding())
	{
		if (unitType.isResourceDepot())
		{
			architect->setDepot(unit);
			//economist->setDepot(unit);
			producer->setDepot(unit);
			workerManager->setDepot(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Protoss_Pylon)
			architect->addPylon(unit);
		else if (unitType == BWAPI::UnitTypes::Protoss_Gateway) // TODO Make generic
			producer->addInfantryFacility(unit);
	}
	else if (unitType.isWorker())
		workerManager->addWorker(unit);
	else // Must be a combat unit
		armyManager->addUnit(unit);
}