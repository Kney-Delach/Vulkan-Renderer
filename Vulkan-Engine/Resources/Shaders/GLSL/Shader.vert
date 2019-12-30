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

layout(location = 0) out vec3 v_FragColor;

vec2 m_Positions[3] = vec2[]
(
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 m_Colors[3] = vec3[]
(
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() 
{
    gl_Position = vec4(m_Positions[gl_VertexIndex], 0.0, 1.0);
    v_FragColor = m_Colors[gl_VertexIndex];
}