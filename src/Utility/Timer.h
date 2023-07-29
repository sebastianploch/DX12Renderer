// Copyright (c) 2023 Sebastian Ploch
#pragma once

class Timer
{
public:
	Timer();

	void Reset();
	void Start();
	void Stop();
	void Tick(const std::function<void(double)>& TickFunction = nullptr);

	[[nodiscard]] double GetTotalTime() const;
	[[nodiscard]] double GetDeltaTime() const;

private:
	double m_SecondsPerCount {-1.0};
	double m_DeltaTime {0.0};

	int64 m_StartTime {0};
	int64 m_PausedTime {0};
	int64 m_StopTime {0};

	int64 m_CurrentTime {0};
	int64 m_PreviousTime {0};

	bool m_Stopped {false};
};
