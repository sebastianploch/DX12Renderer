// Copyright (c) 2023 Sebastian Ploch
#pragma once


class Application
{
public:
	Application();

	void Initialise();

	void Tick();

	void Update(float DeltaTime);
	void Render();

private:
	void CalculateFrameStats();

private:
	inline static std::wstring s_ApplicationName {L"DX12Renderer"};
};
