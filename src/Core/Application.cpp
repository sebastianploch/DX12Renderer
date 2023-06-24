#include "Application.h"
#include <format>

void Application::Initialise()
{
	int i = 5;
	std::format("Test %i", i);
}
