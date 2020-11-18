#include "VulkanRenderer.h"
#include <iostream>
#include <cstring>
#include <set>
#include <algorithm>
#include <fstream>

#ifdef NDBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VulkanRenderer::VulkanRenderer(float width, float height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;

	m_VkValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	m_VkDeviceExtenstions  = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Initalise stuff needed for vulkan rendering.
	CreateGLFWWindow();
	CreateInstance();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
}

VulkanRenderer::~VulkanRenderer()
{
	// Delete all the vulkan stuff.
	vkDestroyPipeline(m_VkLogicalDevice, m_VkGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_VkLogicalDevice, m_VkPipelineLayout, nullptr);
	vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
	vkDestroyInstance(m_VkInstance, nullptr);
	vkDestroySwapchainKHR(m_VkLogicalDevice, m_VkSwapChain, nullptr);
	vkDestroyPipelineLayout(m_VkLogicalDevice, m_VkPipelineLayout, nullptr);
	vkDestroyRenderPass(m_VkLogicalDevice, m_VkRenderPass, nullptr);
	vkDestroyCommandPool(m_VkLogicalDevice, m_VkCommandPool, nullptr);

	for (auto imageView : m_VkSwapChainImageViews)
	{
		vkDestroyImageView(m_VkLogicalDevice, imageView, nullptr);
	}

	for (auto framebuffer : m_VkSwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_VkLogicalDevice, framebuffer, nullptr);
	}

	vkDestroyDevice(m_VkLogicalDevice, nullptr);

	// Destry glfw and terminate it.
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void VulkanRenderer::CreateGLFWWindow()
{
	// Initialise glfw.
	glfwInit();

	// Specifications for the glfw window.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create the glfw window.
	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "GEngine Vulkan", nullptr, nullptr);
}

void VulkanRenderer::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not avaliable!");

	// Create the vulkan instance.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "GEngine Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint>(m_VkValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_VkValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance!");
}

void VulkanRenderer::PickPhysicalDevice()
{
	// Find all the devices connected to the computer.
	uint deviceCount = 0;
	vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

	// If no devices connected support vulkan, output error message.
	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPU with vulkan support!");
	}
	
	// Get list of all the supported devices and find one to use for rendering.
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

	// If no device was found, output error message.
	if (m_VkPhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find GPU with vulkan support!");
		return;
	}
}

void VulkanRenderer::CreateSurface()
{
	// Create the vulkan surface.
	VkDisplaySurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;

	if (glfwCreateWindowSurface(m_VkInstance, m_Window, nullptr, &m_VkSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupport = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupport)
	{
		// Check swap chain support of physical device.
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		// Check if either swap chain formats or present modes are empty, if they are, swap chain isn't supported by GPU.
		swapChainAdequate = !swapChainSupport.m_Formats.empty() && !swapChainSupport.m_PresentModes.empty();
	}

	return indices.IsComplete() && extensionsSupport && swapChainAdequate;
}

bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> avaliableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avaliableExtensions.data());

	std::set<std::string> requiredExtensions(m_VkDeviceExtenstions.begin(), m_VkDeviceExtenstions.end());

	for (const auto& extension : avaliableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VulkanRenderer::CheckValidationLayerSupport()
{
	uint layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> avaliableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

	for (const char* layerName : m_VkValidationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : avaliableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> VulkanRenderer::GetRequiredExtensions()
{
	uint glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice device)
{
	// Get the queue families of the current checking device.
	QueueFamilyIndices indices;

	uint queueFamilyCount = 0;
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
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_VkSurface, &presentSupport);

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

void VulkanRenderer::CreateLogicalDevice()
{
	// Get the queue families from the graphics card.
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

	// Creation information.
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint>(m_VkDeviceExtenstions.size());
	createInfo.ppEnabledExtensionNames = m_VkDeviceExtenstions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = (uint)m_VkValidationLayers.size();
		createInfo.ppEnabledLayerNames = m_VkValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkLogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Logical device creation failed");
	}

	// Get the graphics and present family queues.
	vkGetDeviceQueue(m_VkLogicalDevice, indicies.m_GraphicsFamily.value(), 0, &m_VkGraphicsQueue);
	vkGetDeviceQueue(m_VkLogicalDevice, indicies.m_PresentFamily.value(), 0, &m_VkPresentQueue);
}

SwapChainSupportDetails VulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_VkSurface, &details.m_Capabilities);

	uint formatCount = 0;

	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VkSurface, &formatCount, nullptr);

	if (formatCount > 0)
	{
		details.m_Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VkSurface, &formatCount, details.m_Formats.data());
	}

	uint presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VkSurface, &presentModeCount, nullptr);

	if (presentModeCount > 0)
	{
		details.m_PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VkSurface, &presentModeCount, details.m_PresentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaliableFormats)
{
	for (const auto& avaliableFormat : avaliableFormats)
	{
		// Check if the current surface format supports 32 bit colour.
		// If it does, return it. Just get the first surface format that supports it.
		if (avaliableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && avaliableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return avaliableFormat;
		}
	}
}

VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaliablePresentModes)
{
	for (const auto& avaliablePresentMode : avaliablePresentModes)
	{
		if (avaliablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return avaliablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { m_WindowWidth, m_WindowHeight };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void VulkanRenderer::CreateSwapChain()
{
	// Check swap chain support.
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_VkPhysicalDevice);

	// Get a swap chain surface format.
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.m_Formats);
	// Get a swap chain present mode.
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_PresentModes);
	// Get the extents of the surface.
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.m_Capabilities);

	// Amount of images we can have in the swap chain.
	// Just the minimum + 1.
	uint imageCount = swapChainSupport.m_Capabilities.minImageCount + 1;

	if (swapChainSupport.m_Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_Capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.m_Capabilities.maxImageCount;
	}

	// Creation information.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_VkSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Queue families indices.
	QueueFamilyIndices indices = FindQueueFamilies(m_VkPhysicalDevice);
	uint queueFamilyIndices[] = { indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value() };

	if (indices.m_GraphicsFamily != indices.m_PresentFamily) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.m_Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create the swap chain object.
	if (vkCreateSwapchainKHR(m_VkLogicalDevice, &createInfo, nullptr, &m_VkSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_VkLogicalDevice, m_VkSwapChain, &imageCount, nullptr);
	m_VkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_VkLogicalDevice, m_VkSwapChain, &imageCount, m_VkSwapChainImages.data());
	m_VkSwapChainImageFormat = surfaceFormat.format;
	m_VkSwapChainExtent = extent;
}

void VulkanRenderer::CreateImageViews()
{
	m_VkSwapChainImageViews.resize(m_VkSwapChainImages.size());

	for (size_t i = 0; i < m_VkSwapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_VkSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_VkSwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_VkLogicalDevice, &createInfo, nullptr, &m_VkSwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views");
		}
	}
}

void VulkanRenderer::CreateGraphicsPipeline()
{
	std::vector<char> vertShaderCode = ReadFile("../Shaders/Simple/vert.spv");
	std::vector<char> fragShaderCode = ReadFile("../Shaders/Simple/frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_VkSwapChainExtent.width;
	viewport.height = (float) m_VkSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_VkSwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(m_VkLogicalDevice, &pipelineLayoutInfo, nullptr, &m_VkPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = m_VkPipelineLayout;
	pipelineInfo.renderPass = m_VkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(m_VkLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_VkGraphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("Failed to create graphics pipeline!");

	vkDestroyShaderModule(m_VkLogicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_VkLogicalDevice, vertShaderModule, nullptr);
}

std::vector<char> VulkanRenderer::ReadFile(const std::string& fileName)
{
	// Get the text from the file.
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

VkShaderModule VulkanRenderer::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_VkLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");

	return shaderModule;
}

void VulkanRenderer::CreateRenderPass()
{
	VkAttachmentDescription colourAttachment{};
	colourAttachment.format = m_VkSwapChainImageFormat;
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colourAttachmentRef{};
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colourAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_VkLogicalDevice, &renderPassInfo, nullptr, &m_VkRenderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");
}

void VulkanRenderer::CreateFramebuffers()
{
	m_VkSwapChainFramebuffers.resize(m_VkSwapChainImageViews.size());

	for (size_t i = 0; i < m_VkSwapChainImageViews.size(); ++i)
	{
		VkImageView attachments[] = { m_VkSwapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_VkRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_VkSwapChainExtent.width;
		framebufferInfo.height = m_VkSwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_VkLogicalDevice, &framebufferInfo, nullptr, &m_VkSwapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create framebuffer!");
	}
}

void VulkanRenderer::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_VkPhysicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily.value();

	if (vkCreateCommandPool(m_VkLogicalDevice, &poolInfo, nullptr, &m_VkCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

void VulkanRenderer::CreateCommandBuffers()
{
	m_VkCommandBuffers.resize(m_VkSwapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_VkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint)m_VkCommandBuffers.size();

	if (vkAllocateCommandBuffers(m_VkLogicalDevice, &allocInfo, m_VkCommandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers!");

	for (size_t i = 0; i < m_VkCommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_VkCommandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_VkRenderPass;
		renderPassInfo.framebuffer = m_VkSwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_VkSwapChainExtent;

		VkClearValue clearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColour;

		vkCmdBeginRenderPass(m_VkCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_VkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkGraphicsPipeline);
		vkCmdDraw(m_VkCommandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(m_VkCommandBuffers[i]);

		if (vkEndCommandBuffer(m_VkCommandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}