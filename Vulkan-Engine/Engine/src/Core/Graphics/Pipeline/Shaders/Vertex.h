#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <array>

namespace Vulkan_Engine
{
	namespace Graphics
	{
		struct Vertex
		{
			//TODO: Change the format type depending on size of types in struct
			glm::vec3 Position; 
			glm::vec3 Color;
			glm::vec2 TexCoord;

			// describes at which rate to load data from memory throughout the vertices
			// specifies the number of bytes between data entries, and whether to move
			// to the next data entry after each vertex or after each instance
			static VkVertexInputBindingDescription GetBindingDescription()
			{
				VkVertexInputBindingDescription bindingDescription = {};
				bindingDescription.binding = 0; // index of this binding description in the array of bindings 
				bindingDescription.stride = sizeof(Vertex); // number of bytes from one entry to the next
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // vertex || instance 
				return bindingDescription;
			}

			// two of these structures used (as using 2 attributes, POSITION & COLOR)
			// describes how to extract vertex attributes from a chunk of vertex data originating from a binding description.
			static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
			{
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

				// position attribute per-vertex description
				attributeDescriptions[0].binding = 0; // which binding the per vertex data comes from (binding 0 above)
				attributeDescriptions[0].location = 0; // location derivative of the input in the vertex shader (location = 0 in shader)
				// float: VK_FORMAT_R32_SFLOAT
				// vec2 : VK_FORMAT_R32G32_SFLOAT
				// vec3 : VK_FORMAT_R32G32B32_SFLOAT
				// vec4 : VK_FORMAT_R32G32B32A32_SFLOAT
				//todo: note, match the bit width
				// ivec2  : VK_FORMAT_R32G32_SINT,       a 2-component vector of 32-bit signed integers
				// uvec4  : VK_FORMAT_R32G32B32A32_UINT, a 4 - component vector of 32 - bit unsigned integers
				// double : VK_FORMAT_R64_SFLOAT,        a double - precision(64 - bit) float
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // byte size of attribute data 
				attributeDescriptions[0].offset = offsetof(Vertex, Position); // number of bytes since start of per-vertex data to read from

				// color attribute per-vertex description
				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, Color);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, TexCoord);
				
				return attributeDescriptions;
			}

			bool operator==(const Vertex& other) const
			{
				return Position == other.Position && Color == other.Color && TexCoord == other.TexCoord;
			}
		};
	}
}

namespace std
{
	template<> struct hash<Vulkan_Engine::Graphics::Vertex>
	{
		size_t operator()(Vulkan_Engine::Graphics::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.Position) 
				^ (hash<glm::vec3>()(vertex.Color) << 1)) >> 1)
				^ (hash<glm::vec2>()(vertex.TexCoord) << 1);
		}
	};
}