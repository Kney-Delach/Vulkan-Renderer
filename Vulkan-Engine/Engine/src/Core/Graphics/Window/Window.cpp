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

namespace Vulkan_Engine
{
	namespace Graphics
	{
		Window::Window()
		{
			InitWindow();
			InitVulkan();
			CreateVulkanInstance();
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
			glfwInit();			
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no opengl context
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // because resizing is more complex			
			m_Window = glfwCreateWindow(m_WindowData.Properties.Width, m_WindowData.Properties.Height, m_WindowData.Properties.Title.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(m_Window, &m_WindowData);
			SetGLFWConfigurations();
			SetGLFWCallbacks();
		}

		void Window::InitVulkan()
		{
			VK_CORE_DEBUG("[Graphics System]: Initializing Vulkan");
			CreateVulkanInstance(); // create a vulkan instance 
		}

		void Window::CreateVulkanInstance()
		{
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

			VK_CORE_TRACE("[Graphics System]: GLFW Extensions");
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
			VK_CORE_ASSERT(result == VK_SUCCESS, result); //TODO: Create a translator function between VkResult and VK_LOGGER

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

		//void Window::GetAvailableExtensionDetails()
//{
//	uint32_t extensionCount = 0;
//	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//	std::vector<VkExtensionProperties> vulkanExtensions = std::vector<VkExtensionProperties>(extensionCount);

//	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vulkanExtensions.data());
//	m_ExtensionNames = std::vector<char*>(extensionCount);
//	int i = 0;
//	for (VkExtensionProperties vkp : vulkanExtensions)
//	{
//		m_ExtensionNames[i] = (vkp.extensionName);
//		i++;
//	}
//	VK_CORE_INFO("Available Vulkan Extensions");
//	for (const auto& extension : vulkanExtensions) {
//		VK_CORE_INFO("-> {0}",extension.extensionName);
//	}
//}
	}
}
