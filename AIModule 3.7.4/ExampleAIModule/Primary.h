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
#include "LandLord.h"
#include "Recruiter.h"
#include "CombatJudge.h"
#include "Architect.h"
#include "Reconnoiter.h"
#include "Economist.h"
#include "ArmyManager.h"
#include "Defender.h"
#include "Attacker.h"
#include "Strategist.h"
#include "Planner.h"


using namespace BWAPI; //TODO Remove this


// Handles callbacks and initializes and updates all sub-agents.
//TODO Rename to something module related?
class Primary : public BWAPI::AIModule
{
private:
	Accountant		accountant;
	Archivist		archivist;
	Landlord		landlord;
	Recruiter		recruiter;
	CombatJudge		combatJudge;
	Architect		architect;
	Reconnoiter		reconnoiter;
	Economist		economist;
	ArmyManager		armyManager;
	Attacker		attacker;
	Defender		defender;
	Strategist		strategist;
	Planner			planner;

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

	//void designateUnit(BWAPI::Unit * unit);
	void drawTerrainData();
};
