#pragma once


// Contains a map of reserved tiles.
class ReserveMap
{
private:
	int width,
		height;
	int ** map;

public:
	ReserveMap(int width, int height);
	~ReserveMap();

	void reserve(int x, int y);
	void free(int x, int y);

	bool contains(int x, int y);
	bool isReserved(int x, int y);
};

