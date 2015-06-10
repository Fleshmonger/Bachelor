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
#include "Settler.h"
#include "Strategist.h"
#include "Planner.h"


const unsigned int
		MIN_SUPPLY = 10,
		MAX_SUPPLY = 400,
		MIN_MINERAL_SATURATION = 2,
		MAX_MINERAL_SATURATION = 3,
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
	Settler		* settler;
	Strategist	* strategist;
	Planner		* planner;

	utilUnit::UnitList refineries;

public:
	Economist(Accountant * accountant, Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Settler * settler, Strategist * strategist, Planner * planner);
	~Economist();

	void update();
	void addRefinery(BWAPI::Unit * refinery);
	void maynardSlide(BWTA::Region * target);
};

