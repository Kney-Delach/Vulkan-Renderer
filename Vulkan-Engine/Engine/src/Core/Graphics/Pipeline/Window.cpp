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
			vkDestroyPipeline(m_LogicalDevice, m_GraphicsPipeline, nullptr); // destroy the graphics pipeline 
			vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr); // pipeline layout  (data passed to shaders)
			vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr); // destroy the render pass 
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
			CreateGraphicsRenderPass();
			CreateGraphicsPipeline();
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
			////todo: This is an example of how the extension would be implemented without glfw (win32 specific)
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

		// tell Vulkan about the framebuffer attachments
		// color and depth buffers there will be, how many samples to use for each of them and how their contents should be handled throughout the rendering operations
		void Window::CreateGraphicsRenderPass()
		{
			////////////////////////////////////////////
			// 1. Attachment Description
			////////////////////////////////////////////
			
			// assign single color buffer attachment
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = m_SwapChainImageFormat; // represented by 1 of the images in the swap chain 
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Implement multisampling in the future
			// determine what to do with the data in the attachment before rendering and after rendering
			// Load Op:
			// 1. VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
			// 2. VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
			// 3. VK_ATTACHMENT_LOAD_OP_DONT_CARE : Existing contents are undefined; we don't care about them
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear the framebuffer to black before drawing a new frame
			// StoreOp
			// 1. VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
			// 2. VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store it, as want to render to screen
			//TODO: In the future -> Stencil Buffer Attachment
			// The loadOp and storeOp apply to color and depth data, and stencilLoadOp / stencilStoreOp apply to stencil data.
			//  Our application won't do anything with the stencil buffer, so the results of loading and storing are irrelevant.

			// Textures & Framebuffer pixels
			//Note: images need to be transitioned to specific layouts that are suitable for the operation that they're going to be involved in next.
			// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
			// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : Images to be presented in the swap chain
			// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : Images to be used as destination for a memory copy operation
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // (undefined = don't care) layout before render pass
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // (ready for presentation) automatic transition to this layout post render pass

			////////////////////////////////////////////
			// 2. Subpasses & attachment references (currently single subpass) 
			////////////////////////////////////////////
			// A single render pass can consist of multiple subpasses.
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAttachmentReference.html
			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0; // which attachment (indexed)
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			
			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // explicit as compute passes are possible 
			subpass.colorAttachmentCount = 1; // index of the attachment in this array is directly referenced from the fragment shader with the layout(location = 0) out vec4 outColor directive!
			subpass.pColorAttachments = &colorAttachmentRef;

			// The following other types of attachments can be referenced by a subpass:
			// 
			// pInputAttachments: Attachments that are read from a shader
			// pResolveAttachments : Attachments used for multisampling color attachments
			// pDepthStencilAttachment : Attachment for depthand stencil data
			// pPreserveAttachments : Attachments that are not used by this subpass, but for which the data must be preserved

			// create the render pass 
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;

			if (vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateGraphicsRenderPass]: Failed to create Render pass!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}
		}

		void Window::CreateGraphicsPipeline()
		{
			//TODO: Maybe something like this? Shader::SetActiveLogicalDevice(m_LogicalDevice); 
			const Shader vertexShader("Resources/Shaders/Vert.spv", &m_LogicalDevice);
			const Shader fragmentShader("Resources/Shaders/Frag.spv", &m_LogicalDevice);

			// create vertex shader info 
			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; 
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // set shader type as vertex
			vertShaderStageInfo.module = vertexShader.GetShaderModule();; // set shader data 
			vertShaderStageInfo.pName = "main"; // function to invoke 
			vertShaderStageInfo.pSpecializationInfo = nullptr; // used to set constant data for branch prediction

			// create fragment shader info 
			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragmentShader.GetShaderModule();;
			fragShaderStageInfo.pName = "main";
			vertShaderStageInfo.pSpecializationInfo = nullptr; // used to set constant data for branch prediction

			// used to reference creation structs during pipeline creation stage 
			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			////////////////////////////////////////////
			// 1. Vertex Input
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineVertexInputStateCreateInfo.html
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {}; // format of the vertex data to pass to vertex shader 
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			// bindings: spacing between data and wheter data is per-vertex or per-instance
			vertexInputInfo.vertexBindingDescriptionCount = 0; 
			vertexInputInfo.pVertexBindingDescriptions = nullptr; // Point to array of structs that contain the descriptions of the data
			// attributes: /type of attributes and which binding to load them from & at which offset 
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr; // same as bindings descriptions 

			////////////////////////////////////////////
			// 2. Input Assembly 
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineInputAssemblyStateCreateInfo.html
			// [1]: what kind of geometry will be drawn (stored in topology)
			// VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
			// VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
			// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
			// [2]: Is primitive restart enabled
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE; // set to true to break up lines and triangles in strip

			////////////////////////////////////////////
			// 3. Viewports & Scissors 
			////////////////////////////////////////////
			// assign a window viewport to render entire window to
			VkViewport viewport = {};
			viewport.x = 0.0f; // bottom left x
			viewport.y = 0.0f; // bottom left y
			viewport.width = (float)m_SwapChainExtent.width; // top right x
			viewport.height = (float)m_SwapChainExtent.height; // top right y
			viewport.minDepth = 0.0f; // framebuffer value range min
			viewport.maxDepth = 1.0f; // framebuffer value range max

			// define scissorm for entire window 
			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = m_SwapChainExtent;

			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineViewportStateCreateInfo.html
			// It is possible to use multiple viewports and scissor rectangles on some graphics cards, so its members reference an array of them. Using multiple requires enabling a GPU feature (see logical device creation).
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1; 
			viewportState.pViewports = &viewport; 
			viewportState.scissorCount = 1;  
			viewportState.pScissors = &scissor;

			////////////////////////////////////////////
			// 4. Rasterizer
			////////////////////////////////////////////
			// performs depth testing, face culling, scissor testing, can output wireframe or full geometry.
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE; // if true, fragments beyond near and far planes are clamped, rather than discarded (useful in shadow maps -> requires GPU Feature)
			rasterizer.rasterizerDiscardEnable = VK_FALSE; // if true, geometry never passes through rasterizer -> disables output to framebuffer
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // determines how fragm,ents are generated for geometry (FILL / LINE / POINT) -> any other mode than fill require gpu feature
			rasterizer.lineWidth = 1.0f; // thickness of lines (number of fragments) (if > 1.0, requires wideLines GPU feature)
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // type of face culling 
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // vertex ordering
			// alters depth values by adding constants / biasing based on fragment's slope (USAGE:Shadow mapping)
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

			////////////////////////////////////////////
			// 5. Multisampling - (anti-aliasing) 
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineMultisampleStateCreateInfo.html
			// Because it doesn't need to run the fragment shader multiple times if only one polygon maps to a pixel,
			// it is significantly less expensive than simply rendering to a higher resolution and then down-scaling.
			// Enabling it requires enabling a GPU feature.
			// DISABLED ~ Will continue later
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f; // Optional
			multisampling.pSampleMask = nullptr; // Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE; // Optional

			////////////////////////////////////////////
			// 6. Depth and Stencil Testing  
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineDepthStencilStateCreateInfo.html
			// VkPipelineDepthStencilStateCreateInfo
			
			////////////////////////////////////////////
			// 7. Color blending (currently disabling both modes -> no blending occurs) 
			////////////////////////////////////////////
			// post fragment shader
			// 1. Mix the old and new value to produce a final color
			// 2. Combine the old and new value using a bitwise operation
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineColorBlendAttachmentState.html // config per framebuffer
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineColorBlendStateCreateInfo.html // global color blend settings 
			// configure color blending using mixing old and new values to produce a final color (method 1)
			//  The most common way to use color blending is to implement alpha blending,
			// where we want the new color to be blended with the old color based on its opacity
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {}; // attachment per framebuffer 
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE; // if false -> unmodified, otherwise -> mixing operation occurs &'d with colorWriteMask to determine which colors to pass 
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
			 
			//TODO: Example -> alpha opacity blending
			//colorBlendAttachment.blendEnable = VK_TRUE;
			//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			//TODO: Example

			VkPipelineColorBlendStateCreateInfo colorBlending = {}; 
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE; // if using combination (method 2) of blending, then set to true (bitwise combination)
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // sets the bitwise operation in combination approach
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment; // references array of structures for all framebuffers 
			colorBlending.blendConstants[0] = 0.0f; // Optional blend constants
			colorBlending.blendConstants[1] = 0.0f; // "" 
			colorBlending.blendConstants[2] = 0.0f; // ""
			colorBlending.blendConstants[3] = 0.0f; // ""

			////////////////////////////////////////////
			// 8. Dynamic State 
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineDynamicStateCreateInfo.html
			// A limited amount of the state that we've specified in the previous structs can actually be changed without recreating the pipeline.
			// size of the viewport, line width and blend constants.
			// VkPipelineDynamicStateCreateInfo
			// This will cause the configuration of these values to be ignored and user will be required to specify the data at drawing time
			//TODO: Return to this in the future, can be substituted by nullptr if doesn't exist 
			//VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
			//VkPipelineDynamicStateCreateInfo dynamicState = {};
			//dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			//dynamicState.dynamicStateCount = 2;
			//dynamicState.pDynamicStates = dynamicStates;

			////////////////////////////////////////////
			// 9. Pipeline Layout 
			////////////////////////////////////////////
			// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineLayout.html
			// uniform values (passed to shaders) must be specified during pipeline creation (VkPipelineLayout)
			// Push Constants: ways to pass dynamic data to shaders
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 0; // Optional
			pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

			if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateGraphicsPipeline]: Failed to create pipeline layout!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}

			////////////////////////////////////////////
			// 10. Create the pipeline 
			////////////////////////////////////////////
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = nullptr; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; // Optional
			pipelineInfo.layout = m_PipelineLayout;
			pipelineInfo.renderPass = m_RenderPass;
			pipelineInfo.subpass = 0; // only running a single pass
			// compatibility requirements below 
			// https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
			// These values are only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag is also specified in the flags field of VkGraphicsPipelineCreateInfo.
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			//todo: Notice definition and usages of pipeline cache, implement these in the future 
			// A pipeline cache can be used to store and reuse data relevant to pipeline creation across multiple calls to vkCreateGraphicsPipelines and even across program executions if the cache is stored to a file.
			if (vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Window::CreateGraphicsPipeline]: Failed to create pipeline pipeline!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
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
