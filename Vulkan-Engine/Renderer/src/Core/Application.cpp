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

#include "vulkan/vulkan.h"

namespace Vulkan_Engine
{
	namespace Renderer
	{
		Application::Application() : m_Running(true)
		{
		}

		Application::~Application()
		{
		}

		VKE_RESULT Application::Run()
		{
			InitEngineUtils();
			InitWindow();
			InitVulkan();
			UpdateLoop();
			Cleanup();
			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::InitEngineUtils()
		{
			Log::Init();
			VK_CORE_DEBUG("Initializing Engine Utilities");

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::InitWindow()
		{
			VK_CORE_DEBUG("Initializing Application Window");

			m_Window = CreateRef<Window>();

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::InitVulkan()
		{
			VK_CORE_DEBUG("Initializing Vulkan");

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::UpdateLoop()
		{
			VK_CORE_DEBUG("Running main loop");
			while(m_Running) //todo: Make this a callback
			{
				m_Running = m_Window->OnUpdate();
			}
			
			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::Cleanup()
		{
			VK_CORE_DEBUG("Cleaning up engine data");
			m_Window->CleanupBegin();
			m_Window->CleanupEnd(); 
			return VKE_RESULT::VKE_SUCCESS;
		}
	}
}
