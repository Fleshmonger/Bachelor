#pragma once
#include <BWAPI.h>
#include "Landlord.h"
#include "Recruiter.h"
#include "Gatherer.h"
#include "Architect.h"
#include "Strategist.h"
#include "Planner.h"
#include "Economist.h"


// Manages build-orders and auto-pilot managers.
//TODO Make economist and strategist local.
class Despot
{
private:
	Landlord	* landlord;
	Recruiter	* recruiter;
	Gatherer	* gatherer;
	Architect	* architect;
	Strategist	* strategist;
	Planner		* planner;
	Economist	* economist;

public:
	Despot(Landlord * landlord, Recruiter * recruiter, Gatherer * gatherer, Architect * architect, Strategist * strategist, Planner * planner, Economist * economist);
	~Despot();

	void update();
};

