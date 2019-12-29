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
			vkDestroyInstance(m_VkInstance, nullptr);

			glfwDestroyWindow(m_Window);
			glfwTerminate();
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

		//TODO: Abstract vulkan to rendering context? 
		void Window::InitVulkan()
		{
			VK_CORE_DEBUG("[Graphics System]: Initializing Vulkan");
			CreateVulkanInstance(); // create a vulkan instance 
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

			// required
			// tell driver which global (entire program) extensions & validation layers to use
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// extensions 
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			VK_CORE_TRACE("[Graphics System::Window::CreateVulkanInstance]: GLFW Extensions List");
			for (uint32_t i = 0; i < glfwExtensionCount; i++)
			{
				VK_CORE_TRACE("-> {0}", *(glfwExtensions + i));
			}
			createInfo.enabledExtensionCount = glfwExtensionCount;
			createInfo.ppEnabledExtensionNames = glfwExtensions;
			// validation layers 
			createInfo.enabledLayerCount = 0;
			// Creates the vulkan instance 
			const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
			VK_CORE_ASSERT(result == VK_SUCCESS, result);
		}

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
