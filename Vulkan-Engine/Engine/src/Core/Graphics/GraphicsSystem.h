// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once
#include "Core/Core.h"
#include "Core/Events/Event.h"

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
			void OnUpdate(Timestep deltaTime) const;

			void Cleanup() const;

			void SetWindowEventCallback(const EventCallbackFunction& callback) const;

		public:
			static float GetTime();

		private:
			void Init();

		private:
			Ref<Window> m_Window;
		};
	}
}
