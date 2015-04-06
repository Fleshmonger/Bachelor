#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "utilUnit.h"
#include "Archivist.h"
#include "WorkerManager.h"
#include "ArmyManager.h"


// Commands units to defend.
class Defender
{
private:
	Archivist		* archivist;
	WorkerManager	* workerManager;
	CombatJudge		* combatJudge;
	ArmyManager		* armyManager;

public:
	Defender(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge, ArmyManager * armyManager);
	~Defender();

	void update();
};

