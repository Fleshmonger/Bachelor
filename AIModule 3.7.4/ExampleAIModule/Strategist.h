#pragma once
#include <BWAPI.h>
#include "Accountant.h"
#include "Archivist.h"
#include "Vassal.h"
#include "Landlord.h"
#include "recruiter.h"
#include "Architect.h"

const int MAX_FACTORIES = 7; // The desired amount of factories.
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
	Architect	* architect;

public:
	Strategist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Architect * architect);
	~Strategist();

	void update();
};