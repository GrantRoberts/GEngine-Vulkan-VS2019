#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

void GameObject::Update(float deltaTime)
{
	for (int i = 0; i < m_ChildObjects.size(); ++i)
	{
		m_ChildObjects[i]->Update(deltaTime);
	}
}

void GameObject::Draw()
{
	for (int i = 0; i < m_ChildObjects.size(); ++i)
	{
		m_ChildObjects[i]->Draw();
	}
}