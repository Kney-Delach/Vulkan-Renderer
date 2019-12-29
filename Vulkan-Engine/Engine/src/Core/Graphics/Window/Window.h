/***************************************************************************
 * Filename		: Window.h
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
#pragma once
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN // includes the vulkan header 
#include <GLFW/glfw3.h>

#include "Core/Events/Event.h"

namespace Vulkan_Engine
{
	// forward declarations 
	class Timestep; 

	using EventCallbackFunction = std::function<void(Event&)>;

	namespace Graphics
	{
		struct WindowProperties
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;

			WindowProperties(const std::string& title = "Vulkan-Engine", unsigned int width = 800, unsigned int height = 600)
				: Title(title), Width(width), Height(height) {}

			WindowProperties(const WindowProperties& copy)
			{
				Title = copy.Title;
				Width = copy.Width;
				Height = copy.Height;
			}
		};
		
		class Window
		{
		public:
			Window();
			~Window();
		public:
			void OnUpdate(const Timestep deltaTime);
			void Cleanup();
			_NODISCARD GLFWwindow* GetWindow() const { return m_Window; }
			void SetEventCallback(const EventCallbackFunction& callback);
			void SetVSync(const bool enabled);
		private:
			void InitWindow();
			void InitGLFW(std::vector<const char*>& extensions);
			void VerifyVulkanExtensionsAvailable(std::vector<const char*>& extensions) const;
			void InitVulkan();
			void CreateVulkanInstance();
			void SetGLFWCallbacks();
			void SetGLFWConfigurations();
		private:
			//todo: abstract the window, and vk instances into a structure of rendering context 
			GLFWwindow* m_Window = nullptr;
			VkInstance m_VkInstance;
			struct WindowData
			{
				WindowProperties Properties;
				bool VSync;
				EventCallbackFunction EventCallback;
			};
			WindowData m_WindowData;
		};
	}
}
