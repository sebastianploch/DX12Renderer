// Copyright (c) 2023 Sebastian Ploch
#include "Application.h"

#include "RHI.h"
#include "Utility/Timer.h"

static Timer g_Timer;

Application::Application()
{
	g_Timer.Reset();
	Initialise();
}

void Application::Initialise()
{
	RHI::Initialise();
}

void Application::Tick()
{
	g_Timer.Tick();
}
