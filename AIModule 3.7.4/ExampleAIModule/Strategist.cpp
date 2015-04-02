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


// TEMP
void Strategist::addFactory()
{
	factories++;
}


// TEMP
void Strategist::removeFactory()
{
	factories--;
}


// Builds facilities and troops.
void Strategist::update()
{
	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	if (architect->scheduled(INFANTRY_FACTORY) + factories < MAX_FACTORIES)
		architect->scheduleBuild(INFANTRY_FACTORY);
}