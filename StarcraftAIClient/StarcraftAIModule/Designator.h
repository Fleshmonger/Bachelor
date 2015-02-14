#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"
#include "ArmyManager.h"

using namespace BWAPI;

class Designator
{
private:
	Unitset * undesignated;
public:
	Designator();
	~Designator();
	void addUnit(BWAPI::Unit);
	void update();
};