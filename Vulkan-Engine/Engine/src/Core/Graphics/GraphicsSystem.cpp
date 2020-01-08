#include "vkepch.h"
#include "GraphicsSystem.h"

#include "Window.h"
#include "Core/Logger/Log.h"
#include "Core/Utils/Timestep.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{

		GraphicsSystem::GraphicsSystem()
		{
			Init();
		}

		GraphicsSystem::~GraphicsSystem()
		{
		}

		void GraphicsSystem::OnUpdate(const Timestep deltaTime)
		{
			m_Window->OnUpdate(deltaTime);
		}

		void GraphicsSystem::Cleanup()
		{
			m_Window->Cleanup();
		}

		void GraphicsSystem::SetWindowEventCallback(const EventCallbackFunction& callback) const
		{
			m_Window->SetEventCallback(callback);
		}		

		void GraphicsSystem::Init()
		{
			m_Window = CreateRef<Window>();
		}
	}
}