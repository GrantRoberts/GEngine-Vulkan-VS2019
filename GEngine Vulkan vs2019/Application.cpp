#include "Application.h"
#include <iostream>
#include <cstring>
#include <set>

#ifdef NDBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

	Application::Application()
	{
		CreateGLFWWindow();
		CreateInstance();
		PickPhysicalDevice();
		CreateLogicalDevice();
	}

	Application::~Application()
	{
		vkDestroyDevice(m_VkDevice, nullptr);

		delete m_GameScene;
		m_GameScene = NULL;

		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		vkDestroyInstance(m_VkInstance, nullptr);

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

	void Application::CreateGLFWWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(1280, 720, "GEngine Vulkan", nullptr, nullptr);
	}

	void Application::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
		{
			std::cout << "Failed to create instance!" << std::endl;
		}
	}

	void Application::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			std::cout << "Failed to find GPUs with Vulkan support!" << std::endl;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

		for (const auto& device : devices) 
		{
			if (IsDeviceSuitable(device)) 
			{
				m_VkPhysicalDevice = device;
				break;
			}
		}

		if (m_VkPhysicalDevice == VK_NULL_HANDLE) 
		{
			std::cout << "Failed to find a suitable GPU!" << std::endl;
		}
	}

	void Application::CreateSurface()
	{
		VkDisplaySurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;

		if (vkCreateDisplayPlaneSurfaceKHR(m_VkInstance, &createInfo, nullptr, &m_VkSurface) != VK_SUCCESS)
		{
			std::cout << "Failed to create display surface!" << std::endl;
		}
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

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, i, m_VkSurface, &presentSupport);

			if (presentSupport)
			{
				indices.m_PresentFamily = i;
			}

			if (indices.IsComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	void Application::CreateLogicalDevice()
	{
		QueueFamilyIndices indicies = FindQueueFamilies(m_VkPhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint> uniqueQueueFamilies = { indicies.m_GraphicsFamily.value(), indicies.m_PresentFamily.value() };

		float queuePriority = 1.0f;
		for (uint queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = (uint)m_VkValidationLayers.size();
			createInfo.ppEnabledLayerNames = m_VkValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
		{
			std::cout << "Logical device creation failed" << std::endl;
		}

		vkGetDeviceQueue(m_VkDevice, indicies.m_GraphicsFamily.value(), 0, &m_VkGraphicsQueue);
		vkGetDeviceQueue(m_VkDevice, indicies.m_PresentFamily.value(), 0, &m_VkGraphicsQueue);
	}