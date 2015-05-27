#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Geologist.h"
#include "Landlord.h"
#include "Recruiter.h"
#include "Architect.h"
#include "Settler.h"


enum Base { BASE_ANY, BASE_NATURAL, BASE_MAIN };
typedef std::pair<BWAPI::UnitType, Base> Build;


class Planner
{
private:
	Geologist	* geologist;
	Landlord	* landlord;
	Recruiter	* recruiter;
	Architect	* architect;
	Settler		* settler;

	std::list<Build> buildOrder;

public:
	Planner(Geologist * geologist, Landlord * landlord, Recruiter * recruiter, Architect * architect, Settler * settler);
	~Planner();

	void update();
	void enqueue(BWAPI::UnitType unitType);
	void enqueue(BWAPI::UnitType unitType, Base base);

	bool contains(BWAPI::UnitType unitType);
	bool empty();
	bool produce(Build);

	std::list<Build> getBuildOrder();
};

