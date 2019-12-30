/***************************************************************************
 * Filename		: Window.cpp
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
#include "vkepch.h"
#include "Window.h"
#include "Core/Logger/Log.h"
#include "Core/Timers/Timestep.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Graphics/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		Window::Window()
		{
			InitWindow();
		}

		Window::~Window()
		{
		}

		void Window::OnUpdate(const Timestep deltaTime)
		{
			glfwPollEvents();
		}

		void Window::Cleanup()
		{
			for (auto imageView : m_SwapChainImageViews)  // destroy all the image views 
			{
				vkDestroyImageView(m_LogicalDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
			vkDestroyDevice(m_LogicalDevice, nullptr); // clean logical device 
			if (s_EnableValidationLayers) 
			{
				DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugCallbackMessenger, nullptr); // clean layers debugger
			}
			vkDestroySurfaceKHR(m_VkInstance, m_WindowSurface, nullptr); // destroy the window surface (before the instance) 
			vkDestroyInstance(m_VkInstance, nullptr); // clean active vulkan instance 
			glfwDestroyWindow(m_Window); // clean glfw window 
			glfwTerminate(); // terminate window 
		}

		void Window::SetEventCallback(const EventCallbackFunction& callback)
		{
			m_WindowData.EventCallback = callback;
		}

		void Window::SetVSync(const bool enabled)
		{
			if (enabled)
				glfwSwapInterval(1);
			else
				glfwSwapInterval(0);
			m_WindowData.VSync = enabled;
		}

		void Window::InitWindow()
		{
			VK_CORE_DEBUG("[Graphics System]: Initializing GLFW");
			m_WindowData.Properties = WindowProperties();
			std::vector<const char*> requiredExtensions; // used to get glfw's extension requirements
			InitGLFW(requiredExtensions);
			VerifyVulkanExtensionsAvailable(requiredExtensions);
			InitVulkan(); // if the extensions are available, then go ahead and initialize the vulkan context
		}

		// ------------------------------ GLFW Setup ------------------------------

		void Window::InitGLFW(std::vector<const char*>& extensions)
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no opengl context
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // because resizing is more complex			
			m_Window = glfwCreateWindow(m_WindowData.Properties.Width, m_WindowData.Properties.Height, m_WindowData.Properties.Title.c_str(), nullptr, nullptr);
			VK_ASSERT(m_Window != nullptr, "GLFW Window Initialization failed!")
			glfwSetWindowUserPointer(m_Window, &m_WindowData);
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// add glfw extensions to required extensions vector
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			VK_ASSERT(glfwExtensions != NULL, "GLFW Extension list returned empty, something somewhere went terrible wrong!")
			for(uint32_t i = 0; i < glfwExtensionCount; ++i)
			{
				extensions.push_back(glfwExtensions[i]);
			}
			SetGLFWConfigurations();
			SetGLFWCallbacks();
		}

		void Window::VerifyVulkanExtensionsAvailable(std::vector<const char*>& extensions) const
		{
			auto availableExtensions = GetVulkanData<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, "");
			std::vector<const char*> availableExtNames;
			std::transform(availableExtensions.begin(), availableExtensions.end(), std::back_inserter(availableExtNames),
				[](const VkExtensionProperties& prop)
			{
				VK_CORE_TRACE("[Graphics System::Window::InitWindow::Vulkan_Extension_AVAILABLE]:: {0}", prop.extensionName);
				return prop.extensionName;
			});
			for (auto extension : extensions) //TODO: Add more extension error handling mechanisms for runtime....
			{ 
				VK_ASSERT(std::find(std::begin(availableExtNames), std::end(availableExtNames), extension) == availableExtNames.end(), "System is missing GLFW required Vulkan extensions!");
			}
		}

		// ------------------------------ Vulkan Initialization ------------------------------

		//TODO: Abstract vulkan to rendering context? 
		void Window::InitVulkan()
		{
			VK_CORE_DEBUG("[Graphics System]: Initializing Vulkan");
			CreateVulkanInstance();
			CreateVulkanDebugMessenger();
			CreateVulkanWindowSurface();
			InitVulkanPhysicalDevice();
			InitVulkanLogicalDevice();
			CreateVulkanSwapChain();
			CreateVulkanImageViews();
		}

		void Window::CreateVulkanInstance()
		{
			if(s_EnableValidationLayers && !AreValidationLayersAvailable())
			{
				VK_CORE_CRITICAL("[Graphics System]: Validation layers requested, but not available!");
				throw std::runtime_error("VALIDATION LAYERS: requested, but not available!");
			}

			// optional
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // type of the info 
			appInfo.pApplicationName = "Vulkan-Engine";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "No Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;
			appInfo.pNext = nullptr;

			//! Required
			// tell driver which global (entire program) extensions & validation layers to use
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// extensions 
			auto extensions = GetAllRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			// initialize debug messenger for creation / destruction
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

			// validation layers
			if(s_EnableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = s_ValidationLayers.data();

				PopulateDebugMessengerCreateInfo(debugCreateInfo, &m_WindowData);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;

			}
			
			// Creates the vulkan instance 
			const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
			VK_CORE_ASSERT(result == VK_SUCCESS, result);
		}

		void Window::CreateVulkanWindowSurface()
		{
			//////////////////////////////////////////////////////////////////
			///todo: This is an example of how the extension would be implemented without glfw (win32 specific)
			//VkWin32SurfaceCreateInfoKHR createInfo = {};
			//createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			//createInfo.hwnd = glfwGetWin32Window(window);
			//createInfo.hinstance = GetModuleHandle(nullptr);
			//if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
			//throw std::runtime_error("failed to create window surface!");
			//}
			//////////////////////////////////////////////////////////////////

			// the glfw version of assigning a window surface (m_WindowSurface)
			if (glfwCreateWindowSurface(m_VkInstance, m_Window, nullptr, &m_WindowSurface) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateVulkanWindowSurface]:Failed to create window surface!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}
		}

		// ------------------------------ Vulkan Debug Settup ------------------------------

		void Window::CreateVulkanDebugMessenger()
		{
			if(!s_EnableValidationLayers) return;
			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			PopulateDebugMessengerCreateInfo(createInfo, &m_WindowData);
			if (CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugCallbackMessenger) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateVulkanDebugMessenger]: Failed to set up debug messenger!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}			
		}
		
		std::vector<const char*> Window::GetAllRequiredExtensions() const
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			if (s_EnableValidationLayers)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // add debugging utility extension
			}
			// Extension existence implied by availability of validation layers...
			VK_CORE_TRACE("[Graphics System::Window::CreateVulkanInstance]: ALL Required Extensions List:"); 
			for (uint32_t i = 0; i < extensions.size(); i++)
			{
				VK_CORE_TRACE("-> {0}", extensions[i]);
			}
			return extensions;
		}

		void Window::InitVulkanPhysicalDevice()
		{
			const auto availableDevices = GetVulkanData<VkPhysicalDevice>(vkEnumeratePhysicalDevices, m_VkInstance);
			if(availableDevices.empty())
			{
				static const std::string message = "[GraphicsSystem::Window::InitVulkanPhysicalDevice]: Failed to find GPUs with Vulkan support!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}

			//TODO: change this to give user options, or choose device with the best score
			for (const auto& device : availableDevices) 
			{
				if (IsGraphicsVulkanCompatible(device, m_WindowSurface)) 
				{
					m_PhysicalDevice = device;
					break;
				}
			}
			
			if (m_PhysicalDevice == VK_NULL_HANDLE) 
			{
				static const std::string message = "[GraphicsSystem::Window::InitVulkanPhysicalDevice]: Failed to find a suitable GPU!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}

		}
		

		void Window::InitVulkanLogicalDevice()
		{
			// m_LogicalDevice
			QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_WindowSurface);

			// specify the queues to be created (we require multiple of these to create a queue for each family)
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
			static const float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies) // for all family queues 
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}
			
			// specify features of device being used
			VkPhysicalDeviceFeatures deviceFeatures = {}; //TODO: Come back to this

			// create the logical device info
			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			// setup swap chain extensions
			createInfo.enabledExtensionCount = s_DeviceExtensions.size();
			createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

			if (s_EnableValidationLayers) 
			{
				// enabled layer count and enabled layer names aren't used in new specs of vulkan (Set them for backwards compatibility)
				createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
			}
			else 
			{
				createInfo.enabledLayerCount = 0;
			}

			// create the device 
			if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)  //TODO: Make a macro to check vs_success result
			{
				static const std::string message = "[GraphicsSystem::Window::InitVulkanLogicalDevice]: Failed to create logical device!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}
			vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamily.value(), 0, &m_GraphicsQueueHandle);
			vkGetDeviceQueue(m_LogicalDevice, indices.PresentFamily.value(), 0, &m_PresentQueueHandle);
		}

		void Window::CreateVulkanSwapChain()
		{
			const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice, m_WindowSurface);
			const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
			const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
			const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

			// decide how many images to have in the swap chain
			uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1; // minimum number of images required for swap chain to function
			if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
			{
				imageCount = swapChainSupport.Capabilities.maxImageCount;
			}

			// create the swap chain
			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = m_WindowSurface; // specify the surface 

			// specify details of swap chain images
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1; // amount of layers each image consists of  (1 unless developing stereoscopic 3D)
			// VK_IMAGE_USAGE_TRANSFER_DST_BIT  for post processing (for example) 
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // ([currently -> color attachment] | depth attachment etc....) bit field: what kind of operations will use images in swap chain for

			// specify how to handle swap chain images across multiple queue families
			// draw on images from graphics queue, then submit using presentation queue
			// 2 ways to handle images accessed from multiple queues:
			// 1. EXCLUSIVE [BEST PERFORMANCE]: image owned by one queue family at a time, and ownership must be explicitly transferred before using it in another queue family. 
			// 2. CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers.			
			const QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_WindowSurface);
			const uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
			if (indices.GraphicsFamily != indices.PresentFamily) 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // require advanced spec of which queue families will share ownership
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0; // Optional
				createInfo.pQueueFamilyIndices = nullptr; // Optional
			}
			// apply certain transforms to images in the swap chain if supported (supportedTransforms in Capbilities) 
			createInfo.preTransform = swapChainSupport.Capabilities.currentTransform; // specifies no transform (use supported transforms to search for available ones) 

			// specify if the alpha channel should be used for blending with other windows in the window system. 
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignores alpha channel

			// 
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE; // do we care about the color of pixels that are obscured? (other windows infront?) (only turn off when need predictability)

			// 
			createInfo.oldSwapchain = VK_NULL_HANDLE; //TODO: Come back to this when dealing with swapchain recreations (when swapchains become invalid)
			if (vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateVulkanSwapChain]: Failed to create swap chain!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}

			m_SwapChainImages = GetVulkanData<VkImage>(vkGetSwapchainImagesKHR,m_LogicalDevice, m_SwapChain);
			m_SwapChainImageFormat = surfaceFormat.format;
			m_SwapChainExtent = extent;
		}

		void Window::CreateVulkanImageViews()
		{
			m_SwapChainImageViews.resize(m_SwapChainImages.size());
			for (size_t i = 0; i < m_SwapChainImages.size(); i++)
			{
				// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkImageViewCreateInfo.html
				// set an info for each chain image before creating it 
				VkImageViewCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
				createInfo.image = m_SwapChainImages[i];
				// how should the image be interpreted 
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;   // 1D, 2D, 3D, cubemaps
				createInfo.format = m_SwapChainImageFormat;
				// Swizzle color channels -> maps the rgba components to specific channels 
				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				// describes what the image's purpose is & which part of the image should be accessed.
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // use image as a color target
				createInfo.subresourceRange.baseMipLevel = 0; // no mipmap generation 
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				//If you were working on a stereographic 3D application, then you would create a swap chain with multiple layers.You could then create multiple image views for each image representing the views for the leftand right eyes by accessing different layers.
				createInfo.subresourceRange.layerCount = 1; // image will have a single layer

				// create the image view
				if (vkCreateImageView(m_LogicalDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) 
				{
					static const std::string message = "[GraphicsSystem::Window::CreateVulkanImageViews]: Failed to create image view!";
					VK_CORE_CRITICAL(message);
					throw std::runtime_error(message);		
				}
			}
		}

		// ------------------------------ GLFW Settings ------------------------------

		void Window::SetGLFWCallbacks()
		{
			// ------------------------------ Window ------------------------------
			glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& windowData = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				WindowClosedEvent event;
				windowData.EventCallback(event);
			});
		}

		void Window::SetGLFWConfigurations()
		{
			SetVSync(true);
		}
	}
}
