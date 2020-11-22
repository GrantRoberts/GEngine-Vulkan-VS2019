#pragma once
#include "SwapChainSupportDetails.h"
#include <vector>
#include "GameObject.h"
#include "VulkanRenderer.h"

class Scene
{
public:
	// Constructor.
	Scene();
	// Destructor.
	~Scene();

	// Create semaphores for drawing.
	void CreateSemaphores(VulkanRenderer* renderer);
	// Destroy the semaphores.
	void DeleteSamophores(VulkanRenderer* renderer);

	// Update the game scene.
	void Update(float deltaTime);

	// Draw the game scene.
	void Draw(VulkanRenderer* renderer);

private:
	// Vector of the game objects in the scene.
	std::vector<GameObject*> m_GameObjects;

	// Semaphore for if the image is avaliable.
	VkSemaphore m_VkImageAvaliableSemaphore;

	// Semaphore for if rendering is finished.
	VkSemaphore m_VkRenderFinishedSemaphore;
};