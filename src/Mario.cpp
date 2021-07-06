#include <vector>

#include "Mario.h"
#include "Goomba.h"
#include "Debug.h"
#include "Definition.h"
#include "Collision.h"
#include "Portal.h"
#include "Game.h"
#include "Types.h"

Mario::Mario(float x, float y) : GameObject()
{
	state = MARIO_SMALL;
	untouchable = 0;
	ax = 0; ay = 0;
	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;

	powerMeter = 0;
	powerTimer = 0;
	flyTimer = 0;

	lastTimeGainPower = 0;
	lastTimeDecreasePowerMaxHeight = 0;
	lastTimeDecreasePowerFlying = 0;
	lastTimeDecreasePowerIdle = 0;

}

void Mario::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	GameObject::Update(dt);
	Movement();

	ManagePowerDuration();


	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalculatePotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;

	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: Improve this function
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block 
		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.4f;

		// if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		// Stop when touching edge
		if (min_ty == 1 && !GetAction(JUMPING)) {
			ax = 0;
			edge[RIGHT] = true;
		}

		// Reset when touching the ground
		if (ny == -1) {
			UnsetAction(JUMPING);
			UnsetAction(PEAKING);
			UnsetAction(DESCENDING);
		}

		// DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f\n", min_tx, min_ty, nx, ny, rdx, rdy);

		//
		// Collision logic with other objects
		//
		for (size_t i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Goomba*>(e->obj)) // if e->obj is Goomba 
			{
				Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (state > MARIO_SMALL)
							{
								state = MARIO_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			} // if Goomba
			else if (dynamic_cast<Portal*>(e->obj))
			{
				Portal* p = dynamic_cast<Portal*>(e->obj);
				Game::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}
	// clean up collision events
	for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
	// DebugOut(L"x %f y %f vx %f vy %f dx %f dy %f\n", x, y, vx, vy, dx, dy);
}

void Mario::Render()
{
	int ani = NULL;
	if (state == MARIO_STATE_DIE)
		ani = DIE;
	else
		switch (state) {

		case MARIO_SMALL:
			if (AS_INT(vx) != 0 && AS_INT(ax) != 0)
			{
				if (!edge[RIGHT]) {
					ani = SMALL_WALKING;
					if (powerMeter > 2) ani = SMALL_RUNNING;
					if (powerMeter == MAX_POWER_METER) ani = SMALL_RUNNING_MAX;
				}
				else ani = SMALL_IDLE;
				if ((AS_INT(ax) ^ nx) < 0) ani = SMALL_BRAKING;
			}
			else ani = SMALL_IDLE;
			if (GetAction(JUMPING))
			{
				ani = SMALL_JUMPING;
				if (powerMeter == MAX_POWER_METER) ani = SMALL_JUMPING_MAX;
			}
			break;


		case MARIO_BIG:
			if (vx != 0 && ax != 0)
			{
				// Stop animating when hit an edge
				if (!edge[RIGHT])
				{
					ani = BIG_WALKING;
					if (powerMeter > 2) ani = BIG_RUNNING;
					if (powerMeter == MAX_POWER_METER) ani = BIG_RUNNING_MAX;
				}
				else ani = BIG_IDLE;

				// Efficently check if ax * nx < 0
				if ((AS_INT(ax) ^ nx) < 0) ani = BIG_BRAKING;
			}
			else ani = BIG_IDLE;
			if (GetAction(JUMPING)) {
				if (vy < 0) ani = BIG_JUMPING;
				else ani = BIG_JUMPED;
				if (powerMeter == MAX_POWER_METER) ani = BIG_JUMPING_MAX;
			}

			/*
			Optimize old code: if (!movement[LEFT] && !movement[RIGHT] && movement[DOWN]) ani = BIG_DUCKING;
			Movement is an array of 4 elements: L R U D; only care about L R and D
			So values will be: 0 0 0 1 or 0 0 1 1
			Using AS_INT treat these 4 boolean bytes and an 4 bytes of an integer
			Values to execute ani = BIG_DUCKING: 0x01000000 or 0x01010000 (little endian)
			These two value AND 0x01000101 always equal 0x01000000
			*/
			if ((AS_INT(movement) & 0x01000101) == 0x01000000) ani = BIG_DUCKING;
			break;


		case MARIO_RACOON:
			if (vx != 0 && ax != 0)
			{
				if (!edge[RIGHT])
				{
					ani = RACOON_WALKING;
					if (powerMeter > 2) ani = RACOON_RUNNING;
					if (powerMeter == MAX_POWER_METER) ani = RACOON_RUNNING_MAX;
				}
				else ani = RACOON_IDLE;
				if ((AS_INT(ax) ^ nx) < 0) ani = RACOON_BRAKING;
			}
			else ani = RACOON_IDLE;
			if (GetAction(JUMPING)) {
				if (vy < 0) ani = RACOON_JUMPING;
				// else ani = RACOON_JUMPED;
				if (powerMeter == MAX_POWER_METER) ani = RACOON_JUMPING_MAX;
			}
			if (GetAction(FLYING))
			{
				ani = RACOON_FLYING;
			}
			if (GetAction(DESCENDING) && vy > 0) ani = RACOON_DESCENDING;

			if ((AS_INT(movement) & 0x01000101) == 0x01000000) ani = RACOON_DUCKING;
			break;
		}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(nx, x, y, alpha);

	RenderBoundingBox();
}

void Mario::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	}
}

void Mario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	switch (state)
	{
	case MARIO_SMALL:
		right = x + SMALL_BBOX_WIDTH;
		bottom = y + SMALL_BBOX_HEIGHT;
		break;
	case MARIO_BIG:
		right = x + BIG_BBOX_WIDTH;
		bottom = y + BIG_BBOX_HEIGHT;
		break;
	case MARIO_RACOON:
		right = x + RACOON_BBOX_WIDTH;
		bottom = y + RACOON_BBOX_HEIGHT;
		break;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void Mario::Reset()
{
	SetState(MARIO_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

void Mario::Movement()
{
	// Velocity with friction + power meter
	vx = state == MARIO_SMALL ?
		(MARIO_WALKING_SPEED_SMALL + powerMeter * MARIO_POWER_ACCELERATION) * ax :
		(MARIO_WALKING_SPEED + powerMeter * MARIO_POWER_ACCELERATION) * ax;
	// Simple fall down
	vy += MARIO_GRAVITY * dt;


	if (GetMovement(LEFT))
	{
		nx = -1;
		if (ax > -1) ax -= MARIO_ACCELERATION_X;
		if (vx > 0) ax -= MARIO_ACCELERATION_X;

		edge[RIGHT] = false;
	}
	else if (GetMovement(RIGHT))
	{
		nx = 1;
		if (ax < 1) ax += MARIO_ACCELERATION_X;
		if (vx <= 0) ax += MARIO_ACCELERATION_X;

	}

	if (GetMovement(UP))
	{
		if (GetAction(SUPER_JUMPING)) {
			if (!GetAction(JUMPING))
			{
				last_y = y;
				vy = -MARIO_JUMP_SPEED_HIGH;
			}
			if (last_y - y < MARIO_JUMP_HEIGHT_MAX + powerMeter * MARIO_JUMP_HEIGHT_POWER) {
				vy -= MARIO_ACCELERATION_Y;
				// Gravity compensation
				vy -= MARIO_GRAVITY * dt;
			}
			else SetAction(DONE_JUMPING);
		}
		else
		{
			if (!GetAction(JUMPING)) {
				vy = -MARIO_JUMP_SPEED_LOW;
				if (!GetAction(FLYING)) vy -= powerMeter == MAX_POWER_METER ? MARIO_POWER_JUMP : 0;

				// Set flyTimer when start jumping
				flyTimer = 0;
			}

			if (state == MARIO_RACOON)
			{
				if (GetAction(FLYING) && powerMeter == MAX_POWER_METER)
				{
					// Move left/right slower 
					ax *= MARIO_ACCELERATION_X_PERCENTAGE;

					flyTimer += dt;
					if (flyTimer < POWER_MAX_FLY_TIME)
					{
						// Jump before taking off 
						// with a little jump speed easing
						if (vy < 0) vy += MARIO_SPEED_EASING;
						y -= MARIO_FLY_SPEED;
						vy -= MARIO_GRAVITY * dt;
					}
					else {
						UnsetAction(FLYING);
						SetAction(PEAKING);
					};
				}
				// Set descending action after jumping finished
				SetAction(DESCENDING);
			}
		}

		SetAction(JUMPING);
		edge[RIGHT] = false;

	}

	if (GetMovement(DOWN))
	{
		// Check if both movement[LEFT] and movement[LEFT] equal 0
		if (state != MARIO_SMALL && !AS_SHORT(movement)) {
			if (ax != 0) ax += (ax > 0) ? -MARIO_ACCELERATION_X : MARIO_ACCELERATION_X;
		}
	}

	if (state == MARIO_RACOON)
	{
		if (GetAction(DESCENDING))
		{
			// Set vy to a constant when falling down
			if (vy > 0) vy = MARIO_DESCENDING_SPEED;
		};
	}


	// Mario Inertia
	if (abs(ax) <= MARIO_INERTIA) ax = 0;
	// Disabled when moving left to prevent it slow down left movement
	if (ax < 0 && !GetMovement(LEFT))
	{
		ax += (state == MARIO_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA);
		ax -= powerMeter * MARIO_POWER_INERTIA;
	}
	if (ax > 0 && !GetMovement(RIGHT))
	{
		ax -= (state == MARIO_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA);
		ax += powerMeter * MARIO_POWER_INERTIA;
	}

	// Add inertia when changing direct suddenly (braking)
	if (!GetAction(FLYING) && abs(ax) <= 1)
	{
		if (nx * ax < 0)
		{
			if (ax < 0) ax -= powerMeter * MARIO_BRAKE_INERTIA;
			else ax += powerMeter * MARIO_BRAKE_INERTIA;

			// Stop Mario when releasing key after braking
			if (!AS_SHORT(movement))
			{
				if (ax < 0) ax += MARIO_BRAKE_IDLE_INERTIA;
				else ax -= MARIO_BRAKE_IDLE_INERTIA;
			}
		}

	}
}

void Mario::ManagePowerDuration()
{
	powerTimer += dt;

	if (!GetAction(FLYING))
	{
		// Power decrease slower
		if (!GetAction(PEAKING)) {
			if (powerTimer - lastTimeDecreasePowerIdle >= POWER_DOWN_DURATION_STEP)
			{
				if (powerMeter > 0 && (!GetAction(GAINING_POWER) || !AS_SHORT(movement))) {
					powerMeter--;
					lastTimeDecreasePowerIdle = powerTimer;
				}
			}
		}

		int duration;
		// If Mario is moving in an unchaged direction
		if (nx * ax > 0) duration = POWER_DIRECTION_UNCHANGED_STEP;
		else duration = POWER_DIRECTION_CHANGED_STEP;
		if (powerTimer - lastTimeGainPower >= duration)
		{
			// Only increase power when moving left or right
			if (powerMeter < MAX_POWER_METER && powerMeter >= 0 &&
				GetAction(GAINING_POWER) && AS_SHORT(movement))
			{
				if (nx * ax > 0) powerMeter++;
				else if (powerMeter > 0) powerMeter--;
				lastTimeGainPower = powerTimer;
			}
		}
	}

	// Decrease power quickly when maximum flying height reached
	if (powerTimer - lastTimeDecreasePowerMaxHeight >= POWER_PEAKED_DECREASE_TIME)
	{
		if (GetAction(PEAKING) && powerMeter > 0)
		{
			powerMeter--;
			lastTimeDecreasePowerMaxHeight = powerTimer;
		}
	}
}