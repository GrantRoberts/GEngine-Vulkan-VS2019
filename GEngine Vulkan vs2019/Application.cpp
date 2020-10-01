#include "Application.h"
#include <iostream>
#include <cstring>

	Application::Application()
	{
		m_VkInstance = VK_NULL_HANDLE;
		m_VkSurface = VK_NULL_HANDLE;
		m_VkPhysicalDevice = VK_NULL_HANDLE;
		m_Window = NULL;

		m_ShuttingDown = false;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwInit();

		uint count;
		const char** extensions = glfwGetRequiredInstanceExtensions(&count);

		m_GameScene = new Scene();

		// Make sure vulkan is supported before doing anything.
		if (glfwVulkanSupported())
		{
			memset(&m_VkCreateInfo, 0, sizeof(m_VkCreateInfo));
			m_VkCreateInfo.enabledExtensionCount = count;
			m_VkCreateInfo.ppEnabledExtensionNames = extensions;

			m_Window = glfwCreateWindow(1280, 720, "GEngine Vulkan", nullptr, nullptr);

			glfwMakeContextCurrent(m_Window);
		}
		else
		{
			std::cout << "Vulkan not supported! :(" << std::endl;
			m_ShuttingDown = true;
			system("pause");
		}
	}

	Application::~Application()
	{
		delete m_GameScene;
		m_GameScene = NULL;

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	bool Application::Startup()
	{
		if (vkCreateInstance(&m_VkCreateInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
		{
			std::cout << "Vulkan instance not successfully created" << std::endl;
			system("pause");
			return false;
		}
		else
		{
			uint deviceCount = 0;
			vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
			if (deviceCount == 0)
			{
				std::cout << "Failed to find Vulkan supported GPUs!" << std::endl;
				system("pause");
				return false;
			}
			else
			{
				std::vector<VkPhysicalDevice> devices(deviceCount);
				vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

				for (const auto& device : devices)
				{
					if (IsDeviceSuitable(device))
					{
						m_VkPhysicalDevice = device;
						std::cout << "Physical device found!" << std::endl;
						break;
					}
				}

				if (m_VkPhysicalDevice == VK_NULL_HANDLE) 
				{
					std::cout << "Failed to find Vulkan supported GPUs!" << std::endl;
					return false;
				}
			}

			return true;
		}
	}

	void Application::Run()
	{
		while (!m_ShuttingDown)
		{
			//std::cout << "Application is running!" << std::endl;

			m_CurrentFrame = glfwGetTime();
			m_DeltaTime = m_CurrentFrame - m_LastFrame;
			m_LastFrame = m_CurrentFrame;

			m_GameScene->Update((float)m_DeltaTime);
			m_GameScene->Draw();
		}

		return;
	}

	bool Application::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device);

		return indices.IsComplete();
	}

	QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			{
				indices.m_GraphicsFamily = i;
			}

			if (indices.IsComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}