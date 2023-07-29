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
	g_Timer.Tick([this](float DeltaTime) -> void
	{
		Application::Update(DeltaTime);
	});

	Render();
}

void Application::Update(float DeltaTime)
{
	CalculateFrameStats();
}

void Application::Render()
{
}

void Application::CalculateFrameStats()
{
	static int64 frameCount = 0;
	static float timeElapsed = 0.0;

	++frameCount;

	if (static_cast<float>(g_Timer.GetTotalTime()) - timeElapsed < 1.0f)
	{
		return;
	}

	const float frameRate = static_cast<float>(frameCount);
	const float frameProcessTime = 1000.f / frameRate;

	const std::wstring windowText = std::format(L"{}	FPS: {} ({}ms)", s_ApplicationName, static_cast<int32>(frameRate), frameProcessTime);
	SetWindowText(RHI::m_WindowInfo.m_Window, windowText.c_str());

	frameCount = 0;
	timeElapsed += 1.f;
}
