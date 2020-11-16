#include "Application.h"

	Application::Application()
	{
		m_ShuttingDown = false;

		m_VulkanRenderer = new VulkanRenderer(1280, 720);

		m_GameScene = new Scene();
	}

	Application::~Application()
	{
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
			//std::cout << "Application is running!" << std::endl;

			m_CurrentFrame = glfwGetTime();
			m_DeltaTime = m_CurrentFrame - m_LastFrame;
			m_LastFrame = m_CurrentFrame;

			m_GameScene->Update((float)m_DeltaTime);
			m_GameScene->Draw();
		}

		return;
	}