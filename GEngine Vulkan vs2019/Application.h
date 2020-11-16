#pragma once
#include "VulkanRenderer.h"

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

	private:
		// The vulkan renderer.
		VulkanRenderer* m_VulkanRenderer;

		// If the game is shutting down.
		bool m_ShuttingDown;

		// The game scene.
		Scene* m_GameScene;

		// For calculating delta time.
		double m_CurrentFrame = 0.0;
		double m_LastFrame = 0.0;
		double m_DeltaTime = 0.0;
 };