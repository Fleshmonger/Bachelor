#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Geologist.h"
#include "Landlord.h"
#include "Recruiter.h"
#include "Settler.h"
#include "Architect.h"


enum Base { BASE_ANY, BASE_NATURAL, BASE_MAIN };
typedef std::pair<BWAPI::UnitType, Base> Build;


class Planner
{
private:
	Geologist	* geologist;
	Landlord	* landlord;
	Recruiter	* recruiter;
	Settler		* settler;
	Architect	* architect;

	std::list<Build> buildOrder;

public:
	Planner(Geologist * geologist, Landlord * landlord, Recruiter * recruiter, Settler * settler, Architect * architect);
	~Planner();

	void update();
	void enqueue(BWAPI::UnitType unitType);
	void enqueue(BWAPI::UnitType unitType, Base base);

	bool empty();
	bool produce(Build);

	std::list<Build> getBuildOrder();
};

