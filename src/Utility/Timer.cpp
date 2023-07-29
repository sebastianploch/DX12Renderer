// Copyright (c) 2023 Sebastian Ploch
#include "Timer.h"

Timer::Timer()
{
	int64 countsPerSec = -1;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
	m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

void Timer::Reset()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_CurrentTime));
	m_PreviousTime = m_CurrentTime;

	m_StartTime = m_CurrentTime;
	m_StopTime = 0;

	m_Stopped = false;
}

void Timer::Start()
{
	// Already started
	if (!m_Stopped)
	{
		return;
	}

	int64 startTime = -1;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

	m_PausedTime += (startTime - m_StopTime);
	m_PreviousTime = startTime;

	m_StopTime = 0;
	m_Stopped = false;
}

void Timer::Stop()
{
	// Already stopped
	if (m_Stopped)
	{
		return;
	}

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_CurrentTime));
	m_StopTime = m_CurrentTime;

	m_Stopped = true;
}

void Timer::Tick(const std::function<void(float)>& TickFunction /*= nullptr*/)
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_CurrentTime));

	m_DeltaTime = static_cast<double>((m_CurrentTime - m_PreviousTime)) * m_SecondsPerCount;

	m_PreviousTime = m_CurrentTime;

	// Ensure that delta time is positive
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}

	if (TickFunction)
	{
		TickFunction(static_cast<float>(m_DeltaTime));
	}
}

float Timer::GetTotalTime() const
{
	if (m_Stopped)
	{
		// Return corrected time that doesn't include current stop
		return static_cast<float>((m_StopTime - m_PreviousTime) - m_StartTime) * static_cast<float>(m_SecondsPerCount);
	}

	return static_cast<float>((m_CurrentTime - m_PausedTime) - m_StartTime) * static_cast<float>(m_SecondsPerCount);
}

float Timer::GetDeltaTime() const
{
	return static_cast<float>(m_DeltaTime);
}
