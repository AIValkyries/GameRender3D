#pragma once
#ifndef __PerlinNoise_H__
#define __PerlinNoise_H__

#include "DataType.h"
#include "lMathf.h"

class PerlinNoise
{
public:
	PerlinNoise();
	~PerlinNoise();

	enum
	{
		TABLE_SIZE = 256,
		TABLE_MASK = TABLE_SIZE - 1,
	};

	float32 Noise(float32 x,float32 y,float32 scale);

	void SetupVecTable(int32 _maxSize);
	const Vector2& GetVec(int32 x,int32 y)const;

	float32 Fade(float32 t);

private:
	Vector2 vecTable[TABLE_SIZE];
	uint16 lut[TABLE_SIZE];
	int32 maxSize;	 
};

inline PerlinNoise::PerlinNoise() 
{
	maxSize = 0;
	SetupVecTable(256);
}
inline PerlinNoise::~PerlinNoise() {}

inline void PerlinNoise::SetupVecTable(int32 _maxSize)
{
	maxSize = _maxSize;
	float32 step = float32(TWO_PI / maxSize);

	float32 val = 0;

	for (int32 i = 0;i < maxSize;i++)
	{
		vecTable[i].x = Cos(val);
		vecTable[i].y = Sin(val);
		vecTable[i].Normalize();
		val += step;

		lut[i] = rand() & TABLE_MASK;
	}
}

inline const Vector2& PerlinNoise::GetVec(int32 x,int32 y)const
{
	int32 index = lut[x + y] & TABLE_MASK;
	return vecTable[index];
}

inline float32 PerlinNoise::Fade(float32 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float32 PerlinNoise::Noise(float32 x, float32 y, float32 scale)
{
	Vector2 pos(x * scale, y * scale);

	int32 x0 = int32(Floor(pos.x));
	int32 y0 = int32(Floor(pos.y));

	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	// 随机向量
	Vector2 v0 = GetVec(x0, y0);
	Vector2 v1 = GetVec(x1, y0);
	Vector2 v2 = GetVec(x0, y1);
	Vector2 v3 = GetVec(x1, y1);

	pos.Normalize();
	// 点到四个顶点的向量
	Vector2 d0 = pos - v0;
	Vector2 d1 = pos - v1;
	Vector2 d2 = pos - v2;
	Vector2 d3 = pos - v3;

	// 计算两个向量的梯度, 该梯度非微分中的梯度
	float32 g0 = Dot(v0, d0);
	float32 g1 = Dot(v1, d1);
	float32 g2 = Dot(v2, d2);
	float32 g3 = Dot(v3, d3);

	// 运用公式  6t^5-15t^4+10t^3
	float32 tx = pos.x - (int32)(pos.x);
	float32 ty = pos.y - (int32)(pos.y);

	float32 Sx, Sy;
	Sx = Fade(tx);
	Sy = Fade(ty);

	// 求梯度插值
	float32 avgX0 = Lerp(g0, g1, Sx);
	float32 avgX1 = Lerp(g2, g3, Sx);

	float32 result = Lerp(avgX0, avgX1, Sy);

	return result;
}


#endif

