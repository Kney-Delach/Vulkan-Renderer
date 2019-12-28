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
			InitVulkan();
			MainLoop();
			Cleanup();
			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::InitEngineUtils()
		{
			Log::Init();
			VK_CORE_DEBUG("Initializing Engine Utilities");

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::InitVulkan()
		{
			VK_CORE_DEBUG("Initializing Vulkan");

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::MainLoop()
		{
			VK_CORE_DEBUG("Running main loop");

			return VKE_RESULT::VKE_SUCCESS;
		}

		VKE_RESULT Application::Cleanup()
		{
			VK_CORE_DEBUG("Cleaning up engine data");

			return VKE_RESULT::VKE_SUCCESS;
		}
	}
}