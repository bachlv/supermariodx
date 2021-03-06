#pragma once

#include <windows.h>
#include <vector>

#include "AnimationSets.h"
#include "Collision.h"
#include "Definition.h"

constexpr int OBJECT_TYPE_MARIO = 0;
constexpr int OBJECT_TYPE_BRICK = 1;
constexpr int OBJECT_TYPE_GOOMBA = 2;
constexpr int OBJECT_TYPE_KOOPA = 3;
constexpr int OBJECT_TYPE_PLATFORM = 4;
constexpr int OBJECT_TYPE_BLOCK = 5;
constexpr int OBJECT_TYPE_PLANT= 6;
constexpr int OBJECT_TYPE_COIN = 7;
constexpr int OBJECT_TYPE_ROULETTE = 8;
constexpr int OBJECT_TYPE_FLOATING_WOOD = 9;
constexpr int OBJECT_TYPE_PORTAL = 50;

constexpr float PUSH_BACK = 0.4f;
constexpr float GLOBAL_GRAVITY = 0.001f;
constexpr float GLOBAL_TERMINAL_VELOCITY = .25f;

enum RenderPriority
{
	PRIORITY_LOW = 0,
	PRIORITY_HIGH = 1,
};

class GameObject
{
protected:
	float x;
	float y;

	float dx{};	// dx = vx*dt
	float dy{};	// dy = vy*dt

	float vx;
	float vy;

	float ax{};
	float ay{};

	int nx{};

	int state{};

	bool isEnabled;

	ULONGLONG dt{};
	LPANIMATION_SET animation_set{};

	bool priority;

public:
	GameObject();
	virtual void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	virtual void SetState(int state) { this->state = state; }
	void SetDirection(int nx) { this->nx = nx; }

	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }
	int GetState() { return this->state; }

	void RenderBoundingBox();

	void SetAnimationSet(LPANIMATION_SET ani_set) { animation_set = ani_set; }

	LPCOLLISIONEVENT SweptAABBEx(LPGAMEOBJECT coO);
	LPGAMEOBJECT CheckOverlap(LPGAMEOBJECT coO);
	void CalculatePotentialCollisions(std::vector<LPGAMEOBJECT>* coObjects, std::vector<LPCOLLISIONEVENT>& coEvents);
	void FilterCollision(
		std::vector<LPCOLLISIONEVENT>& coEvents,
		std::vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny,
		float& rdx,
		float& rdy);


	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
	virtual void Render() = 0;

	bool IsEnabled() { return isEnabled; }
	void Disable() { isEnabled = false;  }
	void Enable() { isEnabled = true; }
	bool IsPrioritized() { return priority; }
	~GameObject();
};

