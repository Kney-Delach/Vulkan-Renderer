/***************************************************************************
 * Filename		: Log.cpp
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Contains a wrapper class implementation for spdlog logging functionality.
     .---.
   .'_:___".
   |__ --==|
   [  ]  :[|
   |__| I=[|
   / / ____|
  |-/.____.'
 /___\ /___\
***************************************************************************/
#include "vkepch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_sinks.h>

namespace Vulkan_Engine
{
	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_AppLogger;

	void Log::Init()
	{
		// severity | timestamp | logger_name | message
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("EXALTED");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_AppLogger = spdlog::stdout_color_mt("SANDBOX");
		s_AppLogger->set_level(spdlog::level::trace);
	}
}