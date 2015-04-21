#include "Attacker.h"


// Constructor
Attacker::Attacker(Archivist * archivist, Landlord * landlord, CombatJudge * combatJudge, ArmyManager * armyManager) :
	archivist(archivist),
	landlord(landlord),
	combatJudge(combatJudge),
	armyManager(armyManager),
	target()
{
}


// Destructor
Attacker::~Attacker()
{
}


// Commands units to attack.
// TODO Cleanup
void Attacker::update()
{
	// Aquire units.
	utilUnit::UnitSet
		fighters = armyManager->getEnlisted(DUTY_ATTACK_FIGHT),
		transit = armyManager->getEnlisted(DUTY_ATTACK_TRANSIT),
		idle = armyManager->getEnlisted(DUTY_IDLE),
		ready;

	// Aquire target.
	BWAPI::Position targetPosition = archivist->getPosition(target);
	if (!targetPosition)
	{
		BOOST_FOREACH(BWAPI::Unit * building, archivist->getBuildings())
		{
			BWAPI::Position buildingPosition = archivist->getPosition(building);
			if (buildingPosition)
			{
				targetPosition = buildingPosition;
				break;
			}
		}
	}

	// Enlist idle units as attackers.
	armyManager->assignUnits(idle, DUTY_ATTACK_TRANSIT);

	// Move travellers in combat.
	utilUnit::UnitSet::iterator transitIt = transit.begin(), transitEnd = transit.end();
	while (transitIt != transitEnd)
	{
		// Verify traveller.
		BWAPI::Unit * traveller = *transitIt;
		if (enemyDetected(traveller))
		{
			// Move traveller to ready.
			ready.insert(traveller);
			transitIt = transit.erase(transitIt);
			transitEnd = transit.end();
		}
		else
			transitIt++;
	}

	// Move fighters outside combat.
	utilUnit::UnitSet::iterator fightersIt = fighters.begin(), fightersEnd = fighters.end();
	while (fightersIt != fightersEnd)
	{
		// Verify fighter.
		BWAPI::Unit * fighter = *fightersIt;
		if (!enemyDetected(fighter))
		{
			// Move fighter to transit.
			transit.insert(fighter);
			armyManager->assignUnit(fighter, DUTY_ATTACK_TRANSIT);
			fightersIt = fighters.erase(fightersIt);
			fightersEnd = fighters.end();
		}
		else
			fightersIt++;
	}

	// Strength check.
	if (combatJudge->strength(fighters) + combatJudge->strength(ready) > combatJudge->strength(archivist->getTroops()) + combatJudge->strength(archivist->getTurrets()))
	{
		// Move ready to fighters.
		fighters.insert(ready.begin(), ready.end());
		armyManager->assignUnits(ready, DUTY_ATTACK_FIGHT);
	}
	else
	{
		// Aquire depot.
		BWAPI::Unit * depot = landlord->getHeadquarters()->getDepot();

		// Command ready.
		if (depot &&
			depot->exists())
		{
			// Retreat.
			BOOST_FOREACH(BWAPI::Unit * unit, ready)
				utilUnit::command(unit, BWAPI::UnitCommandTypes::Move, depot->getPosition());
		}
	}

	// Verify target position.
	if (targetPosition)
	{
		// Command transit.
		BOOST_FOREACH(BWAPI::Unit * traveller, transit)
			utilUnit::command(traveller, BWAPI::UnitCommandTypes::Move, targetPosition);

		// Command fighters.
		BOOST_FOREACH(BWAPI::Unit * fighter, fighters)
			utilUnit::command(fighter, BWAPI::UnitCommandTypes::Attack_Move, targetPosition);
	}
}


// Returns whether the given unit is within danger.
//TODO Remove code duplication.
bool Attacker::enemyDetected(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists())
	{
		// Check enemy detection.
		BOOST_FOREACH(BWAPI::Unit * enemy, archivist->getEnemies())
		{
			BWAPI::Position position = archivist->getPosition(enemy);
			if (position &&
				unit->getDistance(position) < DETECTION_DISTANCE)
				return true;
		}
	}
	return false;
}