#pragma once
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR 
#include <GLFW/glfw3.h>
#include "Scene.h"
#include <optional>

#define uint uint32_t

struct QueueFamilyIndices
{
	bool IsComplete()
	{
		return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
	}

	std::optional<uint> m_GraphicsFamily;
	// (feelsbadman)
	std::optional<uint> m_PresentFamily;
};

 class Application
 {
	 public:
		 // Constructor.
		Application();
		// Destructor.
		~Application();

		// Start the application.
		// Returns: if the application successfully started up.
		bool Startup();

		// Run the application.
		void Run();

		void CreateGLFWWindow();

		void CreateInstance();

		void PickPhysicalDevice();

		void CreateSurface();

		// Is the device suitable for vulkan rendering?
		// Params: the device to check.
		// Returns: if the device is suitable for vulkan rendering.
		bool IsDeviceSuitable(VkPhysicalDevice device);

		// Find the vulkan families the device supports.
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();

	private:
	// The game window.
	GLFWwindow* m_Window;

	// The vulkan instance.
	VkInstance m_VkInstance;

	// The vulkan window surface.
	VkSurfaceKHR m_VkSurface;

	// The vulkan creation information.
	VkInstanceCreateInfo m_VkCreateInfo;

	// The device to use for rendering.
	VkPhysicalDevice m_VkPhysicalDevice;

	// Again, the device for rendering.
	VkDevice m_VkDevice;

	// The queue for drawing graphics.
	VkQueue m_VkGraphicsQueue;

	VkQueue m_VkPresentQueue;

	// Validation layers
	std::vector<const char*> m_VkValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	bool m_ShuttingDown;

	Scene* m_GameScene;

	// For calculating delta time.
	double m_CurrentFrame = 0.0;
	double m_LastFrame = 0.0;
	double m_DeltaTime = 0.0;
 };