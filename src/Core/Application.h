// Copyright (c) 2023 Sebastian Ploch
#pragma once


class Application
{
public:
	Application(const wchar_t* ApplicationName);

	void Initialise();

	void Tick();

	void Update(float DeltaTime);
	void Render();

private:
	void CalculateFrameStats();

public:
	const wchar_t* m_ApplicationName;
};
