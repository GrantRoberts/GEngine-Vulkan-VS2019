#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

 class Application
 {
	 public:
		Application();
		~Application();

		bool Startup();

		void Run();

	private:
	GLFWwindow* m_Window;

	VkInstance m_VkInstance;

	VkSurfaceKHR m_VkSurface;

	VkAllocationCallbacks* m_VkCallbacks;

	bool m_ShuttingDown;
 };