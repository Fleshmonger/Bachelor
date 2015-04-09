#include "Attacker.h"


// Constructor
Attacker::Attacker(Archivist * archivist, CombatJudge * combatJudge, ArmyManager * armyManager) :
	archivist(archivist),
	combatJudge(combatJudge),
	armyManager(armyManager),
	depot(NULL),
	target(NULL)
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

	// Aquire target.
	BWAPI::Position targetPosition = archivist->getPosition(target);
	if (!targetPosition)
	{
		utilUnit::UnitSet enemyBuildings = archivist->getBuildings();
		utilUnit::UnitSet::iterator enemBuildIt = enemyBuildings.begin();
		while (enemBuildIt != enemyBuildings.end())
		{
			BWAPI::Unit * enemyBuilding = *enemBuildIt;
			BWAPI::Position enemyPosition = archivist->getPosition(enemyBuilding);
			if (enemyPosition)
			{
				targetPosition = enemyPosition;
				enemBuildIt = enemyBuildings.end();
			}
			else
				enemBuildIt++;
		}
	}

	// Aquire attackers.
	utilUnit::UnitSet
		fighters = armyManager->getEnlisted(ATTACK_FIGHT),
		transit = armyManager->getEnlisted(ATTACK_TRANSIT),
		ready;

	// Calculate strength.
	double strength = combatJudge->strength(fighters);

	// Command transit.
	BOOST_FOREACH(BWAPI::Unit * unit, transit)
	{
		// Verify unit.
		if (unit &&
			unit->exists())
		{
			// Detection check.
			if (enemyDetected(unit))
			{
				// Move unit to ready.
				ready.insert(unit);
				strength += combatJudge->strength(unit);
			}
			else
			{
				// Verify target.
				if (targetPosition)
					unit->move(targetPosition);
			}
		}
	}

	// Check if attack.
	if (strength > combatJudge->strength(archivist->getTroops()) + combatJudge->strength(archivist->getTurrets()))
	{
		// Move ready to fighters.
		BOOST_FOREACH(BWAPI::Unit * unit, ready)
		{
			fighters.insert(unit);
			armyManager->assignUnit(unit, ATTACK_FIGHT);
		}
	}
	else
	{
		// Command ready.
		BOOST_FOREACH(BWAPI::Unit * unit, ready)
			unit->move(depot->getPosition());
	}

	// Command fighters.
	BOOST_FOREACH(BWAPI::Unit * unit, fighters)
	{
		// Verify unit.
		if (unit &&
			unit->exists())
		{
			if (unit->isIdle() || unit->getLastCommand().getType() != BWAPI::UnitCommandTypes::Attack_Move)
				unit->attack(targetPosition);
		}
	}
}


// Returns whether the given unit can see an enemy unit.
bool Attacker::enemyDetected(BWAPI::Unit * unit)
{
	BOOST_FOREACH(BWAPI::Unit * unit, archivist->getUnits())
	{
		BWAPI::Position pos = archivist->getPosition(unit);
		if (utilUnit::isEnemy(unit) && unit->getDistance(pos) < DETECTION_DISTANCE)
			return true;
	}
	return false;
}