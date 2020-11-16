#include <iostream>
#include "Application.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	try
	{
		Application* app = new Application();

		if (app->Startup())
		{
			std::cout << "Application creation successful!" << std::endl;

			app->Run();
		}	

			delete app;
			app = nullptr;

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}