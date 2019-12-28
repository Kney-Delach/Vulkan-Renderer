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

// external forward declarations 
struct GLFWwindow;

namespace Vulkan_Engine
{
	namespace Renderer
	{
		class Timestep;

		class Window
		{
		public:
			Window();
			~Window();
		public:
			bool OnUpdate(Timestep deltaTime); //TODO: Add error catching
			void CleanupBegin() const;
			void CleanupEnd() const;
			GLFWwindow* GetWindow() const { return m_Window; }
			int GetWindowWidth() const { return m_Width; }
			int GetWindowHeight() const { return m_Height; }
		private:
			void InitWindow();
		private:
			GLFWwindow* m_Window;
		private:
			const int m_Width = 800;
			const int m_Height = 600; 
		};
	}
}