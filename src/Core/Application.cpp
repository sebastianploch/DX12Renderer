#include "Application.h"
#include "RHI.h"

Application::Application()
{
	Initialise();
}

void Application::Initialise()
{
	RHI::Initialise();
}
