/***************************************************************************
 * Filename		: Timestep.h
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Used for frame time differencing .
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

namespace Vulkan_Engine
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_Time(time) {}
		~Timestep() = default;
		inline operator float() const { return m_Time; }
		inline float GetSeconds() const { return m_Time; }
		inline float GetMilliseconds() const { return m_Time * 1000.f; }
	private:
		float m_Time; 
	};	
}