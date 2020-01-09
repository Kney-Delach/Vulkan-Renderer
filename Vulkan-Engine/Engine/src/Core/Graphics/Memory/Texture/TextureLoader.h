#pragma once


namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: classes
		class Image;
		class LogicalDevice;
		class DeviceMemory;

		class TextureLoader
		{
		public:
			TextureLoader(const LogicalDevice* logicalDevice);
			~TextureLoader();
		private:
			const LogicalDevice* m_LogicalDevice;
			DeviceMemory* m_DeviceMemory;
		private:
			static const std::string s_TexturePath;
			static const std::string s_TextureFileExtension;
		};
	}
}