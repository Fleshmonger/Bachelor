#include "Attacker.h"


// Constructor
Attacker::Attacker(Archivist * archivist, CombatJudge * combatJudge, ArmyManager * armyManager) :
	archivist(archivist),
	combatJudge(combatJudge),
	armyManager(armyManager),
	attackingStrength(0),
	target(NULL),
	depot(NULL)
{
}


// Destructor
Attacker::~Attacker()
{
}


// Sets the position units will retreat to.
void Attacker::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists())
		this->depot = depot;
}


// Commands units to attack.
// TODO Cleanup
void Attacker::update()
{
	// Enlist idle units as attackers.
	// TODO Is this safe?
	BOOST_FOREACH(BWAPI::Unit * unit, armyManager->getEnlisted(IDLE))
		armyManager->assignUnit(unit, ATTACK_TRANSIT);

	// Verify target.
	if (!target ||
		!target->exists())
	{
		// Aquire enemies.
		utilUnit::UnitSet enemyBuildings = archivist->getBuildings();

		// Verify enemies.
		if (!enemyBuildings.empty())
			target = *enemyBuildings.begin();
	}

	// Aquire target position.
	BWAPI::Position targetPosition = archivist->getPosition(target);

	// Aquire attackers.
	utilUnit::UnitSet
		fighters = armyManager->getEnlisted(ATTACK_FIGHT),
		transit = armyManager->getEnlisted(ATTACK_TRANSIT),
		waiting;
		//waiting = armyManager->getEnlisted(ATTACK_WAIT);

	// Calculate strength.
	double strength = combatJudge->strength(fighters) + combatJudge->strength(waiting);

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
			if (enemyDetected(transiter) || transiter->isUnderAttack())
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
		// Move waiters to fighting.
		BOOST_FOREACH(BWAPI::Unit * waiter, waiting)
		{
			fighters.insert(waiter);
			armyManager->assignUnit(waiter, ATTACK_FIGHT);
		}
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
				if (enemyDetected(waiter) && depot)
					waiter->move(depot->getPosition());
				else
					waiter->stop();
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


// Returns whether the given unit can see an enemy unit.
bool Attacker::enemyDetected(BWAPI::Unit * unit)
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