/***************************************************************************
 * Filename		: Window.cpp
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Windowing abstraction for this engine.
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
#include "Window.h"
#include "Core/Logger/Log.h"
#include "Core/Timers/Timestep.h"

#define GLFW_INCLUDE_VULKAN // includes the vulkan header 
#include <GLFW/glfw3.h>

namespace Vulkan_Engine
{
	namespace Renderer
	{
		Window::Window()
		{
			InitWindow();
		}

		Window::~Window()
		{
		}

		bool Window::OnUpdate(Timestep deltaTime)
		{
			if(!glfwWindowShouldClose(m_Window))
			{
=				glfwPollEvents();
				return true;
			}
			return false;
		}

		void Window::InitWindow()
		{
			VK_CORE_DEBUG("Initializing GLFW Window & Setting Hints");

			glfwInit();
			glfwSwapInterval(1);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no opengl context
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // because resizing is more complex

			m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan-Engine: Renderer Application", nullptr, nullptr);
		}

		void Window::CleanupBegin() const 
		{
			glfwDestroyWindow(m_Window);
		}

		void Window::CleanupEnd() const 
		{
			glfwTerminate();
		}
	}
}