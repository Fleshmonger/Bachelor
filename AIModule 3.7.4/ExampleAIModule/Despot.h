#pragma once
#include <BWAPI.h>
#include "UtilUnit.h"
#include "Accountant.h"
#include "Archivist.h"
#include "Landlord.h"
#include "Recruiter.h"
#include "Gatherer.h"
#include "Architect.h"
#include "Strategist.h"
#include "Settler.h"
#include "Planner.h"
#include "Economist.h"


// Manages build-orders and auto-pilot managers.
//TODO Make economist and strategist local.
class Despot
{
private:
	Accountant	* accountant;
	Archivist	* archivist;
	Landlord	* landlord;
	Gatherer	* gatherer;
	Strategist	* strategist;
	Settler		* settler;
	Planner		* planner;
	Economist	* economist;

	bool expand;

public:
	Despot(Accountant * accountant, Archivist * archivist, Landlord * landlord, Gatherer * gatherer, Strategist * strategist, Settler * settler, Planner * planner, Economist * economist);
	~Despot();

	void update();

	bool isExpanding();
};

