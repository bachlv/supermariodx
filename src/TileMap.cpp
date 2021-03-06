#include "Game.h"
#include "TileMap.h"
#include "Camera.h"
#include "Debug.h"
#include "Definition.h"

TileMap::TileMap(int textureId, int mapRows, int mapColumns, int tilesheetRows, int  tilesheetColumns, int totalTiles)
{
	tileTexture = Textures::GetInstance()->Get(textureId);
	this->mapRows = mapRows;
	this->mapColumns = mapColumns;
	this->tilesheetRows = tilesheetRows;
	this->tilesheetColumns = tilesheetColumns;
	this->totalTiles = totalTiles;
	mapWidth = 0;
	mapHeight = 0;
}

TileMap::~TileMap()
{
	delete tiles;
	tileTexture = NULL;
	tileSprites.clear();
	tiles = NULL;
}

void TileMap::Render()
{
	if (Game::GetInstance()->IsKeyDown(DIK_M)) return;
	int cameraLeft = (int)(Camera::GetInstance()->GetPosition()->x) / TILE_WIDTH;
	int cameraTop = (int)(Camera::GetInstance()->GetPosition()->y) / TILE_HEIGHT;
	int cameraRight = cameraLeft + SCREEN_HORIZONTAL_MAX_TILES;
	int cameraBottom = cameraTop + SCREEN_VERTICAL_MAX_TILES;
	
	for (int i = 0; i < mapRows; i++)
		for (int j = 0; j < mapColumns; j++)
		{
			// Only draw tiles visible in the viewport
			if (i >= cameraTop && j >= cameraLeft && i < cameraBottom && j < cameraRight)
			tileSprites[tiles[i][j] - 1]->Draw(j * (float)TILE_WIDTH, i * (float)TILE_HEIGHT);
		}
}

void TileMap::SetTileMapData(int** tiles)
{
	this->tiles = tiles;
}


void TileMap::SetTileSprites()
{
	for (int i = 0; i < totalTiles; i++)
	{
		int left = i % tilesheetColumns * TILE_WIDTH;
		int top = i / tilesheetColumns * TILE_HEIGHT;
		int right = left + TILE_WIDTH;
		int bottom = top + TILE_HEIGHT;
		LPSPRITE sprite = new Sprite(i, left, top, right, bottom, tileTexture);
		this->tileSprites.push_back(sprite);
	}
}

void TileMap::SetMapSize(int width, int height)
{
	mapWidth = width;
	mapHeight = height;
}

void TileMap::GetMapSize(int& width, int& height)
{
	width = mapWidth;
	height = mapHeight;
}