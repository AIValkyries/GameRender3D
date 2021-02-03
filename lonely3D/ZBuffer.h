#pragma once
#ifndef __ZBuffer_H__
#define __ZBuffer_H__

#include "DataType.h"
#include "UtilTools.h"

class ZBuffer
{
public:
	ZBuffer();
	ZBuffer(uint32 _width, uint32 _height);
	~ZBuffer();

	void* Lock();
	void Unlock();

	void CreateBuffer(uint32 _width, uint32 _height);
	void Clear();

	const uint32 GetPitch()const { return pitch; }

private:
	uint8* data;
	uint32 width;
	uint32 height;
	uint32 pitch;
	uint32 totalSize;
};

inline ZBuffer::ZBuffer()
{
	data = 0;
	width = 0;
	height = 0;
	pitch = 0;
	totalSize = 0;
}

inline ZBuffer::ZBuffer(uint32 _width,uint32 _height)
{
	data = 0;
	pitch = 0;
	totalSize = 0;

	CreateBuffer(_width, _height);
}

inline ZBuffer::~ZBuffer()
{
	SafeDeleteArray(data);
}




#endif
