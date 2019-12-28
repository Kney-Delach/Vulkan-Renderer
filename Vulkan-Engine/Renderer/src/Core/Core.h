/***************************************************************************
 * Filename		: Core.h
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Contains some helpful utility declarations used throughout the engine.
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
#include <memory>

/** Debug macro implementation. */
#ifdef VKE_DEBUG
#define VKE_ENABLE_ASSERTS
#endif

#ifdef VKE_ENABLE_ASSERTS
#define VK_ASSERT(x, ...) { if(!(x)) { VK_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define VK_CORE_ASSERT(x, ...) { if(!(x)) { VK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define VK_ASSERT(x, ...)
#define VK_CORE_ASSERT(x, ...)
#endif

/** bitwise bitfield macro, used for applying multiple categories to a single event type. */
#define BIT(x) (1 << x)

/** Scope and Reference definitions. */
namespace Vulkan_Engine
{
	// unique pointers
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// shared pointers
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}