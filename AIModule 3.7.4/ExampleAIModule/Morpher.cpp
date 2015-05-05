#include "Morpher.h"


// Constructor.
Morpher::Morpher(Accountant * accountant, Gatherer * gatherer) :
	accountant(accountant),
	gatherer(gatherer),
	morphs()
{
}


// Deconstructor.
Morpher::~Morpher()
{
}


// Monitors the morphing and designates competed units.
void Morpher::update()
{
	// Iterate through morphs.
	utilUnit::UnitSet::iterator it = morphs.begin(), end = morphs.end();
	while (it != end)
	{
		// Verify morph.
		BWAPI::Unit * morph = *it;
		if (morph->isCompleted() || !morph->exists())
		{
			// Remove morph.
			BWAPI::UnitType morphType = morph->getType();
			it = morphs.erase(it);
			accountant->removeSchedule(morphType);

			// Check alive.
			if (morph->exists())
			{
				// Designate morph.
				if (morphType.isRefinery())
					gatherer->addRefinery(morph);
			}
		}
		else
			it++;
	}
}


// Adds a morph to monitoring.
void Morpher::addMorph(BWAPI::Unit * morph)
{
	// Verify morph.
	if (utilUnit::isOwned(morph))
	{
		// Add morph.
		morphs.insert(morph);
		accountant->addSchedule(morph->getType());
	}
}


// Removes a morph from monitoring.
void Morpher::removeMorph(BWAPI::Unit * morph)
{
	// Verify morph.
	if (contains(morph))
	{
		// Remove morph.
		morphs.erase(morph);
		accountant->removeSchedule(morph->getType());
	}
}


// Returns whether the unit is monitored.
bool Morpher::contains(BWAPI::Unit * unit)
{
	return morphs.count(unit) > 0;
}