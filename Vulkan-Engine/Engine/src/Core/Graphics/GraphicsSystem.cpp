// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#include "vkepch.h"
#include "GraphicsSystem.h"

#include "Core/Window/Window.h"
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

		GraphicsSystem::~GraphicsSystem() { }

		void GraphicsSystem::OnUpdate(const Timestep deltaTime) const
		{
			m_Window->OnUpdate(deltaTime);
		}

		void GraphicsSystem::Cleanup() const
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

		float GraphicsSystem::GetTime()
		{
			return Window::GetTime();
		}
	}
}
