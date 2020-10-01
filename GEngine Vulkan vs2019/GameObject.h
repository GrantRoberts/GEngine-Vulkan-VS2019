#pragma once
#include <vector>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update(float deltaTime);

	void Draw();

private:
	std::vector<GameObject*> m_ChildObjects;
};