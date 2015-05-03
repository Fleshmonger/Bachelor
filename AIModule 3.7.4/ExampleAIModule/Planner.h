#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Geologist.h"
#include "Landlord.h"
#include "Recruiter.h"
#include "Settler.h"
#include "Architect.h"


class Planner
{
private:
	Geologist	* geologist;
	Landlord	* landlord;
	Recruiter	* recruiter;
	Settler		* settler;
	Architect	* architect;

	std::list<BWAPI::UnitType> buildOrder;

public:
	Planner(Geologist * geologist, Landlord * landlord, Recruiter * recruiter, Settler * settler, Architect * architect);
	~Planner();

	void update();
	void enqueue(BWAPI::UnitType unitType);

	bool empty();
	bool produce(BWAPI::UnitType unitType);

	std::list<BWAPI::UnitType> getBuildOrder();
};

