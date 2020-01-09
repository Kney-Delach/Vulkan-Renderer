// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once

#include "Core/Core.h"

#include <spdlog/spdlog.h>

namespace Vulkan_Engine
{
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_AppLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_AppLogger;
	};
}

// Core engine logging macros
#define VK_CORE_TRACE(...)    ::Vulkan_Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VK_CORE_DEBUG(...)    ::Vulkan_Engine::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define VK_CORE_INFO(...)     ::Vulkan_Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VK_CORE_WARN(...)     ::Vulkan_Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VK_CORE_ERROR(...)    ::Vulkan_Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VK_CORE_CRITICAL(...) ::Vulkan_Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)


// Application logging macros
#define VK_TRACE(...)    ::Vulkan_Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VK_DEBUG(...)    ::Vulkan_Engine::Log::GetClientLogger()->debug(__VA_ARGS__)
#define VK_INFO(...)     ::Vulkan_Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define VK_WARN(...)     ::Vulkan_Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VK_ERROR(...)    ::Vulkan_Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define VK_CRITICAL(...) ::Vulkan_Engine::Log::GetClientLogger()->critical(__VA_ARGS__)
