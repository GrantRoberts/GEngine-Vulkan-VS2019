#include <iostream>
#include "Application.h"

int main()
{
	Application* app = new Application();

	if (app->Startup())
	{
		std::cout << "Application creation successful!" << std::endl;

		app->Run();
	}

	system("pause");

	delete app;
	app = nullptr;

	return 0;
}