// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once

namespace Vulkan_Engine
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_Time(time) {}

		~Timestep() = default;

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }

		float GetMilliseconds() const { return m_Time * 1000.f; }

	private:
		float m_Time;
	};
}
