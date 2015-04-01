#include "Attacker.h"


// Constructor
Attacker::Attacker(Archivist * archivist, CombatJudge * combatJudge) :
	archivist(archivist),
	combatJudge(combatJudge)
{
}


// Destructor
Attacker::~Attacker()
{
}


// Commands the given units to attack the enemy.
void Attacker::commandAttack(utilUnit::UnitSet attackers)
{
	// Verify target.
	if (!target ||
		!target->exists())
	{
		// Aquire enemies.
		utilUnit::UnitSet enemyBuildings = archivist->getBuildings();
		// Verify enemies.
		if (!enemyBuildings.empty())
		{
			// Aquire target.
			target = *enemyBuildings.begin();
		}
	}

	// Aquire target position.
	BWAPI::Position targetPosition;
	if (target)
		targetPosition = archivist->getPosition(target);
	else
		targetPosition = BWAPI::Positions::None;

	/*
	BOOST_FOREACH(BWAPI::Unit * attacker, attackers)
		attacker->attack(targetPosition);
	*/

	// Identify new attackers.
	double strength = 0;
	BOOST_FOREACH(BWAPI::Unit * attacker, attackers)
	{
		if (fighters.count(attacker) > 0 || waiting.count(attacker) > 0)
			strength += combatJudge->strength(attacker);
		else if (transit.count(attacker) == 0)
			transit.insert(attacker);
	}

	// Verify and command transit.
	utilUnit::UnitSet::iterator transitIt = transit.begin();
	while (transitIt != transit.end())
	{
		BWAPI::Unit * transiter = *transitIt;
		// Verify transiter.
		if (transiter &&
			transiter->exists())
		{
			// Identify if enemy spotted.
			if (enemyVisible(transiter) || transiter->isUnderAttack())
			{
				// Move transiter to waiting.
				waiting.insert(transiter);
				strength += combatJudge->strength(transiter);
				transitIt = transit.erase(transitIt);
			}
			else
			{
				// Command transiter.
				if (targetPosition)
					transiter->move(targetPosition);
				transitIt++;
			}
		}
		else
			transitIt = transit.erase(transitIt);
	}

	// Check if attack.
	if (strength > combatJudge->strength(archivist->getTroops()) + combatJudge->strength(archivist->getTurrets()))
	{
		fighters.insert(waiting.begin(), waiting.end()); // Move waiters to fighting.
		waiting.clear();
	}
	else
	{
		// Command waiting.
		utilUnit::UnitSet::iterator waitingIt = waiting.begin();
		while (waitingIt != waiting.end())
		{
			BWAPI::Unit * waiter = *waitingIt;
			// Verify waiter.
			if (waiter &&
				waiter->exists())
			{
				// Command waiter.
				if (enemyVisible(waiter) && depot)
				{
					// Retreat.
					waiter->move(depot->getPosition());
				}
				else
				{
					waiter->stop();
				}
				waitingIt++;
			}
			else
				waitingIt = waiting.erase(waitingIt);

		}
	}

	// Command fighters.
	utilUnit::UnitSet::iterator fightersIt = fighters.begin();
	while (fightersIt != fighters.end())
	{
		// Verify fighter.
		BWAPI::Unit * fighter = *fightersIt;
		if (fighter &&
			fighter->exists())
		{
			if (fighter->isIdle() || fighter->getLastCommand().getType() == BWAPI::UnitCommandTypes::Move)
				fighter->attack(targetPosition);
			fightersIt++;
		}
		else
			fightersIt = fighters.erase(fightersIt);
	}
}


// Sets the position units will retreat to.
void Attacker::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists())
		this->depot = depot;
}


// Returns whether the given unit can see an enemy unit.
// TODO Rename.
bool Attacker::enemyVisible(BWAPI::Unit * unit)
{
	utilUnit::UnitSet visible = unit->getUnitsInRadius(unit->getType().sightRange());
	utilUnit::UnitSet::iterator it = visible.begin();
	while (it != visible.end())
	{
		BWAPI::Unit * visibleUnit = *it;
		if (visibleUnit &&
			visibleUnit->exists() &&
			utilUnit::isEnemy(visibleUnit))
			return true;
		else
			it++;
	}
	return false;
}