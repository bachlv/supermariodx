#pragma once
#include "GameObject.h"

constexpr int CELL_HEIGHT = 136;
constexpr int CELL_WIDTH = 136;

class Grid;

class Unit
{
	friend class Grid;

	float x, y;

	Grid* grid;
	LPGAMEOBJECT obj;

	Unit* prev;
	Unit* next;

public:
	Unit(Grid* grid, LPGAMEOBJECT obj, float x, float y);
	Unit(Grid* grid, LPGAMEOBJECT obj, int gridRow, int gridCol);

	void Move(float x, float y);
	LPGAMEOBJECT GetObject() { return this->obj; }
};

class Grid
{
	int mapWidth;
	int mapHeight;

	int columns, rows;

	std::vector<std::vector<Unit*>> cells;
public:
	Grid(int gridCols, int gridRows);
	void Add(Unit* unit);
	void Add(Unit* unit, int gridRow, int gridCol);
	void Get(std::vector<Unit*>& units);
	void Move(Unit* unit, float x, float y);
	void Clear();
};

