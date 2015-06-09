#pragma once
#include <BWAPI.h>
#include "Accountant.h"
#include "Archivist.h"
#include "Vassal.h"
#include "Landlord.h"
#include "recruiter.h"
#include "Architect.h"
#include "ArmyManager.h"

const int
	FACTORY_DEFAULT = 2, // The default desired factories.
	FACTORY_BASE = 2, // The desired factories per base.
	FACTORY_MAX = 8; // The absolute max factories.
const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;


// Orders more units, upgrades and structures as needed for macro war strategies.
class Strategist
{
private:
	Accountant	* accountant;
	Archivist	* archivist;
	Landlord	* landlord;
	Recruiter	* recruiter;
	CombatJudge * combatJudge;
	Architect	* architect;
	ArmyManager	* armyManager;

public:
	Strategist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, CombatJudge * combatJudge, Architect * architect, ArmyManager * armyManager);
	~Strategist();

	void update();

	bool isDefended();
};