#include "Defender.h"


// Constructor
Defender::Defender(Archivist * archivist, Landlord * landlord, CombatJudge * combatJudge, ArmyManager * armyManager) :
	archivist(archivist),
	landlord(landlord),
	combatJudge(combatJudge),
	armyManager(armyManager)
{
}


// Deconstructor
Defender::~Defender()
{
}


// Commands defense.
//TODO Use utilUnit::command
void Defender::update()
{
	// Aquire units.
	std::map<BWTA::Region*, utilUnit::UnitSet>
		regionDefenders,
		regionInvaders;
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		regionDefenders[region] = utilUnit::UnitSet();
		regionInvaders[region] = utilUnit::UnitSet();
	}
	BOOST_FOREACH(BWAPI::Unit * enemy, archivist->getEnemies())
	{
		// Verify enemy.
		if (enemy &&
			!enemy->getType().isFlyer() &&
			!enemy->isCloaked() &&
			!enemy->isBurrowed())
		{
			// Verify region.
			BWTA::Region * region = BWTA::getRegion(archivist->getPosition(enemy));
			if (region)
				regionInvaders[region].insert(enemy);
		}
	}
	BOOST_FOREACH(BWAPI::Unit * defender, armyManager->getEnlisted(DUTY_DEFEND))
		regionDefenders[BWTA::getRegion(defender->getPosition())].insert(defender);

	// Iterate through regions.
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		// Aquire units.
		utilUnit::UnitSet
			militia = landlord->getEmployed(region, TASK_DEFEND),
			invaders = regionInvaders[region];

		// Verify region.
		Vassal * vassal = landlord->getVassal(region);
		if (vassal &&
			vassal->getDepot() &&
			!invaders.empty())
		{
			// Aquire target.
			BWAPI::Unit * target = *invaders.begin();
			BWAPI::Position targetPos = archivist->getPosition(target);

			// Enlist units in region.
			utilUnit::UnitSet defenders;
			BOOST_FOREACH(BWAPI::Unit * unit, armyManager->getArmy())
			{
				if (archivist->inRegion(unit, region))
				{
					armyManager->assignUnit(unit, DUTY_DEFEND);
					defenders.insert(unit);
				}
			}

			// Enlist militia if necessary.
			double
				defenseStrength = combatJudge->strength(defenders) + combatJudge->strength(militia),
				invaderStrength = combatJudge->strength(invaders);
			while (defenseStrength < invaderStrength)
			{
				// Verify worker.
				BWAPI::Unit * worker = landlord->getIdleWorker(region);
				if (worker &&
					worker->exists())
				{
					// Enlist worker.
					landlord->employWorker(worker, TASK_DEFEND);
					defenseStrength += combatJudge->strength(worker);
					militia.insert(worker);
				}
				else
					break;
			}

			// Command interception.
			BOOST_FOREACH(BWAPI::Unit * defender, defenders)
				if (defender->isIdle() || defender->getLastCommand().getType() != BWAPI::UnitCommandTypes::Attack_Move)
					utilUnit::command(defender, BWAPI::UnitCommandTypes::Attack_Move, targetPos);
			BOOST_FOREACH(BWAPI::Unit * worker, militia)
			{
				// Command militia.
				BWAPI::UnitCommand command = worker->getLastCommand();
				if (worker->getOrderTarget() && command.getType() == BWAPI::UnitCommandTypes::Attack_Move)
					worker->attack(worker->getOrderTarget());
				else if (command.getType() != BWAPI::UnitCommandTypes::Attack_Unit &&
					(command.getType() != BWAPI::UnitCommandTypes::Attack_Move || command.getTargetPosition() != targetPos || worker->isIdle()))
					worker->attack(targetPos);
			}
		}
		else
		{
			// Return defenders.
			BOOST_FOREACH(BWAPI::Unit * defender, regionDefenders[region])
				armyManager->assignUnit(defender, DUTY_IDLE);
			BOOST_FOREACH(BWAPI::Unit * worker, militia)
				landlord->dismissWorker(worker);
		}
	}

}