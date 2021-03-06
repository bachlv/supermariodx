#pragma once

#include "GameObject.h"

constexpr float COIN_VELOCITY = -.35f;
constexpr float COIN_FALLING_FLOOR = 20;
constexpr int COIN_ANI_SET_ID = 70;

constexpr int COIN_BBOX_WIDTH = 14;
constexpr int COIN_BBOX_HEIGH = 16;

enum CoinType
{
	COIN_SMALL = 0,
	COIN_LARGE = 1,
	COIN_LARGE_STATIC = 2,
};

class Coin : public GameObject
{
	bool isThrowing;
	int type;
	float entryY{};
	
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);

public:
	Coin();
	Coin(float x, float y, int state);

	virtual void SetPosition(float x, float y);
	void Throw();
	void Earn();
};