#pragma once

#include <windows.h>
#include <d3dx9.h>

class GameObject
{
protected:
	float x;
	float y;

	// This should be a pointer to an object containing all graphic/sound/audio assets for rendering this object. 
	// For now, just a pointer to a single texture
	LPDIRECT3DTEXTURE9 texture;
public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	float GetX() { return x; }
	float GetY() { return y; }

	GameObject(float x = 0.0f, float y = 0.0f, LPDIRECT3DTEXTURE9 texture = NULL);

	virtual void Update(DWORD dt) { };
	virtual void Render();

	~GameObject();
};

typedef GameObject* LPGAMEOBJECT;