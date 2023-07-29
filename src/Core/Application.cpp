// Copyright (c) 2023 Sebastian Ploch
#include "Application.h"

#include "RHI.h"
#include "Utility/Timer.h"

Timer g_Timer;

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
	g_Timer.Tick([this](double DeltaTime) -> void
	{
		Application::Update(DeltaTime);
	});

	Render();
}

void Application::Update(double DeltaTime)
{
}

void Application::Render()
{
}
