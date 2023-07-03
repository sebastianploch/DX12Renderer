// Copyright (c) 2023 Sebastian Ploch
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
