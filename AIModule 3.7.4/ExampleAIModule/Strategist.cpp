#include "Strategist.h"


// Constructor
Strategist::Strategist(Landlord * landlord, Producer * producer, Architect * architect) :
	landlord(landlord),
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
	producer->scheduleTraining(INFANTRY_UNIT);
	if (architect->scheduled(INFANTRY_FACTORY) + producer->getFactories(INFANTRY_FACTORY).size() < MAX_FACTORIES)
		architect->scheduleBuild(INFANTRY_FACTORY, landlord->getHeadquarters()->getDepot()->getTilePosition());
}