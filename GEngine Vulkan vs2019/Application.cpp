#include "Application.h"
#include <iostream>
#include <cstring>

	Application::Application()
	{
		glfwInit();
		
		uint32_t count;
		const char** extensions = glfwGetRequiredInstanceExtensions(&count);

		VkInstanceCreateInfo createInfo;

		memset(&createInfo, 0, sizeof(createInfo));
		createInfo.enabledExtensionCount = count;
		createInfo.ppEnabledExtensionNames = extensions;

		m_Window = glfwCreateWindow(1280,720, "GEngine Vulkan", nullptr, nullptr);

		VkResult error = glfwCreateWindowSurface(m_VkInstance, m_Window, NULL, &m_VkSurface);

		glfwMakeContextCurrent(m_Window);
	}

	Application::~Application()
	{
		glfwDestroyWindow(m_Window);
		//vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, m_VkCallbacks);
		glfwTerminate();
	}

	bool Application::Startup()
	{
		return true;
	}

	void Application::Run()
	{
		while (!m_ShuttingDown)
		{
			std::cout << "Application is running!" << std::endl;
		}

		return;
	}