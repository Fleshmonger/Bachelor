#include "Defender.h"


// Constructor
Defender::Defender(Archivist * archivist, WorkerManager * workerManager, CombatJudge * combatJudge, ArmyManager * armyManager) :
	archivist(archivist),
	workerManager(workerManager),
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
	// Aquire defenders.
	utilUnit::UnitSet defenders = armyManager->getEnlisted(DUTY_DEFEND);

	// Check invasion.
	utilUnit::UnitSet invaders = archivist->invaders();
	if (!invaders.empty())
	{
		// Aquire target.
		BWAPI::Unit * target = *invaders.begin();
		BWAPI::Position targetPos = archivist->getPosition(target);

		// Enlist units in homeregion
		BOOST_FOREACH(BWAPI::Unit * unit, armyManager->getArmy())
		{
			if (archivist->inRegion(unit, archivist->getHomeRegion()))
			{
				armyManager->assignUnit(unit, DUTY_DEFEND);
				defenders.insert(unit);
			}
		}

		// Enlist militia if necessary.
		double defenseStrength = combatJudge->strength(defenders) + combatJudge->strength(militia), invaderStrength = combatJudge->strength(invaders);
		utilUnit::UnitSet idle = workerManager->getEmployed(TASK_IDLE);
		utilUnit::UnitSet::iterator it = idle.begin(), end = idle.end();
		while (defenseStrength < invaderStrength && it != end)
		{
			//BWAPI::Unit * worker = workerManager->takeWorker();
			BWAPI::Unit * worker = *it;
			it++;
			if (worker &&
				worker->exists())
			{
				workerManager->employWorker(worker, TASK_DEFEND);
				defenseStrength += combatJudge->strength(worker);
				militia.insert(worker);
			}
		}

		// Command interception.
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
		{
			if (defender->isIdle())
				utilUnit::command(defender, BWAPI::UnitCommandTypes::Attack_Move, targetPos);
		}
		BOOST_FOREACH(BWAPI::Unit * worker, militia)
		{
			BWAPI::UnitCommand command = worker->getLastCommand();
			if (worker->getOrderTarget() && command.getType() != BWAPI::UnitCommandTypes::Attack_Unit)
				worker->attack(worker->getOrderTarget());
			else if (command.getType() != BWAPI::UnitCommandTypes::Attack_Move || command.getTargetPosition() != targetPos || worker->isIdle())
				worker->attack(targetPos);
		}
	}
	else
	{
		// Return defenders.
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
			armyManager->assignUnit(defender, DUTY_IDLE);
		BOOST_FOREACH(BWAPI::Unit * worker, militia)
			workerManager->addWorker(worker);
	}
}