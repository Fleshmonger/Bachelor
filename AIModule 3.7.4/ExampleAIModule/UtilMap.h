#pragma once
#include <BWAPI.h>
#include <BWTA.h>


namespace utilMap
{
	struct Zone {
		int left, top, right, bottom;

		Zone();
		Zone(int left, int top, int right, int bottom);
		bool contains(int tileX, int tileY);
		bool contains(BWAPI::TilePosition pos, BWAPI::UnitType unitType);
	};
}