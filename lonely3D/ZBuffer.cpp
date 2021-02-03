#include "ZBuffer.h"
#include "Debug.h"

void ZBuffer::CreateBuffer(uint32 _width, uint32 _height)
{
	width = _width;
	height = _height;
	pitch = _width * sizeof(float32);
	totalSize = pitch * height;

	data = new uint8[totalSize];
	Clear();
}

void ZBuffer::Clear()
{
	// 1 / 100000000.0F
	for (uint32 i = 0;i < totalSize;i++)
		data[i] = uint8((float32)(1 / 100000000.0F));
}

void* ZBuffer::Lock()
{
	return (void*)data;
}

void ZBuffer::Unlock()
{

}