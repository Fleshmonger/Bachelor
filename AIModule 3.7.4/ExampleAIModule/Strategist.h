#pragma once
#include <BWAPI.h>
#include "Archivist.h"
#include "Vassal.h"
#include "Landlord.h"
#include "Producer.h"
#include "Architect.h"

const int MAX_FACTORIES = 4; // The desired amount of factories.
const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;


// Orders more units, upgrades and structures as needed for macro war strategies.
class Strategist
{
private:
	Archivist	* archivist;
	Landlord	* landlord;
	Producer	* producer;
	Architect	* architect;

public:
	Strategist(Landlord * landlord, Producer * producer, Architect * architect);
	~Strategist();

	void update();
};