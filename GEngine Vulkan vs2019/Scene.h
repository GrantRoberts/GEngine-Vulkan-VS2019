#pragma once
#include <vector>
#include "GameObject.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Update(float deltaTime);

	void Draw();

private:
	std::vector<GameObject*> m_GameObjects;
};