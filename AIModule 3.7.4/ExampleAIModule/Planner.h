#pragma once
#include <BWAPI.h>
#include "Landlord.h"
#include "Recruiter.h"
#include "Architect.h"


class Planner
{
private:
	Landlord	* landlord;
	Recruiter	* recruiter;
	Architect	* architect;

	std::list<BWAPI::UnitType> buildOrder;

public:
	Planner(Landlord * landlord, Recruiter * recruiter, Architect * architect);
	~Planner();

	void update();
	void enqueue(BWAPI::UnitType unitType);
};

