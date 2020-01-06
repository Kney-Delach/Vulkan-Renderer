/***************************************************************************
 * Filename		: Shader.frag
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

layout(binding = 1) uniform sampler2D a_Texture;

layout(location = 0) in vec3 v_FragColor;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

void main() 
{
    o_Color = vec4(v_FragColor * texture(a_Texture, v_TexCoord).rgb, 0.75);
}