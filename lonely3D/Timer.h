#pragma once
#ifndef __Timer_H__
#define __Timer_H__

#include "DataType.h"
#include "stdafx.h"


/// <summary>
/// 主要做三件事情，
/// 一、计算时间：游戏时间，真实时间，增量时间
/// 二、计算fps
/// 三、设定一秒多少帧
/// </summary>
class Timer
{
public:
	Timer();
	~Timer();

	void SetTimerSpeed(float32 speed);
	float32 GetTimerSpeed();
	float32 GetDeltaTime(); // 增量时间	
	float32 GetRealTime();  // 实时时间	
	float32 GetTime();      // 获取游戏时间
	float32 Tick();

	float32 GetFPS();
	float32 CalculateFPS();

	uint32 StartClock();
	uint32 WaitClock(uint32 frame);

	void InitTimer();
private:

	LARGE_INTEGER HighPerformanceFreq;
	BOOL HighPerformanceTimerSupport;

	// 计算fps
	float32 preTime;
	float32 fps;
	float32 framesCounted;
	float32 startFrameCount;

	float32 timeSpeed;
	float32 deltaTime;
	float32 realTime;
	float32 startTime;

};


inline Timer::Timer()
{

}

inline Timer::~Timer()
{

}


#endif
