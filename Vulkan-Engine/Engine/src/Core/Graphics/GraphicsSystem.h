#pragma once
#include "Core/Core.h"
#include "Core/Events/Event.h"

#define GLFW_INCLUDE_VULKAN // includes the vulkan header 
#include "GLFW/glfw3.h"


namespace Vulkan_Engine
{
	// forward Declarations
	class Timestep;
	using EventCallbackFunction = std::function<void(Event&)>;

	namespace Graphics
	{
		class Window;

		class GraphicsSystem
		{
		public:
			GraphicsSystem();
			~GraphicsSystem();
		public:
			void OnUpdate(const Timestep deltaTime);
			void Cleanup();
			void SetWindowEventCallback(const EventCallbackFunction& callback) const;
		public:
			_NODISCARD static float GetTime() { return static_cast<float>(glfwGetTime()); };
		private:
			void Init();
		private:
			Ref<Window> m_Window;
		};
	}
}
