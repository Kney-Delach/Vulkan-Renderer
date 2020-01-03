/***************************************************************************
 * Filename		: Window.h
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Windowing abstraction for this engine.
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
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN // includes the vulkan header 
#include <GLFW/glfw3.h>

#include "Core/Events/Event.h"

#include "Shaders/Shader.h"
#include "Shaders/Vertex.h"

namespace Vulkan_Engine
{
	class WindowResizeEvent;
	// forward declarations 
	class Timestep; 

	using EventCallbackFunction = std::function<void(Event&)>;

	namespace Graphics
	{
		struct WindowProperties
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool FramebufferResized; 			// Explicit handling of framebuffer resizing 
			
			WindowProperties(const std::string& title = "Vulkan-Engine", unsigned int width = 800, unsigned int height = 600)
				: Title(title), Width(width), Height(height), FramebufferResized(false) {}

			WindowProperties(const WindowProperties& copy)
			{
				Title = copy.Title;
				Width = copy.Width;
				Height = copy.Height;
				FramebufferResized = copy.FramebufferResized;
			}
		};

		struct WindowData
		{
			WindowProperties Properties;
			bool VSync;
			EventCallbackFunction EventCallback;
		};
		
		class Window
		{
		public:
			Window();
			~Window();
		public:
			void OnUpdate(const Timestep deltaTime);
			void Cleanup();
			_NODISCARD GLFWwindow* GetWindow() const { return m_Window; }
			void SetEventCallback(const EventCallbackFunction& callback);
			void SetVSync(const bool enabled);
		private:
			void InitWindow();
			// glfw 
			void InitGLFW(std::vector<const char*>& extensions);
			void SetGLFWCallbacks();
			void SetGLFWConfigurations();
			// vulkan 
			void VerifyVulkanExtensionsAvailable(std::vector<const char*>& extensions) const;
			void InitVulkan();
			void CreateVulkanInstance();
			void CreateVulkanWindowSurface(); 
			void CreateVulkanDebugMessenger();
			std::vector<const char*> GetAllRequiredExtensions() const;
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
			void RenderFrame();
			///////////////////////////////
			//// Everything that has to do with swap chain recreation
			///////////////////////////////
			void CleanupSwapChain();
			void RecreateSwapChain();
			///////////////////////////////
			// Vertex buffer data
			///////////////////////////////
			void CreateVertexBuffer();  // does not depend on swap chain 
		private:
			//todo: abstract the window, and vk instances / device into a structure of rendering context
			// glfw and mindow variables 
			GLFWwindow* m_Window = nullptr;
			WindowData m_WindowData;
			// vulkan stuff
			VkInstance m_VkInstance;
			VkDebugUtilsMessengerEXT m_DebugCallbackMessenger;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice;
			VkQueue m_GraphicsQueueHandle; // handle for graphics queue
			VkQueue m_PresentQueueHandle; // handle for presentation queue
			VkSurfaceKHR m_WindowSurface;// window surface (create directly after instance creation as can affect physical device)
			VkSwapchainKHR m_SwapChain;
			std::vector<VkImage> m_SwapChainImages;
			std::vector<VkImageView> m_SwapChainImageViews; // describes how to access an image, and which part of the image to access.
			VkFormat m_SwapChainImageFormat;
			VkExtent2D m_SwapChainExtent;
			VkRenderPass m_RenderPass;
			VkPipelineLayout m_PipelineLayout;
			VkPipeline m_GraphicsPipeline;
			std::vector<VkFramebuffer> m_SwapChainFramebuffers;
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
			//////////////////////////////////////////////// (each frame should have its own) 
			std::vector<VkSemaphore> m_ImageAvailableSemaphores; // signal image has been acquired & ready for rendering
			std::vector<VkSemaphore> m_RenderFinishedSemaphores; // signal that rendering has finished & presentation can happen
			std::vector<VkFence> m_InFlightFences; // used for cpu <-> gpu synchronization
			std::vector<VkFence> m_ImagesInFlight; // used to track for each swap chain image, if a frame in flight is currently using it
			size_t m_CurrentFrame = 0; // frame index used to keep track of the current frame for correct semaphore usage
			//////////////////////////////////////////////////
			//// Vertex buffer related variables
			////TODO: Abstract this data as in Exalted.
			//////////////////////////////////////////////////
			inline static const std::vector<Vertex> m_Vertices = 
			{
				{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
				{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
			};
			VkBuffer m_VertexBuffer; // does not depend on swap chain 
			VkDeviceMemory m_VertexBufferMemory; // stores handle to buffer memory 
		};
	}
}