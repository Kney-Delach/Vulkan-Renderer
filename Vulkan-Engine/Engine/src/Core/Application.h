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
#include "Utils/Timestep.h"

namespace Vulkan_Engine
{
	// forward declarations
	class WindowClosedEvent;
	class WindowResizeEvent;
	class Event;
	namespace Graphics { class GraphicsSystem; }

	enum class VKE_RESULT { VKE_SUCCESS = 1, VKE_FAILURE = 2 };
	
	class Application
	{
	public:
		Application();
		~Application();	
	public:
		void Run();
		void OnEvent(Event& e);
	private:
		VKE_RESULT InitLogger();
		VKE_RESULT InitGraphics();
		VKE_RESULT UpdateLoop(); 
		VKE_RESULT Cleanup();
	private:
		bool OnWindowClosed(WindowClosedEvent& closedEvent);
		bool OnWindowResize(WindowResizeEvent& resizeEvent);
	private:
		bool m_Running;
		Timestep m_LastFrameTime;
		Ref<Graphics::GraphicsSystem> m_GraphicsSystem;
	};
}