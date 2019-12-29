/***************************************************************************
 * Filename		: EntryPoint.cpp
 * Name			: Ori Lazar
 * Date			: 28/12/2019
 * Description	: Entry point for this engine.
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

#include "Core/Application.h"


int main()
{
	Vulkan_Engine::Application app;
	try
	{
		app.Run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}