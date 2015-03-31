#include "Strategist.h"


// Constructor
Strategist::Strategist(Producer * producer, Architect * architect) :
	producer(producer),
	architect(architect)
{
}


// Deconstructor
Strategist::~Strategist()
{
}


// Builds facilities and troops.
void Strategist::update()
{
	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);
}