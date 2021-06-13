#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>

class Game
{
	static Game* __instance;
	HWND hWnd;										// Window handle

	LPDIRECT3D9 d3d = NULL;							// Direct3D handle
	LPDIRECT3DDEVICE9 d3ddev = NULL;				// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;				// Sprite helper library to help us draw 2D image on the screen 

	int backBufferWidth = 0;
	int backBufferHeight = 0;

public:
	void Init(HWND hWnd);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddev; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	int GetBackBufferWidth() { return backBufferWidth; }
	int GetBackBufferHeight() { return backBufferHeight; }

	static Game* GetInstance();

	~Game();
};