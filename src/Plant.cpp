#include <math.h> 

#include "Plant.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Platform.h"
#include "Projectile.h"
#include "Debug.h"

Plant::Plant(int type)
{
	timer = 0;
	this->type = type;
	SetState(PLANT_STATE_HIDING);
	if (type == PLANT_RED_SHOOTER) height = PLANT_RED_BBOX_HEIGHT;

}

void Plant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == PLANT_STATE_HIDING) return;
	left = x;
	top = y;
	right = x + PLANT_BBOX_WIDTH;
	bottom = y + height;
}

void Plant::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	y += dy;

	timer += dt;

	float marioX, marioY;
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);
	if (marioX < x) nx = -1;
	else nx = 1;
	if (marioY < y) ny = -1;
	else ny = 1;

	switch (type)
	{
	case PLANT_RED_SHOOTER:
	{
		if (state == PLANT_STATE_HIDING)
		{
			if (timer >= PLANT_RED_HIDING_INTERVAL)
			{
				SetState(PLANT_STATE_EXPANDING);
				timer = 0;
			}
		}
		if (state == PLANT_STATE_ATTACKING)
		{
			if (!isProjectileShooted)
			{
				if (timer >= PLANT_RED_PROJECTILE_DELAY)
				{
					int direction;
					// The principal value of the arc tangent of y/x, expressed in PI radian.
					float piRad = atan2f(marioY - y, marioX - x) / -D3DX_PI;

					// Find direction to shoot projectile
					if (piRad >= 0)
					{
						if (piRad >= 0 && piRad < PROJECTILE_22DEG)
							direction = UPPER_RIGHT;
						if (piRad >= PROJECTILE_22DEG && piRad < PROJECTILE_90DEG)
							direction = TOP_RIGHT;
						if (piRad >= PROJECTILE_90DEG && piRad < PROJECTILE_157DEG)
							direction = TOP_LEFT;
						if (piRad >= PROJECTILE_157DEG)
							direction = UPPER_LEFT;
					}
					else
					{
						if (piRad >= -PROJECTILE_22DEG && piRad < 0)
							direction = LOWER_RIGHT;
						if (piRad >= -PROJECTILE_90DEG && piRad < -PROJECTILE_22DEG)
							direction = BOTTOM_RIGHT;
						if (piRad >= -PROJECTILE_157DEG && piRad < -PROJECTILE_90DEG)
							direction = BOTTOM_LEFT;
						if (piRad < -PROJECTILE_157DEG)
							direction = LOWER_LEFT;
					}


					float projectileX = nx > 0 ?
						x + PLANT_PROJECTILE_OFFSET_XX :
						x - PLANT_PROJECTILE_OFFSET_XY;
					float projectileY = ny > 0 ? 
						y + PLANT_PROJECTILE_OFFSET_YX :
						y - PLANT_PROJECTILE_OFFSET_YY;
					Projectile* projectile = new Projectile(projectileX, projectileY, direction);
					LPSCENE scene = Game::GetInstance()->GetCurrentScene();
					((ScenePlayer*)scene)->AddObject(projectile);
					isProjectileShooted = true;
				}
			}

			if (timer >= PLANT_RED_ATTACKING_INTERVAL)
			{
				SetState(PLANT_STATE_COLLAPSING);
				timer = 0;
			}

		}
		break;
	}
	}

	if (y <= entryY && state == PLANT_STATE_EXPANDING)
	{
		vy = 0;
		y = entryY;
		SetState(PLANT_STATE_ATTACKING);
		timer = 0;
	}
	if (y >= entryY + height && state == PLANT_STATE_COLLAPSING)
	{
		vy = 0;
		SetState(PLANT_STATE_HIDING);
		timer = 0;
	}


}

void Plant::Render()
{
	int ani;
	switch (type)
	{
	case PLANT_RED_SHOOTER:
	{
		if (ny > 0) ani = PLANT_RED_ANI_BITING_DOWN;
		else ani = PLANT_RED_ANI_BITING_UP;

		if (state == PLANT_STATE_ATTACKING)
		{
			if (ny > 0)
				ani = PLANT_RED_ANI_SHOOTING_UP;
			else ani = PLANT_RED_ANI_SHOOTING_DOWN;
		}
		break;
	}
	default:
		ani = 0;
	}

	animation_set->at(ani)->Render(nx, x, y, PLANT_BBOX_WIDTH, (int)(entryY + height - y));
	//RenderBoundingBox();
}

void Plant::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case PLANT_STATE_EXPANDING:
		vy -= 0.045f;
		break;
	case PLANT_STATE_COLLAPSING:
		vy += 0.045f;
	case PLANT_STATE_ATTACKING:
		isProjectileShooted = false;
		break;
	}
}

void Plant::SetPosition(float x, float y)
{
	entryY = y;
	y += PLANT_RED_BBOX_HEIGHT;
	GameObject::SetPosition(x, y);
}