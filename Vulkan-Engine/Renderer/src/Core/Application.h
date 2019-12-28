/***************************************************************************
 * Filename		: Application.h
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
#pragma once

#include "Core.h"
#include "Window/Window.h"

namespace Vulkan_Engine
{
	namespace Renderer
	{
		enum class VKE_RESULT { VKE_SUCCESS = 1, VKE_FAILURE = 2 };

		class Window;
		
		class Application
		{
		public:
			Application();
			~Application();
		public:
			VKE_RESULT Run();
		private:
			VKE_RESULT InitEngineUtils();
			VKE_RESULT InitWindow(); 
			VKE_RESULT InitVulkan();
			VKE_RESULT UpdateLoop();
			VKE_RESULT Cleanup();
		private:
			bool m_Running;
			Ref<Window> m_Window;
		};
	}
}