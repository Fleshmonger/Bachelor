#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"
#include "Archivist.h"
#include "BaseManager.h"
#include "Geologist.h"
#include "LandLord.h"
#include "Recruiter.h"
#include "CombatJudge.h"
#include "Gatherer.h"
#include "Architect.h"
#include "Reconnoiter.h"
#include "ArmyManager.h"
#include "Morpher.h"
#include "Settler.h"
#include "Attacker.h"
#include "Defender.h"
#include "Strategist.h"
#include "Planner.h"
#include "Economist.h"
#include "Despot.h"


using namespace BWAPI; //TODO Remove this


// Handles callbacks and initializes and updates all sub-agents.
//TODO Rename to something module related?
class Primary : public BWAPI::AIModule
{
private:
	Accountant		accountant;
	Archivist		archivist;
	BaseManager		baseManager;
	Geologist		geologist;
	Landlord		landlord;
	Recruiter		recruiter;
	CombatJudge		combatJudge;
	Gatherer		gatherer;
	Architect		architect;
	Reconnoiter		reconnoiter;
	ArmyManager		armyManager;
	Morpher			morpher;
	Settler			settler;
	Attacker		attacker;
	Defender		defender;
	Strategist		strategist;
	Planner			planner;
	Economist		economist;
	Despot			despot;

public:
	Primary();
	~Primary();

	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit *unit);

	void drawBaseManager();
	void drawGatherer();
	void drawGeologist();
	void drawLandlord();
	void drawPlanner();
	void drawVassals();
	void drawBuildingOutline(BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType);
	void drawBuildingOutline(BWAPI::Unit * building);
	void drawUnitOutline(BWAPI::Unit * unit);
	void drawTerrainData();
};
