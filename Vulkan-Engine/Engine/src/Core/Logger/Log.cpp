// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#include "vkepch.h"
#include "Log.h"

namespace Vulkan_Engine
{
	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_AppLogger;

	void Log::Init()
	{
		// severity | timestamp | logger_name | message
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_AppLogger = spdlog::stdout_color_mt("CLIENT_APPLICATION");
		s_AppLogger->set_level(spdlog::level::trace);
	}
}
