#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "WorkerManager.h"
#include "Accountant.h"
#include "Reconnoiter.h"
#include "Producer.h"
#include "Architect.h"
#include "Economist.h"
#include "ArmyManager.h"

using namespace BWAPI;
//using namespace BWTA;
using namespace Filter;

//DWORD WINAPI AnalyzeThread();

class Primary : public BWAPI::AIModule
{
private:
	/*
	bool analyzed;
	bool analysis_just_finished;
	*/
	Accountant * accountant;
	Producer * producer;
	WorkerManager * workerManager;
	Architect * architect;
	Reconnoiter * reconnoiter;
	Economist * economist;
	ArmyManager * armyManager;
public:
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
	// Everything below this line is safe to modify.
	void designateUnit(BWAPI::Unit unit);
	bool isOwned(BWAPI::Unit unit);
	bool isEnemy(BWAPI::Unit unit);
	//void drawTerrainData();
};