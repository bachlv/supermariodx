#include "Grid.h"
#include "Debug.h"
#include "Camera.h"

Unit::Unit(Grid* grid, LPGAMEOBJECT obj, float x, float y)
{
	this->grid = grid;
	this->obj = obj;
	this->x = x;
	this->y = y;

	this->prev = NULL;
	this->next = NULL;

	grid->Add(this);
}

Unit::Unit(Grid* grid, LPGAMEOBJECT obj, int gridRow, int gridCol)
{
	this->grid = grid;
	this->obj = obj;
	obj->GetPosition(x, y);

	this->prev = NULL;
	this->next = NULL;

	grid->Add(this, gridRow, gridCol);
}

void Unit::Move(float x, float y)
{
	grid->Move(this, x, y);
}

void Grid::Add(Unit* unit)
{
	int col = (int)(unit->x / CELL_WIDTH);
	int row = (int)(unit->y / CELL_HEIGHT);

	if (col == this->columns) col = this->columns - 1;
	if (row == this->rows) row = this->rows - 1;

	unit->prev = NULL;
	unit->next = cells[row][col];
	cells[row][col] = unit;

	if (unit->next != NULL) unit->next->prev = unit;
}

void Grid::Add(Unit* unit, int gridRow, int gridCol)
{
	if (gridRow == this->rows) gridRow = this->rows - 1;
	if (gridCol == this->columns) gridCol = this->columns - 1;

	unit->prev = NULL;
	unit->next = cells[gridRow][gridCol];
	cells[gridRow][gridCol] = unit;

	if (unit->next != NULL) unit->next->prev = unit;
}


void Grid::Get(std::vector<Unit*>& units)
{
	float cameraX = Camera::GetInstance()->GetPosition()->x;
	float cameraY = Camera::GetInstance()->GetPosition()->y;

	int startCol = (int)(cameraX / CELL_WIDTH);
	int endCol = (int)ceil((cameraX + SCREEN_WIDTH) / CELL_WIDTH);
	int finalCol = (int)ceil((mapWidth) / CELL_WIDTH);

	if (endCol > finalCol) endCol = finalCol;

	for (int i = 0; i < rows; i++)
	{
		for (int j = startCol; j < endCol; j++)
		{
			Unit* unit = cells[i][j];
			while (unit != NULL)
			{
				units.push_back(unit);
				unit = unit->next;
			}
		}
	}
}


Grid::Grid(int gridCols, int gridRows)
{
	columns = gridCols;
	rows = gridRows;

	mapWidth = gridCols * CELL_WIDTH;
	mapHeight = gridRows * CELL_HEIGHT;


	cells.resize(columns);

	for (int i = 0; i < columns; i++)
		cells[i].resize(columns);

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
			cells[i][j] = NULL;
}

void Grid::Move(Unit* unit, float x, float y)
{
	int oldCol = (int)(unit->x / CELL_WIDTH);
	int oldRow = (int)(unit->y / CELL_HEIGHT);

	int newCol = (int)(x / CELL_WIDTH);
	int newRow = (int)(y / CELL_HEIGHT);

	if (newCol < 0 || newCol >= columns || newRow < 0 || newRow >= rows)
		return;

	unit->x = x;
	unit->y = y;

	if (oldRow == newRow && oldCol == newCol) return;

	if (unit->prev != NULL) unit->prev->next = unit->next;
	if (unit->next != NULL) unit->next->prev = unit->prev;

	if (cells[oldRow][oldCol] == unit) cells[oldRow][oldCol] = unit->next;

	Add(unit);
}

void Grid::Clear()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			Unit* unit = cells[i][j];

			while (unit != NULL)
			{
				if (cells[i][j] == unit) cells[i][j] = unit->next;
				if (unit->next != NULL) unit->next->prev = unit->prev;
				if (unit->prev != NULL) unit->prev->next = unit->next;

				Unit* temp = unit;
				unit = unit->next;
				delete temp->GetObject();
				delete temp;
			}
		}
	}
}