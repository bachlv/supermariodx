#include "Tail.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Brick.h"
#include "Goomba.h"
#include "Plant.h"
#include "Debug.h"
#include "Koopa.h"

Tail::Tail()
{
	timer = 0;
	Disable();
}

void Tail::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (!isEnabled) return;
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TAIL_HEIGHT;
}

void Tail::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = CheckOverlap(coObjects->at(i));
		if (obj != NULL)
		{
			if (dynamic_cast<Brick*>(obj))
			{
				Brick* b = dynamic_cast<Brick*>(obj);
				b->Hit();
			}
			else if (dynamic_cast<Goomba*>(obj))
			{
				Goomba* g = dynamic_cast<Goomba*>(obj);
				g->SetDirection(nx);
				if (!g->Hit()) return;

				Hit* hit = new Hit(x + nx * TAIL_HIT_OFFSET_X, y - TAIL_HEIGHT);
				LPSCENE scene = Game::GetInstance()->GetCurrentScene();
				((ScenePlayer*)scene)->AddObject(hit);
			}
			else if (dynamic_cast<Plant*>(obj))
			{
				Plant* p = dynamic_cast<Plant*>(obj);
				if (!p->Hit()) return;

				Hit* hit = new Hit(x + nx * TAIL_HIT_OFFSET_X, y - TAIL_HEIGHT);
				LPSCENE scene = Game::GetInstance()->GetCurrentScene();
				((ScenePlayer*)scene)->AddObject(hit);
			}
			else if (dynamic_cast<Koopa*>(obj))
			{
				Koopa* k = dynamic_cast<Koopa*>(obj);
				k->SetDirection(nx);
				if (!k->Hit()) return;

				Hit* hit = new Hit(x + nx * TAIL_HIT_OFFSET_X, y - TAIL_HEIGHT);
				LPSCENE scene = Game::GetInstance()->GetCurrentScene();
				((ScenePlayer*)scene)->AddObject(hit);
			}
		}
	}

}

void Tail::SetPosition(float x, float y)
{
	x += nx > 0 ? TAIL_OFFSET_RIGHT : -TAIL_OFFSET_LEFT;
	y += TAIL_OFFSET_Y;
	GameObject::SetPosition(x, y);
}

void Tail::Render()
{
	RenderBoundingBox();
}

