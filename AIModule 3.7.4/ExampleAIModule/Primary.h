#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "Producer.h"
#include "CombatJudge.h" // TEMP
#include "Architect.h"
#include "Economist.h"
#include "Reconnoiter.h"
#include "ArmyManager.h"

using namespace BWAPI;


class Primary : public BWAPI::AIModule
{
#define DEBUG
#ifdef DEBUG
#define DEBUG_OUT(s) Broodwar->printf(s);
#define DEBUG_SCREEN(x, y, s, d) Broodwar->drawTextScreen(x, y, s, d);
#else
#define DEBUG_OUT(s)
#define DEBUG_SCREEN(x, y, s, d)
#endif

private:
	Accountant accountant;
	Archivist archivist;
	Producer producer;
	WorkerManager workerManager;
	Architect architect;
	Reconnoiter reconnoiter;
	Economist economist;
	ArmyManager armyManager;

	CombatJudge combatJudge;
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

	void designateUnit(BWAPI::Unit * unit);
};
