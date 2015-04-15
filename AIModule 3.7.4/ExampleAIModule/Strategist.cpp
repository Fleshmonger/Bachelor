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
	/*
	// Train new troops.
	producer->train(INFANTRY_UNIT);
	if (architect->scheduled(INFANTRY_FACTORY) + producer->getFactories(INFANTRY_FACTORY).size() < MAX_FACTORIES)
		architect->scheduleBuild(INFANTRY_FACTORY);
		*/
}