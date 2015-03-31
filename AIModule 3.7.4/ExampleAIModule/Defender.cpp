#include "Defender.h"


// Constructor
Defender::Defender(Archivist * archivist) :
	archivist(archivist)
{
}


// Deconstructor
Defender::~Defender()
{
}


// Commands the given units to defend.
void Defender::commandDefense(utilUnit::UnitSet guards, utilUnit::UnitSet invaders)
{
	// Command guards.
	if (!invaders.empty())
	{
		// Aquire target.
		BWAPI::Unit * target = *invaders.begin();
		// Command interception.
		BOOST_FOREACH(BWAPI::Unit * guard, guards)
		{
			// TODO Use utilunits command.
			if (guard->isIdle())
				guard->attack(archivist->getPosition(target));
		}
	}
	else
	{
		//TODO Command patrol.
	}
}