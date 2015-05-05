#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"
#include "Vassal.h"
#include "Landlord.h"
#include "Recruiter.h"
#include "Gatherer.h"
#include "Architect.h"


const unsigned int
		MIN_SUPPLY = 10,
		MINERAL_SATURATION = 2,
		REFINERY_SATURATION = 3;
const BWAPI::UnitType
		UNIT_DEPOT = BWAPI::UnitTypes::Protoss_Nexus,
		UNIT_REFINERY = BWAPI::UnitTypes::Protoss_Assimilator,
		UNIT_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon,
		UNIT_WORKER = BWAPI::UnitTypes::Protoss_Probe;


// Commands and expands harvesting and controlled regions.
class Economist
{
private:
	Accountant	* accountant;
	Landlord	* landlord;
	Recruiter	* recruiter;
	Gatherer	* gatherer;
	Architect	* architect;

	utilUnit::UnitList refineries;

public:
	Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect);
	~Economist();

	void update();
	void addRefinery(BWAPI::Unit * refinery);

	BWTA::Region * nextExpansion();
};

