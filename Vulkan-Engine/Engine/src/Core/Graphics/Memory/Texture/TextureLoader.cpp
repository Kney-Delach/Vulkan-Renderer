#include "vkepch.h"

#include "TextureLoader.h"
#include "Image.h"

#include "Core/Graphics/Core/Devices/LogicalDevice.h"
#include "Core/Graphics/Memory/DeviceMemory.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		const std::string TextureLoader::s_TexturePath = "Core../Resources/Textures/";
		const std::string TextureLoader::s_TextureFileExtension = ".dds";

		
		TextureLoader::TextureLoader(const LogicalDevice* logicalDevice)
			: m_LogicalDevice(logicalDevice), m_DeviceMemory(logicalDevice->GetDeviceMemory())
		{
		}

		TextureLoader::~TextureLoader() {}
	}
}