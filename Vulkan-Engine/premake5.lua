-- Project  : Vulkan-Renderer
-- Filename : premake5.lua
-- Author   : Ori Lazar
-- Date     : 28/12/2019

workspace "Vulkan-Engine"
	architecture "x64"
	startproject "Engine"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Engine/Dependencies/GLFW/include"
IncludeDir["glm"] = "Engine/Dependencies/glm"

group "Dependencies"
	include "Engine/Dependencies/GLFW"
group ""

-- Exalted graphics engine
project "Engine"
	location "Engine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("Bin/Output/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin/Intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "vkepch.h"
	pchsource "Engine/src/vkepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Dependencies/glm/glm/**.hpp",
		"%{prj.name}/Dependencies/glm/glm/**.inl"
		-- "Resources/Shaders/**.spv"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/Dependencies/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"C:/VulkanSDK/1.1.130.0/Include",
		"%{prj.name}/Resources/",
		"%{prj.name}/Dependencies/STB/include"
	}

	libdirs 
	{
		"C:/VulkanSDK/1.1.130.0/Lib"
	}
	links
	{
		"GLFW",
		"vulkan-1.lib"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VKE_PLATFORM_WINDOWS",
			"VKE_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "VKE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VKE_RELEASE"
		runtime "Release"
		optimize "on"