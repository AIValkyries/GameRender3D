#pragma once
#ifndef __Timer_H__
#define __Timer_H__

#include "DataType.h"
#include "stdafx.h"


/// <summary>
/// ��Ҫ���������飬
/// һ������ʱ�䣺��Ϸʱ�䣬��ʵʱ�䣬����ʱ��
/// ��������fps
/// �����趨һ�����֡
/// </summary>
class Timer
{
public:
	Timer();
	~Timer();

	void SetTimerSpeed(float32 speed);
	float32 GetTimerSpeed();
	float32 GetDeltaTime(); // ����ʱ��	
	float32 GetRealTime();  // ʵʱʱ��	
	float32 GetTime();      // ��ȡ��Ϸʱ��
	float32 Tick();

	float32 GetFPS();
	float32 CalculateFPS();

	uint32 StartClock();
	uint32 WaitClock(uint32 frame);

	void InitTimer();
private:

	LARGE_INTEGER HighPerformanceFreq;
	BOOL HighPerformanceTimerSupport;

	// ����fps
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
