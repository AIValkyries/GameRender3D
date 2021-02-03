#pragma once
#ifndef __Vertex_H__
#define __Vertex_H__

#include "DataType.h"
#include "lMathf.h"
#include "Color.h"

#define TEXTURED_MAX 4
// 顶点
typedef struct Vertex
{
	Vector4 position;
	Vector3 normal;
	ColorF color;
	Vector2 uv;
	float32 zValue;  // 1/z
	int32 Index; // 临时测试数据

	Vertex()
	{
		position = Vector4::zero();
		color = 0;
		normal = Vector4::zero();
		zValue = 0;
		uv.Set(0, 0);
	}

	Vertex(const Vector3& pos, const Vector3& _normal, const Vector2& _uv)
	{
		zValue = 0;
		uv = _uv;
		position = pos;
		normal = _normal;
		color = 0;
	}

	Vertex(const Vector3& pos, const Vector2& _uv)
	{
		zValue = 0;
		uv = _uv;
		position = pos;
		normal = Vector4::zero();
		color = 0;
	}

	Vertex(const Vector4& pos, const Vector2& _uv)
	{
		zValue = 0;
		uv = _uv;
		position = pos;
		normal = Vector4::zero();
		color = 0;
	}

	Vertex(const Vector4& pos, ColorU32& c)
	{
		position = pos;
		color = c.ToColorF();
		normal = Vector4::zero();
		uv.Set(0, 0);
		zValue = 0;
	}

	Vertex(const Vector4& pos, const ColorF& c)
	{
		position = pos;
		color = c;
		normal = Vector4::zero();
		uv.Set(0, 0);
		zValue = 0;
	}

	Vertex(const Vector4& pos, const Vector2& _uv, const ColorF& c)
	{
		position = pos;
		color = c;
		uv = _uv;
		zValue = 0;
		normal = Vector4::zero();
	}

	Vertex(
		const float32& px, const float32& py, const float32& pz,
		const float32& nx, const float32& ny, const float32& nz,
		const float32& u, const float32& v)
	{
		position = Vector4(px, py, pz, 1);
		normal = Vector4(nx, ny, nz, 1);
		uv = Vector2(u, v);
		zValue = 0;
		color = 0;
	}

	Vertex(
		const float32& px, const float32& py, const float32& pz,
		const float32& nx, const float32& ny, const float32& nz,
		const float32& u,  const float32& v,
		ColorF& c)
	{
		position = Vector4(px, py, pz, 1);
		normal = Vector4(nx, ny, nz, 1);
		uv = Vector2(u, v);
		color = c;
		zValue = 0;
	}

}Vertex;


#endif
