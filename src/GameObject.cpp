#pragma once

#include <d3dx9.h>
#include <vector>

#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

std::vector<LPANIMATION> GameObject::animations;

GameObject::GameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;
}

void GameObject::Update(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void GameObject::Render()
{
}

void GameObject::AddAnimation(int aniId)
{
	LPANIMATION ani = Animations::GetInstance()->Get(aniId);
	animations.push_back(ani);
}


GameObject::~GameObject()
{
}