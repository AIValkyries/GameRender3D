#pragma once
#ifndef __SoftwareDriver2_helper_H__
#define __SoftwareDriver2_helper_H__

#include "DataType.h"


// 混合公式????
// Pixel = dest * ( 1 - alpha ) + source * alpha
// Alpha[0, 1]
static uint32 PixelBlend32(
	const uint32 dest, float32 sr, float32 sg, float32 sb, float32 a)
{
	float32 aa = 1.0F - a;

	uint8 dr = (dest >> 16) & 0x00FF;
	uint8 dg = (dest >> 8) & 0x0000FF;
	uint8 db = (dest & 0x000000FF);

	uint8 r = uint8(dr * aa + sr * a);
	uint8 g = uint8(dg * aa + sg * a);
	uint8 b = uint8(db * aa + sb * a);

	return ((int32)(a * 255) & 0xFF000000) | (r << 16) | (g << 8) | b;
}

// 目标颜色=缓冲区中的颜色，源颜色=当前使用颜色
static uint32 PixelBlend32(const uint32 dest, uint32 src,float32 alpha)
{
	float32 aa = 1.0F - alpha;

	uint8 dr = (dest >> 16) & 0x00FF;
	uint8 dg = (dest >> 8) & 0x0000FF;
	uint8 db = (dest & 0x000000FF);

	uint8 sr = (src >> 16) & 0x00FF;
	uint8 sg = (src >> 8) & 0x0000FF;
	uint8 sb = (src & 0x000000FF);

	uint8 r = uint8(dr * aa + sr * alpha);
	uint8 g = uint8(dg * aa + sg * alpha);
	uint8 b = uint8(db * aa + sb * alpha);

	return (r << 16) | (g << 8) | b;
}

// Pixel = dest * ( 1 - alpha ) + source * alpha
// Alpha[0, 1]
// dest + (source-dest)*alpha
static uint32 PixelBlend32(const uint32 dest, uint32 src)
{
	float32 alpha = ((src >> 24) & 0xFF) / 255.0F;
	if (alpha <= 0)
		return src;

	float32 aa = 1.0F - alpha;

	uint8 dr = (dest >> 16) & 0x00FF;
	uint8 dg = (dest >> 8) & 0x0000FF;
	uint8 db = (dest & 0x000000FF);

	uint8 sr = (src >> 16) & 0x00FF;
	uint8 sg = (src >> 8) & 0x0000FF;
	uint8 sb = (src & 0x000000FF);

	uint8 r = uint8(dr * aa + sr * alpha);
	uint8 g = uint8(dg * aa + sg * alpha);
	uint8 b = uint8(db * aa + sb * alpha);

	return (r << 16) | (g << 8) | b;
}

float32 ReadHeightMap32(int32 x, int32 y, int32 width, int32 height, uint32* data)
{
	if (x < 0)
		x = width-1;
	if (x >= width)
		x = 0;

	if (y < 0)
		y = height - 1;
	if (y >= height)
		y = 0;

	return (float32)Intensity32(data[(y * width) + x]);
}


float32 ReadHeightMap16(int32 x, int32 y, int32 width, int32 height, uint16* data)
{
	if (x < 0)
		x = width - 1;
	if (x >= width)
		x = 0;

	if (y < 0)
		y = height - 1;
	if (y >= height)
		y = 0;

	return float32(Intensity16(data[y * width + x]));
}

#endif

