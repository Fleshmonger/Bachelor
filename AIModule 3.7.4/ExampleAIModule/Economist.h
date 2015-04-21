#pragma once
#include <BWAPI.h>
#include "Vassal.h"
#include "Landlord.h"
#include "Producer.h"
#include "Architect.h"


const unsigned int
		MIN_SUPPLY = 10,
		MINERAL_SATURATION = 2;
const BWAPI::UnitType
		UNIT_DEPOT = BWAPI::UnitTypes::Protoss_Nexus,
		UNIT_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon,
		UNIT_WORKER = BWAPI::UnitTypes::Protoss_Probe;


// Commands and expands harvesting and controlled regions.
class Economist
{
private:
	Accountant	* accountant;
	Landlord	* landlord;
	Producer	* producer;
	Architect	* architect;

	Vassal * expansion;

public:
	Economist(Accountant * accountant, Landlord * landlord, Producer * producer, Architect * architect);
	~Economist();

	void update();
};

