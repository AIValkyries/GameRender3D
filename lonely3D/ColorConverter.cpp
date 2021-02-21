#include "ColorConverter.h"
#include "Color.h"

// 8个像素一个字节
void ColorConverter::Convert1BitTo16Bit(
	uint8* inData, uint16* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;

	// 输入数据转换成输出数据
	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		int32 shift = 7;

		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x++)
		{
			outData[x] = ((*inData >> shift) & 0x01) ? 0xFFFF : 0x8000;

			if (--shift < 0)
			{
				shift = 7;
				++inData;
			}
		}

		if (shift != 7)  // 最后一个像素
			++inData;

		if (!flip) outData += width;
		inData += linepad;
	}
}

// 一字节2个像素
void ColorConverter::Convert4BitTo16Bit(
	uint8* inData, uint16* outData,
	uint32 width, uint32 height, 
	uint32* paletteentry, 
	uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;

	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		int32 shift = 4;
		if (flip) outData -= width;
		
		for (uint32 x = 0;x < width;x++)
		{
			uint16 v = X8R8G8B8_TO_A1R5G5B5(paletteentry[(*inData >> shift) & 0x0F]);
			outData[x] = v;

			if (shift == 0)
			{
				shift = 4;
				++inData;
			}
			else
				shift = 0;
		}

		if (shift != 4) ++inData;
		if (!flip) outData += width;
		inData += linepad;
	}
}

// 一个字节1像素
void ColorConverter::Convert8BitTo16Bit(
	uint8* inData, uint16* outData,
	uint32 width, uint32 height, 
	uint32* paletteentry,
	uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;
	if (flip) outData += width * height;

	int32 totalIndex = 0;
	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x++)
		{
			int32 index = (*inData);
			uint16 v = X8R8G8B8_TO_A1R5G5B5(paletteentry[index]);
			outData[x] = v;
			++inData;
			totalIndex++;
		}

		if (!flip) outData += width;
		inData += linepad;
		totalIndex += linepad;
	}
}

void ColorConverter::Convert8BitTo24Bit(
	uint8* inData, uint8* outData,
	uint32 width, uint32 height,
	uint32* paletteentry,
	uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;


	int32 lineWidth = width * 3;
	if (flip) outData += lineWidth * height;

	for (uint32 y = 0; y < height; y++)
	{
		if (flip) outData -= lineWidth;

		for (int32 x = 0; x < lineWidth; x += 3)
		{
			uint16 v = X8R8G8B8_TO_A1R5G5B5(paletteentry[(*inData)]);

			outData[x + 0] = uint8(v);
			outData[x + 1] = uint8(v);
			outData[x + 2] = uint8(v);
			++inData;
		}

		if (!flip)
			outData += lineWidth;
		inData += linepad;
	}

}

void ColorConverter::Convert8BitTo32Bit(
	uint8* inData, uint32* outData,
	uint32 width, uint32 height,
	uint32* paletteentry,
	uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;
	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x++)
		{
			uint16 v = X8R8G8B8_TO_A1R5G5B5(paletteentry[(*inData)]);
			outData[x] = v;
			++inData;
		}

		if (!flip) outData += width;
		inData += linepad;
	}
}

void ColorConverter::Convert16BitTo16Bit(
	uint16* inData, uint16* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip)
{

	if (!inData || !outData)
		return;

	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x += 2)
		{
			outData[x] = *inData;
			++inData;
		}

		if (!flip) outData += width;
		inData += linepad;
	}
}

void ColorConverter::Convert16To24Bit(
	uint16* inData, uint8* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;

	int32 lineWidth = width * 3;
	if (flip) outData += lineWidth * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= lineWidth;

		for (int32 x = 0;x < lineWidth;x += 3)
		{
			outData[x + 0] = uint8(*inData);
			outData[x + 1] = uint8(*inData);
			outData[x + 2] = uint8(*inData);
			++inData;
		}

		if (!flip) outData += lineWidth;
		inData += linepad;
	}

}

void ColorConverter::Convert16To32Bit(
	uint16* inData, uint32* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;

	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x++)
		{
			outData[x] = *inData;
			++inData;
		}

		if (!flip) outData += width;
		inData += linepad;
	}
}

void ColorConverter::Convert24To24Bit(
	uint8* inData, uint8* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip, bool bgr)
{
	if (!inData || !outData)
		return;

	const uint32 lineWidth = width * 3;
	if (flip) outData += lineWidth * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= lineWidth;

		if (bgr)
		{
			for (uint32 x = 0;x < lineWidth;x += 3)
			{
				outData[x + 0] = inData[x + 2];
				outData[x + 1] = inData[x + 1];
				outData[x + 2] = inData[x + 0];
			}
		}
		else
		{
			memcpy(outData, inData, lineWidth);
		}

		if (!flip) outData += lineWidth;
		inData += linepad;
	}

}

void ColorConverter::Convert32To32Bit(
	uint32* inData, uint32* outData,
	uint32 width, uint32 height, uint32 linepad, bool flip)
{
	if (!inData || !outData)
		return;

	if (flip) outData += width * height;

	for (uint32 y = 0;y < height;y++)
	{
		if (flip) outData -= width;

		for (uint32 x = 0;x < width;x++)
		{
			outData[x] = *inData;
			++inData;
		}

		if (!flip) outData += width;
		inData += linepad;
	}

}