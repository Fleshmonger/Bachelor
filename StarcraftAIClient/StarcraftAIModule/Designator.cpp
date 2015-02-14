#include "Designator.h"

Designator::Designator()
{
	undesignated = new Unitset();
}

// Unused deconstructor
Designator::~Designator()
{
}

void Designator::addUnit(BWAPI::Unit unit)
{

}

void Designator::update()
{
	for (auto &u : *undesignated)
	{

	}
}