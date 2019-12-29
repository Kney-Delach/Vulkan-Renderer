/***************************************************************************
 * Filename		: Application.cpp
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Definition of an application wrapper for this engine.
     .---.
   .'_:___".
   |__ --==|
   [  ]  :[|
   |__| I=[|
   / / ____|
  |-/.____.'
 /___\ /___\
***************************************************************************/
#include "vkepch.h"
#include "Application.h"

#include "Logger/Log.h"
#include "Graphics/GraphicsSystem.h"

#include "Events/ApplicationEvent.h"

namespace Vulkan_Engine
{
	Application::Application() : m_Running(true)
	{
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(VKE_BIND_EVENT_FN(Application::OnWindowClosed));
		//TODO: Implement layers to handle the rest of the events
	}

	void Application::Run()
	{
		InitLogger();
		InitGraphics();
		UpdateLoop();
		Cleanup();
	}

	VKE_RESULT Application::InitLogger()
	{
		Log::Init();
		VK_CORE_DEBUG("[Application]: Initialized Engine Logger");
		return VKE_RESULT::VKE_SUCCESS;
	}

	VKE_RESULT Application::InitGraphics()
	{
		VK_CORE_DEBUG("[Application]: Initializing Graphics System");
		m_GraphicsSystem = CreateRef<Graphics::GraphicsSystem>();
		m_GraphicsSystem->SetWindowEventCallback(VKE_BIND_EVENT_FN(Application::OnEvent));
		return VKE_RESULT::VKE_SUCCESS;
	}

	VKE_RESULT Application::UpdateLoop()
	{
		VK_CORE_DEBUG("[Application]: Running main loop");
		while(m_Running)
		{
			//calculate time difference between frames
			const float time = Graphics::GraphicsSystem::GetTime();
			const Timestep deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;
			m_GraphicsSystem->OnUpdate(deltaTime);
		}		
		return VKE_RESULT::VKE_SUCCESS;
	}

	VKE_RESULT Application::Cleanup()
	{
		VK_CORE_DEBUG("[Application]: Cleaning up engine data");
		m_GraphicsSystem->Cleanup();
		return VKE_RESULT::VKE_SUCCESS;
	}

	bool Application::OnWindowClosed(WindowClosedEvent& closedEvent)
	{
		VK_CORE_TRACE("[Application]: Application::OnWindowClosed called with event: {0}", closedEvent.GetName());
		m_Running = false;
		return true;
	}
}