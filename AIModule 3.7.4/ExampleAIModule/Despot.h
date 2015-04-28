#pragma once
#include <BWAPI.h>
#include "Landlord.h"
#include "Recruiter.h"
#include "Gatherer.h"
#include "Architect.h"
#include "Planner.h"
#include "Economist.h"
#include "Strategist.h"


// Manages build-orders and auto-pilot managers.
//TODO Make economist and strategist local.
class Despot
{
private:
	Landlord	* landlord;
	Recruiter	* recruiter;
	Gatherer	* gatherer;
	Architect	* architect;
	Economist	* economist;
	Strategist	* strategist;

	Planner planner;

public:
	Despot(Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Economist * economist, Strategist * strategist);
	~Despot();

	void update();
};

