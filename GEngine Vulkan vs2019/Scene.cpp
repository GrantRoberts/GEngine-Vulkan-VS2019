#include "Scene.h"

Scene::Scene()
{
	m_GameObjects = std::vector<GameObject*>();
}

Scene::~Scene()
{
}

void Scene::Update(float deltaTime)
{
	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		m_GameObjects[i]->Update(deltaTime);
	}
}

void Scene::Draw()
{
	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		m_GameObjects[i]->Draw();
	}
}