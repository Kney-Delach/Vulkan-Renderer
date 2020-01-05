/***************************************************************************
 * Filename		: Shader.vert
 * Name			: Ori Lazar
 * Date			: 30/12/2019
 * Description	: Used as a test shader in setting up an initial vulkan pipeline
     .---.
   .'_:___".
   |__ --==|
   [  ]  :[|
   |__| I=[|
   / / ____|
  |-/.____.'
 /___\ /___\
***************************************************************************/

#version 450
#extension GL_ARB_separate_shader_objects : enable

// https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// variables such as dvec3 (64 bit float), use 2 slots, so increase location by 2
//layout(location = 0) in dvec3 inPosition;
//layout(location = 2) in vec3 inColor;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
} ubo;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_FragColor;
layout(location = 1) out vec2 v_TexCoord;

void main() 
{
    gl_Position = ubo.Projection * ubo.View * ubo.Model * vec4(a_Position, 0.0, 1.0);
    v_FragColor = a_Color;
    v_TexCoord = a_TexCoord;
}