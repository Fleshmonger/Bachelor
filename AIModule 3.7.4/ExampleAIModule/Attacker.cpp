#include "Attacker.h"


// Constructor
Attacker::Attacker(Archivist * archivist) :
	archivist(archivist)
{
}


// Destructor
Attacker::~Attacker()
{
}


// Commands the given units to attack the enemy.
void Attacker::commandAttack(utilUnit::UnitSet attackers)
{
	// Verify enemy location.
	utilUnit::UnitSet enemyBuildings = archivist->getBuildings();
	if (!enemyBuildings.empty())
	{
		// Command attackers.
		BWAPI::Position attackLocation = archivist->getPosition(*enemyBuildings.begin());
		utilUnit::UnitSet::iterator it = attackers.begin();
		while (it != attackers.end())
		{
			// Verify attacker.
			BWAPI::Unit * attacker = *it;
			++it;
			if (attacker &&
				attacker->exists())
			{
				// Command attacker.
				if (attacker->isIdle())
					attacker->attack(attackLocation);
			}
		}
	}
}