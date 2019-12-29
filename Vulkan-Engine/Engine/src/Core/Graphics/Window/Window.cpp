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
			vkDestroyDevice(m_LogicalDevice, nullptr); // clean logical device 
			if (s_EnableValidationLayers) 
			{
				DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugCallbackMessenger, nullptr); // clean layers debugger
			}			
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
			InitVulkanPhysicalDevice();
			InitVulkanLogicalDevice();
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
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();

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
				if (IsGraphicsVulkanCompatible(device)) 
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
			QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

			// specify the queues to be created
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
			queueCreateInfo.queueCount = 1;
			static const float queuePriority = 1.0f;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			// specify features of device being used
			VkPhysicalDeviceFeatures deviceFeatures = {}; //TODO: Come back to this

			// create the logical device info
			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = &queueCreateInfo;
			createInfo.queueCreateInfoCount = 1;
			createInfo.pEnabledFeatures = &deviceFeatures;
			
			createInfo.enabledExtensionCount = 0;
			if (s_EnableValidationLayers) 
			{
				// enabled layer count and enabled layer names aren't used in new specs of vulkan (Set them for backwards compatibility)
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
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
