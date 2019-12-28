#pragma once


// external forward declarations 
struct GLFWwindow;

namespace Vulkan_Engine
{
	namespace Renderer
	{
		class Window
		{
		public:
			Window();
			~Window();
		public:
			bool OnUpdate(); //TODO: Add error catching
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