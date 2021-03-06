#pragma once

#include <unordered_map>
#include "Sprites.h"

/*
	Sprite animation
*/
class Frame
{
	LPSPRITE sprite;
	ULONGLONG time;

public:
	Frame(LPSPRITE sprite, ULONGLONG time) { this->sprite = sprite; this->time = time; }
	ULONGLONG GetTime() { return time; }
	LPSPRITE GetSprite() { return sprite; }
};

typedef Frame* LPANIMATION_FRAME;

class Animation
{
	ULONGLONG lastFrameTime;
	int currentFrame;
	std::vector<LPANIMATION_FRAME> frames;
public:
	Animation() { lastFrameTime = -1; currentFrame = -1; }
	void Add(int spriteId, ULONGLONG time = 0);
	void Render(int nx, float x, float y, 
		int alpha, D3DXVECTOR2 translation, int rotation, 
		int clippingWidth = NULL, int clippingHeight = NULL);
	void Render(int nx, float x, float y, int alpha, D3DXVECTOR2 translation);
	void Render(int nx, float x, float y, int alpha = 255);
	void Render(float x, float y, int alpha = 255);
	void RenderFlipped(int nx, float x, float y, int alpha = 255);
	void Render(int nx, float x, float y, int clippingWidth, int clippingHeight);
	void Render(
		int nx, float x, float y,
		int alpha, D3DXVECTOR2 translation,
		int left, int top,
		int right, int bottom);
	void RenderFirstFrame(float x, float y, int rotation);
	void RenderFirstFrame(int nx, float x, float y, D3DXVECTOR2 translation);
};

typedef Animation* LPANIMATION;

class Animations
{
	static Animations* __instance;

	std::unordered_map<int, LPANIMATION> animations;

public:
	void Add(int id, LPANIMATION ani);
	LPANIMATION Get(int id);
	void Clear();

	static Animations* GetInstance();
};
