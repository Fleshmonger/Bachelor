#pragma once
#include <BWAPI.h>
#include "Producer.h"
#include "Architect.h"


const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;


// Orders more units, upgrades and structures as needed for macro war strategies.
class Strategist
{
private:
	Producer	* producer;
	Architect	* architect;

public:
	Strategist(Producer * producer, Architect * architect);
	~Strategist();

	void update();
};