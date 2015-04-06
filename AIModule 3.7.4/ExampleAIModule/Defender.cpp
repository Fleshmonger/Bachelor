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
void Defender::update()
{
	// Aquire defenders.
	utilUnit::UnitSet defenders = armyManager->getEnlisted(DEFEND);

	// Check invasion.
	utilUnit::UnitSet invaders = archivist->invaders();
	if (!invaders.empty())
	{
		// Aquire target.
		BWAPI::Unit * target = *invaders.begin();

		// Enlist units in homeregion
		BOOST_FOREACH(BWAPI::Unit * unit, armyManager->getArmy())
		{
			if (archivist->inRegion(unit, archivist->getHomeRegion()))
			{
				armyManager->assignUnit(unit, DEFEND);
				defenders.insert(unit);
			}
		}

		// Enlist militia if necessary.
		double defenseStrength = combatJudge->strength(defenders), invaderStrength = combatJudge->strength(invaders);
		while (defenseStrength < invaderStrength)
		{
			BWAPI::Unit * worker = workerManager->takeWorker();
			if (worker &&
				worker->exists())
			{
				defenseStrength += combatJudge->strength(worker);
				defenders.insert(worker);
			}
			else
				break;
		}

		// Command interception.
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
		{
			if (defender->isIdle())
				utilUnit::command(defender, BWAPI::UnitCommandTypes::Attack_Move, archivist->getPosition(target));
		}
	}
	else
	{
		// Return defenders.
		BOOST_FOREACH(BWAPI::Unit * defender, defenders)
		{
			if (defender->getType().isWorker())
				workerManager->addWorker(defender);
			else
				armyManager->assignUnit(defender, IDLE);
		}
	}
}