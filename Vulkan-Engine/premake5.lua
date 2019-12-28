-- Project  : Vulkan-Renderer
-- Filename : premake5.lua
-- Author   : Ori Lazar
-- Date     : 28/12/2019

workspace "Vulkan-Engine"
	architecture "x64"
	startproject "Renderer"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Renderer/Dependencies/GLFW/include"
IncludeDir["glm"] = "Renderer/Dependencies/glm"

group "Dependencies"
	include "Renderer/Dependencies/GLFW"
group ""

-- Exalted graphics engine
project "Renderer"
	location "Renderer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("Bin/Output/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin/Intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "vkepch.h"
	pchsource "Renderer/src/vkepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Dependencies/glm/glm/**.hpp",
		"%{prj.name}/Dependencies/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Resources/",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"C:/VulkanSDK/1.1.130.0/Include"
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