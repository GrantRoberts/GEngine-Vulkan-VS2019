#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Scene.h"
#include <optional>

#define uint uint32_t

struct QueueFamilyIndices
{
	bool IsComplete()
	{
		return m_GraphicsFamily.has_value();
	}

	std::optional<uint> m_GraphicsFamily;
};

 class Application
 {
	 public:
		Application();
		~Application();

		bool Startup();

		void Run();

		bool IsDeviceSuitable(VkPhysicalDevice device);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	private:
	GLFWwindow* m_Window;

	VkInstance m_VkInstance;

	VkSurfaceKHR m_VkSurface;

	VkInstanceCreateInfo m_VkCreateInfo;

	VkPhysicalDevice m_VkPhysicalDevice;

	VkDevice m_Device;

	bool m_ShuttingDown;

	Scene* m_GameScene;

	// For calculating delta time.
	double m_CurrentFrame = 0.0;
	double m_LastFrame = 0.0;
	double m_DeltaTime = 0.0;
 };