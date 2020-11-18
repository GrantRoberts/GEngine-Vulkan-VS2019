#include "Application.h"

	Application::Application()
	{
		m_ShuttingDown = false;

		m_VulkanRenderer = new VulkanRenderer(1280, 720);

		m_GameScene = new Scene();
		m_GameScene->CreateSemaphores(m_VulkanRenderer);
	}

	Application::~Application()
	{
		m_GameScene->DeleteSamophores(m_VulkanRenderer);
		delete m_GameScene;
		m_GameScene = nullptr;

		delete m_VulkanRenderer;
		m_VulkanRenderer = nullptr;
	}

	bool Application::Startup()
	{
		return true;
	}

	void Application::Run()
	{
		while (!m_ShuttingDown)
		{
			m_CurrentFrame = glfwGetTime();
			m_DeltaTime = m_CurrentFrame - m_LastFrame;
			m_LastFrame = m_CurrentFrame;

			m_GameScene->Update((float)m_DeltaTime);

			m_GameScene->Draw(m_VulkanRenderer);
			vkDeviceWaitIdle(m_VulkanRenderer->GetLogicalDevice());
		}

		return;
	}