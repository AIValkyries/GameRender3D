#include "stdafx.h"

#include "Timer.h"
#include "Debug.h"

void Timer::InitTimer()
{
	HighPerformanceTimerSupport = QueryPerformanceFrequency(&HighPerformanceFreq);
	Tick();
	startTime = realTime;
	timeSpeed = 1.0F;
	framesCounted = 0;
	startFrameCount = 0;
	preTime = float32(GetRealTime());
	fps = 0;
}

float32 Timer::Tick()
{
	float32 currentTime = GetRealTime();
	deltaTime = (currentTime - realTime) * timeSpeed;
	realTime = GetRealTime();

	return deltaTime;
}

float32 Timer::GetDeltaTime()
{
	return deltaTime;
}

float32 Timer::GetRealTime()
{
	LARGE_INTEGER nTime;
	BOOL queriedOK = QueryPerformanceCounter(&nTime);
	return (float32)(nTime.QuadPart * 1000 / HighPerformanceFreq.QuadPart);
}

float32 Timer::GetTime()
{
	return (realTime - startTime) * timeSpeed;
}

void Timer::SetTimerSpeed(float32 speed)
{
	timeSpeed = speed;
}

float32 Timer::GetTimerSpeed()
{
	return timeSpeed;
}

float32 Timer::CalculateFPS()
{
	++framesCounted;
	float32 millisecond = GetRealTime() - preTime;

	if (millisecond > 1500)
	{
		fps = (framesCounted * 1000) / (millisecond);
		preTime = GetRealTime();
		framesCounted = 0;
	}

	return fps;
}

uint32 Timer::StartClock()
{
	startFrameCount = GetRealTime();
	return (uint32)startFrameCount;
}

uint32 Timer::WaitClock(uint32 frame)
{
	while (GetRealTime() - startFrameCount < frame)
	{

	}
	return (uint32)GetRealTime();
}


float32 Timer::GetFPS()
{
	return fps;
}
