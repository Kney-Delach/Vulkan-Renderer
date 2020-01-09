// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN // includes the vulkan header 
#include <GLFW/glfw3.h>

#include "Core/Graphics/Memory/Mesh/Vertex.h"

#include "WindowProperties.h"

#define LOAD_MODEL 0

namespace Vulkan_Engine
{
	// forward declarations 
	class Timestep;

	namespace Graphics
	{
		// forward declarations: classes
		class Validation;
		class PhysicalDevice;
		class LogicalDevice;

		// forward declaration: structs
		struct DeviceSurfaceCapabilities;
		
		struct WindowGraphicsDetails
		{
			GLFWwindow* Window = nullptr; // glfw window
			PhysicalDevice* PhysicalDevice = nullptr; // active physical device
 			LogicalDevice* LogicalDevice = nullptr; // active logical device 
			VkSurfaceKHR Surface = nullptr; // window surface 
			VkInstance VulkanInstance = nullptr; // vulkan instance 
		};
		
		////////////////////////////////////////////////////////////////////////////////////////
		////TODO: Reorganise this data below, currently placed here for refactoring purposes. //
		////////////////////////////////////////////////////////////////////////////////////////
		
		class Window
		{
		public: // inline STATIC public functions
			_NODISCARD static float GetTime() { return static_cast<float>(glfwGetTime()); }
		public: // inline NON-STATIC public functions
			_NODISCARD GLFWwindow* GetWindow() const { return m_GraphicsDetails.Window; }
			void SetEventCallback(const EventCallbackFunction& callback) { m_WindowData.EventCallback = callback; }
		public: // public functions  (not inline)
			void SetVSync(bool enabled);
			void OnUpdate(Timestep deltaTime);
			void Cleanup();
		private: // private functions (not inline)
			void InitWindow();
			// GLFW Setup Functions
			void InitGLFW(std::vector<const char*>& extensions);
			void SetGLFWConfigurations();
			void SetGLFWCallbacks() const;
			void VerifyExtensionsAvailable(std::vector<const char*>& extensions) const;
			// Vulkan Setup Functions
			void CreateVulkanInstance(const std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& enabledLayerNames);
			void CreateVulkanDevices(DeviceSurfaceCapabilities& surfaceCapabilities, uint32_t& enabledLayerCount, const char* const*& enabledLayerNames);
		private: // private variables
			WindowGraphicsDetails m_GraphicsDetails;
			Validation* m_ValidationLayer; 
		public: // constructor + destructor
			Window();
			~Window();

		private:
			void CreateVulkanWindowSurface();
			void InitVulkanPhysicalDevice();
			void InitVulkanLogicalDevice();
			void CreateVulkanSwapChain();
			void CreateVulkanImageViews();
			void CreateGraphicsRenderPass();
			void CreateGraphicsPipeline();
			void CreateFramebuffers();
			void CreateCommandPool();
			void CreateCommandBuffers();
			///////////////////////////////
			void CreateSyncObjects();
			void RenderFrame(Timestep deltaTime);
			///////////////////////////////
			//// Everything that has to do with swap chain recreation
			///////////////////////////////
			void CleanupSwapChain();
			void RecreateSwapChain();
			///////////////////////////////
			// Vertex buffer data
			///////////////////////////////
			void CreateVertexBuffer(); // does not depend on swap chain
			void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			                  VkBuffer& buffer, VkDeviceMemory& bufferMemory); // abstracted buffer creation function 
			void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
			void CreateIndexBuffer();
			///////////////////////////////
			// Descriptor layouts (uniform buffers)
			///////////////////////////////
			void CreateDescriptorSetLayout();
			void CreateUniformBuffers();
			void CreateDescriptorPool();
			void CreateDescriptorSets();
			void UpdateUniformBuffer(uint32_t imageIndex, Timestep deltaTime);
			///////////////////////////////
			// Texture Mapping 
			///////////////////////////////
			void CreateTextureImage();
			void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
			                 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			                 VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
			void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
			                           uint32_t mipLevels) const;
			void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
			void CreateTextureImageView();
			void CreateTextureSampler();
			VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
			                            uint32_t mipLevels) const;
			// mip map generation
			void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
			                     uint32_t mipLevels) const;
			// recording and executing command buffer abstractions 
			VkCommandBuffer BeginSingleTimeCommands() const;
			void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;
			// depth buffer stuff
			void CreateDepthResources();
			// msaa utility functions
			VkSampleCountFlagBits GetMaxUsableSampleCount() const;
			void CreateColorResources();
		private:
			//todo: abstract the window, and vk instances / device into a structure of rendering context
			// glfw and mindow variables 
			//GLFWwindow* m_Window = nullptr;
			WindowData m_WindowData;
			// vulkan stuff
			VkInstance m_VkInstance;
			VkDebugUtilsMessengerEXT m_DebugCallbackMessenger;
			VkPhysicalDevice m_PhysicalDevice = nullptr;
			VkDevice m_LogicalDevice;
			VkQueue m_GraphicsQueueHandle; // handle for graphics queue
			VkQueue m_PresentQueueHandle; // handle for presentation queue
			VkSurfaceKHR m_WindowSurface;
			// window surface (create directly after instance creation as can affect physical device)
			VkSwapchainKHR m_SwapChain;
			std::vector<VkImage> m_SwapChainImages;
			std::vector<VkImageView> m_SwapChainImageViews;
			// describes how to access an image, and which part of the image to access.
			VkFormat m_SwapChainImageFormat;
			VkExtent2D m_SwapChainExtent;
			VkRenderPass m_RenderPass;
			VkDescriptorSetLayout m_DescriptorSetLayout; // descriptor set layout (combines all descriptor bindings)
			VkPipelineLayout m_PipelineLayout;
			VkPipeline m_GraphicsPipeline;
			std::vector<VkFramebuffer> m_SwapChainFramebuffers;
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
			//////////////////////////////////////////////// (each frame should have its own) 
			std::vector<VkSemaphore> m_ImageAvailableSemaphores; // signal image has been acquired & ready for rendering
			std::vector<VkSemaphore> m_RenderFinishedSemaphores;
			// signal that rendering has finished & presentation can happen
			std::vector<VkFence> m_InFlightFences; // used for cpu <-> gpu synchronization
			std::vector<VkFence> m_ImagesInFlight;
			// used to track for each swap chain image, if a frame in flight is currently using it
			size_t m_CurrentFrame = 0;
			// frame index used to keep track of the current frame for correct semaphore usage
#if LOAD_MODEL
			// model loading
			std::vector<Vertex> m_Vertices;
			std::vector<uint32_t> m_Indices;
			// load model function
			void LoadModel();
#else
			const std::vector<Vertex> m_Vertices =
			{
				{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

				{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
			};
			const std::vector<uint32_t> m_Indices = // now 32 to fit with model 
			{
				0, 1, 2, 2, 3, 0,
				4, 5, 6, 6, 7, 4
			};
#endif
			VkBuffer m_VertexBuffer; // does not depend on swap chain 
			VkDeviceMemory m_VertexBufferMemory; // stores handle to buffer memory
			VkBuffer m_IndexBuffer; // handle to index buffer 
			VkDeviceMemory m_IndexBufferMemory; // handle to index memory
			std::vector<VkBuffer> m_UniformBuffers;
			std::vector<VkDeviceMemory> m_UniformBuffersMemory;
			VkDescriptorPool m_DescriptorPool;
			std::vector<VkDescriptorSet> m_DescriptorSets;
			// texture mapping stuff
			// mipmap generation data
			uint32_t m_MipLevels; // "mip chain"
			VkImage m_TextureImage;
			VkDeviceMemory m_TextureImageMemory;
			VkImageView m_TextureImageView;
			VkSampler m_TextureSampler;
			// depth buffer data
			VkImage m_DepthImage;
			VkDeviceMemory m_DepthImageMemory;
			VkImageView m_DepthImageView;

			// multisampling variables (use the image, memory and view to sample the data in an off screen buffer)
			VkSampleCountFlagBits m_MsaaSamples;
			VkImage m_ColorImage;
			VkDeviceMemory m_ColorImageMemory;
			VkImageView m_ColorImageView;
		};
	}
}
