#pragma once

#include "GameObject.h"

constexpr int TAIL_HIT_OFFSET_X = 8;
constexpr int HIT_ANIMATION_DURATION = 200;
constexpr int HIT_ANIMSET_ID = 75;

class Hit : public GameObject
{
	LPANIMATION_SET ani;
	ULONGLONG timer;
	ULONGLONG frameStart, tickPerFrame;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

public:
	Hit(float x, float y);
	virtual void SetState(int state);

};