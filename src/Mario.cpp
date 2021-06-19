#include "Mario.h"
#include "Debug.h"
#include "Definition.h"

void Mario::Update(DWORD dt)
{
	GameObject::Update(dt);

	// simple fall down
	vy += MARIO_GRAVITY;
	if (y > 100)
	{
		vy = 0; y = 100.0f;
	}

	// simple screen edge collision!!!
	if (vx > 0 && x > SCREEN_WIDTH - 30) x = SCREEN_WIDTH - 30;
	if (vx < 0 && x < 0) x = 0;
}

void Mario::Render()
{
	int ani;
	if (vx == 0)
	{
		if (nx > 0) ani = MARIO_ANI_IDLE_RIGHT;
		else ani = MARIO_ANI_IDLE_LEFT;
	}
	else if (vx > 0)
		ani = MARIO_ANI_WALKING_RIGHT;
	else ani = MARIO_ANI_WALKING_LEFT;

	animations[ani]->Render(x, y);
}

void Mario::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		vx = MARIO_WALKING_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		vx = -MARIO_WALKING_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (y == 100)
			vy = -MARIO_JUMP_SPEED_Y;

	case MARIO_STATE_IDLE:
		vx = 0;
		break;
	}
}
