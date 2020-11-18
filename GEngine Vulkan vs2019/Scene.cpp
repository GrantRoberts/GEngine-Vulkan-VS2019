#include "Scene.h"
#include <iostream>

Scene::Scene()
{
	m_GameObjects = std::vector<GameObject*>();
}

Scene::~Scene()
{
	for (auto gameObject : m_GameObjects)
	{
		delete gameObject;
		gameObject = nullptr;
	}
}

void Scene::DeleteSamophores(VulkanRenderer* renderer)
{
	VkDevice device = renderer->GetLogicalDevice();
	vkDestroySemaphore(device, m_VkImageAvaliableSemaphore, nullptr);
	vkDestroySemaphore(device, m_VkRenderFinishedSemaphore, nullptr);
}

void Scene::Update(float deltaTime)
{
	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		m_GameObjects[i]->Update(deltaTime);
	}
}

void Scene::Draw(VulkanRenderer* renderer)
{
	uint imageIndex;

	vkAcquireNextImageKHR(renderer->GetLogicalDevice(), renderer->GetSwapChain(), UINT64_MAX, m_VkImageAvaliableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_VkImageAvaliableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	std::vector<VkCommandBuffer> cbs = renderer->GetCommandBuffers();
	submitInfo.pCommandBuffers = &cbs[imageIndex];

	VkSemaphore signalSemaphores[] = { m_VkRenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(renderer->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { renderer->GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(renderer->GetPresentQueue(), &presentInfo);
}

void Scene::CreateSemaphores(VulkanRenderer* renderer)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkDevice device = renderer->GetLogicalDevice();

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_VkImageAvaliableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_VkRenderFinishedSemaphore) != VK_SUCCESS)
		throw std::runtime_error("failed to create semaphores!");
}