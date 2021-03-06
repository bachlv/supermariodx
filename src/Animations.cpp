#include "Game.h"
#include "Animations.h"
#include "Debug.h"

void Animation::Add(int spriteId, ULONGLONG time)
{
	ULONGLONG t = time;

	LPSPRITE sprite = Sprites::GetInstance()->Get(spriteId);
	LPANIMATION_FRAME frame = new Frame(sprite, t);
	frames.push_back(frame);
}

void Animation::Render(
	int nx, float x, float y,
	int alpha, D3DXVECTOR2 translation,
	int left, int top,
	int right, int bottom)
{
	ULONGLONG now = GetTickCount64();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	else
	 {
		frames[currentFrame]->GetSprite()->
			Draw(nx, x, y, alpha, translation, 
				left, top, right, bottom);
	}
}

void Animation::Render(
	int nx, float x, float y,
	int alpha, D3DXVECTOR2 translation, int rotation,
	int clippingWidth, int clippingHeight)
{
	ULONGLONG now = GetTickCount64();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	else if (frames[currentFrame]->GetTime() == 0) {
		if (clippingWidth == NULL && clippingHeight == NULL)
			frames[currentFrame]->GetSprite()->Draw(nx, x, y, alpha, translation, rotation);
		else
			frames[currentFrame]->GetSprite()->Draw(nx, x, y, alpha, translation, clippingWidth, clippingHeight);
		return;
	}
	else
	{
		ULONGLONG t = frames[currentFrame]->GetTime();
		if (now - lastFrameTime > t)
		{
			currentFrame++;
			lastFrameTime = now;
			if (currentFrame == frames.size()) currentFrame = 0;
			//DebugOut(L"now: %d, lastFrameTime: %d, t: %d\n", now, lastFrameTime, t);
		}

	}

	if (clippingWidth == NULL && clippingHeight == NULL)
		frames[currentFrame]->GetSprite()->Draw(nx, x, y, alpha, translation, rotation);
	else
		frames[currentFrame]->GetSprite()->Draw(nx, x, y, alpha, translation, clippingWidth, clippingHeight);
}

void Animation::RenderFirstFrame(float x, float y, int rotation)
{
	ULONGLONG now = GetTickCount64();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	frames[currentFrame]->GetSprite()->Draw(FLIP, x, y, VISIBLE, D3DXVECTOR2(0, 0), rotation);
}

void Animation::RenderFirstFrame(int nx, float x, float y, D3DXVECTOR2 translation)
{
	ULONGLONG now = GetTickCount64();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	frames[currentFrame]->GetSprite()->Draw(nx, x, y, VISIBLE, translation, NOROTATE);
}

void Animation::Render(int nx, float x, float y, int alpha, D3DXVECTOR2 translation)
{
	Render(nx, x, y, alpha, translation, NOROTATE);
}

void Animation::Render(int nx, float x, float y, int alpha)
{
	Render(nx, x, y, alpha, D3DXVECTOR2(0, 0));
}

void Animation::Render(float x, float y, int alpha)
{
	Render(NOFLIP, x, y, alpha, D3DXVECTOR2(0, 0));
}

void Animation::RenderFlipped(int nx, float x, float y, int alpha)
{
	Render(nx, x, y, alpha, D3DXVECTOR2(0, 0), ROTATE180);
}


void Animation::Render(int nx, float x, float y, int clippingWidth, int clippingHeight)
{
	Render(nx, x, y, VISIBLE, D3DXVECTOR2(0, 0), NOROTATE, clippingWidth, clippingHeight);
}

Animations* Animations::__instance = NULL;

Animations* Animations::GetInstance()
{
	if (__instance == NULL) __instance = new Animations();
	return __instance;
}

void Animations::Add(int id, LPANIMATION ani)
{
	animations[id] = ani;
}

LPANIMATION Animations::Get(int id)
{
	LPANIMATION ani = animations[id];
	if (ani == NULL)
		DebugOut(L"[ERROR] Failed to find animation id: %d\n", id);
	return ani;
}

void Animations::Clear()
{
	for (auto x : animations)
	{
		LPANIMATION ani = x.second;
		delete ani;
	}

	animations.clear();
}