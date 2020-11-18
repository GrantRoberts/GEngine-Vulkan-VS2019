#pragma once
#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"
#include <string>

class VulkanRenderer
{
public:
	// Constructor.
	VulkanRenderer(float width, float height);

	// Destructor.
	~VulkanRenderer();

	VkDevice GetLogicalDevice() { return m_VkLogicalDevice; }

	VkSwapchainKHR GetSwapChain() { return m_VkSwapChain; }

	std::vector<VkCommandBuffer> GetCommandBuffers() { return m_VkCommandBuffers; }

	VkQueue GetGraphicsQueue() { return m_VkGraphicsQueue; }

	VkQueue GetPresentQueue() { return m_VkPresentQueue; }

private:
	//-------------------------------------------------------------------------------
	// Functions.
	//-------------------------------------------------------------------------------
	// Create a GFLW window.
	void CreateGLFWWindow();

	// Create a vulkan instance.
	void CreateInstance();

	// Pick a graphics card to use for rendering.
	void PickPhysicalDevice();

	// Create a vulkan surface.
	void CreateSurface();

	// Is the device suitable for vulkan rendering?
	// Params: the device to check.
	// Returns: if the device is suitable for vulkan rendering.
	bool IsDeviceSuitable(VkPhysicalDevice device);

	// Check if the device supports the required extensions.
	// Params: the device to check.
	// Returns: if the device supports the required extensions.
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	// Check if validation layers are supported.
	// Returns: if validation layers are supported.
	bool CheckValidationLayerSupport();

	// Get the extensions required.
	// Returns: std::vector of const char*'s that represent the required extensions.
	std::vector<const char*> GetRequiredExtensions();

	// Populate debug messages for debugging when something goes wrong.
	// Params: the debug message create info.
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// Get the message to output to the console for debugging purposes.
	// Params: severity of the message as vulkan bit mask, message type as vulkan flag, callback data, user data.
	// Returns: VK_FALSE.
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	// Find the vulkan families the device supports.
	// Params: the device to check.
	// Returns: the queue family indicies of that device.
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	// Create a vulkan logical device.
	void CreateLogicalDevice();

	// Check swap chain support.
	// Params: the graphics card to check for.
	// Returns: details of swap chain support.
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	// Get a swap surface format that the graphics card supports.
	// Params: array of avaliable formats.
	// Returns: format that supports 32 bit colour.
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaliableFormats);

	// Get a swap present mode the graphics card supports.
	// Params: array of avaliable present modes.
	// Returns: present mode we want.
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaliablePresentModes);

	// Get the extents of the image to swap.
	// Params: the swap surface.
	// Returns: the extents of the surface.
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	// Create the swap chain.
	void CreateSwapChain();

	// Create the view images.
	void CreateImageViews();

	// Create the graphics pipeline.
	void CreateGraphicsPipeline();

	// Read a file.
	// Params: the path to the file.
	// Returns: char vector of the contents of the file.
	std::vector<char> ReadFile(const std::string& fileName);

	// Create shader modules.
	// Params: the shader code.
	// Returns: Vulkan shader module with the shader code.
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	// Create a render pass.
	void CreateRenderPass();

	// Create the framebuffers.
	void CreateFramebuffers();

	// Create the command pool.
	void CreateCommandPool();

	// Create command buffers.
	void CreateCommandBuffers();

	//-------------------------------------------------------------------------------
	// Variables.
	//-------------------------------------------------------------------------------
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

	// The vulkan logical device.
	VkDevice m_VkLogicalDevice;

	// The swap chain.
	VkSwapchainKHR m_VkSwapChain;

	// Images being drawn to for the swap chain.
	std::vector<VkImage> m_VkSwapChainImages;

	// The image format.
	VkFormat m_VkSwapChainImageFormat;

	// The extents of the image.
	VkExtent2D m_VkSwapChainExtent;

	// The queue for drawing graphics.
	VkQueue m_VkGraphicsQueue;

	// The presentation queue.
	VkQueue m_VkPresentQueue;

	// The vulkan render pass.
	VkRenderPass m_VkRenderPass;

	// The graphics pipeline layout.
	VkPipelineLayout m_VkPipelineLayout;

	// The graphics pipeline.
	VkPipeline m_VkGraphicsPipeline;

	// View of the swap chain images.
	std::vector<VkImageView> m_VkSwapChainImageViews;

	// The framebuffers.
	std::vector<VkFramebuffer> m_VkSwapChainFramebuffers;

	// Manager of memory for buffers and command buffers.
	VkCommandPool m_VkCommandPool;

	// The command buffers.
	std::vector<VkCommandBuffer> m_VkCommandBuffers;

	// Validation layers
	std::vector<const char*> m_VkValidationLayers;

	// Device extensions.
	std::vector<const char*> m_VkDeviceExtenstions;

	// Width of the window.
	float m_WindowWidth;

	// Height of the window.
	float m_WindowHeight;
};