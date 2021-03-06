#pragma once
#include <windows.h>
#include "Game.h"

constexpr float CAMERA_CEIL_Y = 130;
constexpr int SCREEN_HORIZONTAL_MAX_TILES = 17;
constexpr int SCREEN_VERTICAL_MAX_TILES = 13;
constexpr float MOVING_MAP_WIDTH = 2048;

class Camera
{
	static Camera* __instance;

	int width, height;
	D3DXVECTOR2 position;

	float viewLeft, viewTop, viewRight, viewBottom;

public:
	Camera();
	static Camera* GetInstance();

	void SetPosition(D3DXVECTOR2 position);
	void SetPosition(float x, float y);
	void SetViewSize(float left, float top, float right, float bottom);
	void SetViewSize(float width, float height);
	void GetViewSize(float& viewLeft, float& viewTop, float& viewRight, float& viewBottom);

	void Update();
	D3DXVECTOR2* GetPosition() { return &position; }
};

