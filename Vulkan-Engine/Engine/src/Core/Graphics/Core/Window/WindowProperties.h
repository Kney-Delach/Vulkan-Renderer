// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once

#include "Core/Events/Event.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		struct WindowProperties
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool FramebufferResized; // Explicit handling of framebuffer resizing 

			WindowProperties(const std::string& title = "Vulkan-Engine", unsigned int width = 800,
			                 unsigned int height = 600)
				: Title(title), Width(width), Height(height), FramebufferResized(false) {}

			WindowProperties(const WindowProperties& copy)
			{
				Title = copy.Title;
				Width = copy.Width;
				Height = copy.Height;
				FramebufferResized = copy.FramebufferResized;
			}
		};

		struct VulkanProperties
		{
			VkDebugUtilsMessageSeverityFlagBitsEXT Severity; //TODO: Set this value in window data 
		};
		
		using EventCallbackFunction = std::function<void(Event&)>;

		struct WindowData
		{
			WindowProperties Properties;
			bool VSync;
			EventCallbackFunction EventCallback;
			VulkanProperties VulkanProperties;
		};
	}
}
