#include "CombatJudge.h"


// Constructor
CombatJudge::CombatJudge(Archivist * archivist) :
	archivist(archivist)
{
}


// Deconstructor
CombatJudge::~CombatJudge()
{
}


// Returns the interpreted strength of unit as a value.
// TODO Does not account for losing troops during combat.
// TODO Does not account for enemy upgrades.
// TODO Does not account for current enemy unit stats.
// TODO Does not account for armor.
// TODO Does not account for distance and range.
// TODO Does not account for maneuvering into attacking range.
// TODO Does not account for damage types.
// TODO Does not account for splash damage.
// TODO Does not account for abilities.
double CombatJudge::strength(BWAPI::Unit * unit)
{
	// Verify Unit.
	BWAPI::UnitType unitType = archivist->getType(unit);
	if (unitType)
	{
		BWAPI::WeaponType weaponType = unitType.groundWeapon();
		if (weaponType != BWAPI::WeaponTypes::None)
			return (double)(weaponType.damageAmount() * (unitType.maxHitPoints() + unitType.maxShields())) / weaponType.damageCooldown();
	}
	return 0;
}


// Returns the interpreted strength of an army as a value.
double CombatJudge::strength(utilUnit::UnitSet units)
{
	double unitsStrength = 0;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		unitsStrength += strength(unit);
	return unitsStrength;
}