#pragma once
#include <BWAPI.h>
#include <BWTA.h>


namespace utilMap
{
	struct Zone {
		int left, top, right, bottom;

		Zone();
		Zone(int left, int top, int right, int bottom);

		void expand(int left, int top, int right, int bottom);
		void expand(BWAPI::TilePosition pos, BWAPI::UnitType unitType);

		bool contains(int x, int y);
		bool contains(BWAPI::TilePosition pos, BWAPI::UnitType unitType);
	};
}