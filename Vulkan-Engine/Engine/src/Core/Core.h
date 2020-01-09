// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once
#include <memory>

// Debug macro implementation.
#ifdef VKE_DEBUG
#define VKE_ENABLE_ASSERTS
#define VKE_ENABLE_VALIDATION
#endif

#ifdef VKE_ENABLE_ASSERTS
#define VKE_ASSERT(x, ...) { if(!(x)) { VK_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define VKE_CORE_ASSERT(x, ...) { if(!(x)) { VK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define VK_ASSERT(x, ...)
#define VK_CORE_ASSERT(x, ...)
#endif

// Bitwise bitfield macro, used for applying multiple categories to a single event type.
#define BIT(x) (1 << x)

// Event binding macro with single placeholder. 
#define VKE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

// Safely delete for pointers
#define VKE_SAFE_DELETE(p) if (p != nullptr) { delete p; p = nullptr; }

// Scope and Reference definitions.
namespace Vulkan_Engine
{
	// unique pointers
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// shared pointers
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
