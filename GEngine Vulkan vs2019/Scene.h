#pragma once
#include "SwapChainSupportDetails.h"
#include <vector>
#include "GameObject.h"
#include "VulkanRenderer.h"

class Scene
{
public:
	Scene();
	~Scene();

	void CreateSemaphores(VulkanRenderer* renderer);
	void DeleteSamophores(VulkanRenderer* renderer);

	void Update(float deltaTime);

	void Draw(VulkanRenderer* renderer);

private:

	std::vector<GameObject*> m_GameObjects;

	VkSemaphore m_VkImageAvaliableSemaphore;

	VkSemaphore m_VkRenderFinishedSemaphore;
};