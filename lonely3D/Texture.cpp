#include "Texture.h"
#include "SoftwareDriver2_helper.h"
#include "PerlinNoise.h"
#include "Debug.h"
#include "UtilTools.h"

Texture::~Texture()
{
	SafeDeleteArray(datas);

	if (mipMaps)
	{
		for (int32 i = 0;i < mipLevel;i++)
			SafeDelete(mipMaps[i]);
	}

	SafeDelete(rect);
}

void Texture::InitData()
{
	uint16 bitsPerPixel = GetBitsPerPixelFromFormat(colorFormat);
	bytesPerPixel = bitsPerPixel / 8;
	pitch = width * bytesPerPixel;
	length = pitch * height;

	datas = new uint8[length];

	memset(datas, 0, sizeof(uint8) * length);
}

void Texture::CreateTexture(
	COLOR_FORMAT _colorFormat, uint32 _width, uint32 _height)
{
	width = uint16(_width);
	height = uint16(_height);
	colorFormat = _colorFormat;
	InitData();
}

void Texture::CreateTexture(
	COLOR_FORMAT _colorFormat,
	uint32 _width, uint32 _height,
	void* data)
{
	width = uint16(_width);
	height = uint16(_height);
	colorFormat = _colorFormat;
	InitData();
	delete[] datas;
	datas = (uint8*)data;
}

uint16 Texture::GetBilinearSampling16(uint16 x, uint16 y, float32 u, float32 v)const
{
	// 取
	uint16* srcBuffer = (uint16*)datas;
	uint16 nextX = x + 1;
	uint16 nextY = y + 1;

	nextX = Clamp(nextX, 0, (width - 1));
	nextY = Clamp(nextY, 0, (height - 1));

	uint16 p0 = srcBuffer[x + y * width];
	uint16 p1 = srcBuffer[nextX + y * width];
	uint16 p2 = srcBuffer[nextX + nextY * width];
	uint16 p3 = srcBuffer[x + nextY * width];

	float32 avg_u = 1.0F - u;
	float32 avg_v = 1.0F - v;

	float32 p0Inter = avg_u * avg_v;
	float32 p1Inter = u * avg_v;
	float32 p2Inter = u * v;
	float32 p3Inter = avg_u * v;

	uint16 p0Color = (uint16)((p0 & 0xFF00) * p0Inter) | (uint16)((p0 & 0xFF) * p0Inter);
	uint16 p1Color = (uint16)((p1 & 0xFF00) * p1Inter) | (uint16)((p1 & 0xFF) * p1Inter);
	uint16 p2Color = (uint16)((p2 & 0xFF00) * p2Inter) | (uint16)((p2 & 0xFF) * p2Inter);
	uint16 p3Color = (uint16)((p3 & 0xFF00) * p3Inter) | (uint16)((p3 & 0xFF) * p3Inter);

	return p0Color + p1Color + p2Color + p3Color;
}

uint32 Texture::GetBilinearSampling32(uint16 x, uint16 y, float32 u, float32 v)const
{
	uint32* srcBuffer = (uint32*)datas;

	uint16 nextX = x + 1;
	uint16 nextY = y + 1;

	nextX = Clamp(nextX, 0, (width - 1));
	nextY = Clamp(nextY, 0, (height - 1));

	uint32 p0 = srcBuffer[x + y * width];
	uint32 p1 = srcBuffer[nextX + y * width];
	uint32 p2 = srcBuffer[nextX + nextY * width];
	uint32 p3 = srcBuffer[x + nextY * width];

	float32 avg_u = 1.0F - u;
	float32 avg_v = 1.0F - v;

	float32 p0Inter = avg_u * avg_v;
	float32 p1Inter = u * avg_v;
	float32 p2Inter = u * v;
	float32 p3Inter = avg_u * v;

	uint32 p0Color = 
		(uint32)((p0 & 0x00FF0000) * p0Inter) << 16 |
		(uint32)((p0 & 0x0000FF00) * p0Inter) << 8 |
		(uint32)((p0 & 0x000000FF) * p0Inter);

	uint32 p1Color = 
		(uint32)((p1 & 0x00FF0000) * p1Inter) << 16 |
		(uint32)((p1 & 0x0000FF00) * p1Inter) << 8 |
		(uint32)((p1 & 0x000000FF) * p1Inter);

	uint32 p2Color = 
		(uint32)((p2 & 0x00FF0000) * p2Inter) << 16 |
		(uint32)((p2 & 0x0000FF00) * p2Inter) << 8 |
		(uint32)((p2 & 0x000000FF) * p2Inter);

	uint32 p3Color = 
		(uint32)((p3 & 0x00FF0000) * p3Inter) << 16 |
		(uint32)((p3 & 0x0000FF00) * p3Inter) << 8 |
		(uint32)((p3 & 0x000000FF) * p3Inter);

	return p0Color + p1Color + p2Color + p3Color;
}

void Texture::SetColor(int32 x, int32 y, uint32 color)
{
	int32 index = (y * pitch) + (x * bytesPerPixel);
	uint8* startColor = &datas[index];
	uint8* destColor = (uint8*)&color;

	uint8 copyCount = uint8(Min((uint16)sizeof(color), bytesPerPixel));

	for (int32 i = 0;i < copyCount;i++)
	{
		// copyCount - 1 -
		startColor[i] = destColor[ i];
	}
}

ColorU32 Texture::GetColorU32(int32 x, int32 y)
{
	uint32 color = GetColor(x, y);
	ColorU32 c(color);
	return c;
}

uint32 Texture::GetColor(int32 x, int32 y)
{
	uint32 value = 0;
	int32 index = 0;

	switch (colorFormat)
	{
	case RGB_565: 
	{
		index = x + y * width;
		uint16* destBuffer = (uint16*)(datas);

		value = RGB_565_TO_ARGB_32(destBuffer[index]);
	}break;
	case ARGB_1555:
	{			
		index = x + y * width;
		uint16* destBuffer = (uint16*)(datas);

		value = ARGB_1555_TO_ARGB_32(destBuffer[index]);
		//value = destBuffer[index];
	}break;
	case RGB_24:   
	{
		index = y * width * 3 + x * 3;

		uint8 r = datas[index + 2];
		uint8 g = datas[index + 1];
		uint8 b = datas[index + 0];

		value = 0x00000000 | (r << 16) | (g << 8) | b;
	}break;
	case ARGB_32:  
	{			
		index = (x * 4) + (y * pitch);
		
		uint8 a = datas[index + 3];
		uint8 r = datas[index + 2];
		uint8 g = datas[index + 1];
		uint8 b = datas[index + 0];

		value = (a << 24) | (r << 16) | (g << 8) | b;
	}break;
	}	

	return value;
}

ColorF Texture::GetPixelF(float32 u, float32 v)const
{
	// 寻址模式,如果uv超过1，使用哪种模式
	// 过滤模式,过滤方式
	uint16 maxWidth = width - 1;
	uint16 maxHeight = height - 1;

	uint16 x = uint16((u * maxWidth) + 0.5F);
	uint16 y = uint16((v * maxHeight) + 0.5F);

	ColorF color(0, 0, 0, 0);

#pragma region 寻址模式

	if (warpMode == REPEAT)  // 重复
	{
		if (x >= width)
		{
			x = x % width;
		}
		if (y >= height)
		{
			y = y % height;
		}
	}
	else if (warpMode == CLAMP)  // 剪切
	{
		x = Clamp(x, 0, maxWidth);
		y = Clamp(y, 0, maxHeight);

		if (x >= width || y >= height)
			return 0xFFFFFF;
	}
	else if (warpMode == MIRROR) // 镜像
	{
		x = maxWidth - x;
		y = maxHeight - y;
	}

#pragma endregion

#pragma region 过滤模式

	if (filterMode == FILTER_POINT)
	{
		switch (colorFormat)
		{
		case RGB_565:
		{
			uint16* destBuffer = (uint16*)(datas + (x << 1) + y * pitch);
			uint16 v = *destBuffer;

			float32 inv = 1.0F / 255.0f;

			float32 r = ((v >> 11) & 0x1f) * inv;
			float32 g = ((v >> 5) & 0x3F) * inv;
			float32 b = ((v & 0x1f)) * inv;

			color.Set(r, g, b);
			
			return color;
		}
		case ARGB_1555:
		{
			uint16* destBuffer = (uint16*)(datas + (x << 1) + y * pitch);
			uint16 v = *destBuffer;

			float32 inv = 1.0F / 255.0f;

			float32 r = (((v & ~0x8000) >> 10) & 0x1f) * inv;
			float32 g = ((v >> 5) & 0x3F) * inv;
			float32 b = ((v & 0x1f)) * inv;

			color.Set(r, g, b);

			return color;
		}break;
		case RGB_24:
		{
			uint8* p = datas + (y * 3 * width) + (x * 3);

			float32 inv = 1.0F / 255.0F;

			float32 r = p[0] * inv;
			float32 g = p[1] * inv;
			float32 b = p[2] * inv;

			color.Set(r, g, b);
			return color;
		}
		case ARGB_32:
		{
			uint32* destBuffer = (uint32*)(datas + (x * 4) + y * pitch);
			
			float32 inv = 1.0F / 255.0F;
			uint32 v = *destBuffer;

			float32 a = ((v >> 24) & 0xff) * inv;
			float32 r = ((v >> 16) & 0xff) * inv;
			float32 g = ((v >> 8) &  0xff) * inv;
			float32 b = ((v & 0xff)) * inv;

			color.Set(a, r, g, b);

			return color;
		}break;
		}
	}
	else if (filterMode == FILTER_BILINEAR)
	{
		uint16 nextX = x + 1;
		uint16 nextY = y + 1;

		nextX = Clamp(nextX, 0, maxWidth);
		nextY = Clamp(nextY, 0, maxHeight);

		float32 avg_u = 1.0F - u;
		float32 avg_v = 1.0F - v;

		float32 p0Inter = avg_u * avg_v;
		float32 p1Inter = u * avg_v;
		float32 p2Inter = u * v;
		float32 p3Inter = avg_u * v;

		switch (colorFormat)
		{
		case RGB_565:
		{
			uint16* destBuffer = (uint16*)datas;

			uint32 yPitch = y * width;
			uint32 yNextPitch = nextY * width;

			uint16 p0 = *(destBuffer + x + yPitch);
			uint16 p1 = *(destBuffer + nextX + yPitch);
			uint16 p2 = *(destBuffer + nextX + yNextPitch);
			uint16 p3 = *(destBuffer + x + yNextPitch);

			uint16 p0Color =
				(uint16)((p0 & 0xF800) * p0Inter) |
				(uint16)((p0 & 0x07E0) * p0Inter) |
				(uint16)((p0 & 0x001F) * p0Inter);

			uint16 p1Color =
				(uint16)((p1 & 0xF800) * p1Inter) |
				(uint16)((p1 & 0x07E0) * p1Inter) |
				(uint16)((p1 & 0x001F) * p1Inter);

			uint16 p2Color =
				(uint16)((p2 & 0xF800) * p2Inter) |
				(uint16)((p2 & 0x07E0) * p2Inter) |
				(uint16)((p2 & 0x001F) * p2Inter);

			uint16 p3Color =
				(uint16)((p3 & 0xF800) * p3Inter) |
				(uint16)((p3 & 0x07E0) * p3Inter) |
				(uint16)((p3 & 0x001F) * p3Inter);

			uint16 totalColor = p0Color + p1Color + p2Color + p3Color;
			
			float32 inv = 1.0F / 255.0f;

			float32 r = ((totalColor >> 11) & 0x1f) * inv;
			float32 g = ((totalColor >> 5) & 0x3F) * inv;
			float32 b = ((totalColor & 0x1f)) * inv;

			color.Set(r, g, b);

			return color;
		};
		case ARGB_1555:
		{
			uint16* destBuffer = (uint16*)datas;

			uint32 yPitch = y * width;
			uint32 yNextPitch = nextY * width;

			uint16 p0 = *(destBuffer + x + yPitch);
			uint16 p1 = *(destBuffer + nextX + yPitch);
			uint16 p2 = *(destBuffer + nextX + yNextPitch);
			uint16 p3 = *(destBuffer + x + yNextPitch);

			uint16 p0Color =
				(uint16)((p0 & 0x7C00) * p0Inter) |
				(uint16)((p0 & 0x03E0) * p0Inter) |
				(uint16)((p0 & 0x001F) * p0Inter);

			uint16 p1Color =
				(uint16)((p1 & 0x7C00) * p1Inter) |
				(uint16)((p1 & 0x03E0) * p1Inter) |
				(uint16)((p1 & 0x001F) * p1Inter);

			uint16 p2Color =
				(uint16)((p2 & 0x7C00) * p2Inter) |
				(uint16)((p2 & 0x03E0) * p2Inter) |
				(uint16)((p2 & 0x001F) * p2Inter);

			uint16 p3Color =
				(uint16)((p3 & 0x7C00) * p3Inter) |
				(uint16)((p3 & 0x03E0) * p3Inter) |
				(uint16)((p3 & 0x001F) * p3Inter);

			uint16 totalColor = p0Color + p1Color + p2Color + p3Color;
			
			float32 inv = 1.0F / 255.0f;

			float32 r = (((totalColor & ~0x8000) >> 10) & 0x1f) * inv;
			float32 g = ((totalColor >> 5) & 0x3F) * inv;
			float32 b = ((totalColor & 0x1f)) * inv;

			color.Set(r, g, b);

			return color;
		};
		case RGB_24:
		{
			uint32 yPitch = y * 3 * width;
			uint32 yNextPitch = nextY * 3 * width;
			uint32 xPitch = x * 3;
			uint32 xNextPitch = nextX * 3;

			uint8* p0 = datas + yPitch + xPitch;
			uint8* p1 = datas + yPitch + xNextPitch;
			uint8* p2 = datas + yNextPitch + xNextPitch;
			uint8* p3 = datas + yNextPitch + xPitch;

			float32 p00 = p0[0] * p0Inter;
			float32 p01 = p0[1] * p0Inter;
			float32 p02 = p0[2] * p0Inter;

			uint32 p0Color = uint32(((uint32)(p00) << 16) + ((uint32)(p01) << 8) + p02);

			float32 p10 = p1[0] * p1Inter;
			float32 p11 = p1[1] * p1Inter;
			float32 p12 = p1[2] * p1Inter;

			uint32 p1Color = uint32(((uint32)(p10) << 16) + ((uint32)(p11) << 8) + p12);

			float32 p20 = p2[0] * p2Inter;
			float32 p21 = p2[1] * p2Inter;
			float32 p22 = p2[2] * p2Inter;

			uint32 p2Color = uint32(((uint32)(p20) << 16) + ((uint32)(p21) << 8) + p22);

			float32 p30 = p3[0] * p3Inter;
			float32 p31 = p3[1] * p3Inter;
			float32 p32 = p3[2] * p3Inter;

			uint32 p3Color = uint32(((uint32)(p30) << 16) + ((uint32)(p31) << 8) + p32);

			uint32 totalColor = p0Color + p1Color + p2Color + p3Color;
			
			float32 inv = 1.0F / 255.0F;

			float32 r = ((totalColor >> 16) & 0xff) * inv;
			float32 g = ((totalColor >> 8) & 0xff) * inv;
			float32 b = (totalColor & 0xff) * inv;

			color.Set(r, g, b);

			return color;
		};
		case ARGB_32:
		{
			uint32 xPitch = x * 4;
			uint32 xNextPitch = nextX * 4;
			uint32 yPitch = y * 4 * width;
			uint32 yNextPitch = nextY * 4 * width;

			uint8* p0 = datas + xPitch + yPitch;
			uint8* p1 = datas + xNextPitch + yPitch;
			uint8* p2 = datas + xNextPitch + yNextPitch;
			uint8* p3 = datas + nextX + yNextPitch;

			uint32 p0Color =
				(((uint32)(p0[0] * p0Inter)) << 24) |
				(((uint32)(p0[1] * p0Inter)) << 16) |
				(((uint32)(p0[2] * p0Inter)) << 8) |
				((uint32)(p0[3] * p0Inter));

			uint32 p1Color =
				(((uint32)(p1[0] * p1Inter)) << 24) |
				(((uint32)(p1[1] * p1Inter)) << 16) |
				(((uint32)(p1[2] * p1Inter)) << 8) |
				((uint32)(p1[3] * p1Inter));

			uint32 p2Color =
				(((uint32)(p2[0] * p2Inter)) << 24) |
				(((uint32)(p2[1] * p2Inter)) << 16) |
				(((uint32)(p2[2] * p2Inter)) << 8) |
				((uint32)(p2[3] * p2Inter));

			uint32 p3Color =
				(((uint32)(p3[0] * p3Inter)) << 24) |
				(((uint32)(p3[1] * p3Inter)) << 16) |
				(((uint32)(p3[2] * p3Inter)) << 8) |
				((uint32)(p3[3] * p3Inter));

			uint32 totalColor = p0Color + p1Color + p2Color + p3Color;
		
			float32 inv = 1.0F / 255.0F;

			float32 a = ((totalColor >> 24) & 0xff) * inv;
			float32 r = ((totalColor >> 16) & 0xff) * inv;
			float32 g = ((totalColor >> 8) & 0xff) * inv;
			float32 b = ((totalColor & 0xff)) * inv;

			color.Set(a, r, g, b);

			return color;
		};
		}
	}

	return color;
#pragma endregion
}

ColorU32 Texture::GetPixel(float32 u, float32 v)const
{
	// 寻址模式,如果uv超过1，使用哪种模式
	// 过滤模式,过滤方式
	uint16 maxWidth = width - 1;
	uint16 maxHeight = height - 1;

	uint16 x = uint16((u * maxWidth) + 0.5F);
	uint16 y = uint16((v * maxHeight) + 0.5F);

	ColorU32 color(0, 0, 0, 0);

#pragma region 寻址模式

	if (warpMode == REPEAT)  // 重复
	{
		if (x >= width)
		{
			x = x % width;
		}
		if (y >= height)
		{
			y = y % height;
		}
	}
	else if (warpMode == CLAMP)  // 剪切
	{
		x = Clamp(x, 0, maxWidth);
		y = Clamp(y, 0, maxHeight);

		if (x >= width || y >= height)
			return 0xFFFFFF;
	}
	else if (warpMode == MIRROR) // 镜像
	{
		x = maxWidth - x;
		y = maxHeight - y;
	}

#pragma endregion

#pragma region 过滤模式
 
	if (filterMode == FILTER_POINT)
	{
		switch (colorFormat)
		{
		case RGB_565:
		{
			uint16* destBuffer = (uint16*)(datas + (x << 1) + y * pitch);
			color.Set(*destBuffer);
			return color;
		}
		case ARGB_1555:
		{
			uint16* destBuffer = (uint16*)(datas + (x << 1) + y * pitch);
			color.Set(*destBuffer);
		}break;
		case RGB_24:
		{
			uint8* p = datas + (y * 3 * width) + (x * 3);

			uint8 r = p[0];
			uint8 g = p[1];
			uint8 b = p[2];

			uint32 value = 0xFF000000 |
				((r << 16) & 0x00FF0000) |
				((g << 8) & 0x0000FF00) | b;

			color.Set(value);
			return color;
		}
		case ARGB_32:
		{
			uint32* destBuffer = (uint32*)(datas + (x * 4) + y * pitch);
			color.Set(*destBuffer);
			return color;
		}break;
		}
	}
	else if (filterMode == FILTER_BILINEAR)
	{
		uint16 nextX = x + 1;
		uint16 nextY = y + 1;

		nextX = Clamp(nextX, 0, maxWidth);
		nextY = Clamp(nextY, 0, maxHeight);

		float32 avg_u = 1.0F - u;
		float32 avg_v = 1.0F - v;

		float32 p0Inter = avg_u * avg_v;
		float32 p1Inter = u * avg_v;
		float32 p2Inter = u * v;
		float32 p3Inter = avg_u * v;

		switch (colorFormat)
		{
		case RGB_565:
		{
			uint16* destBuffer = (uint16*)datas;

			uint32 yPitch = y * width;
			uint32 yNextPitch = nextY * width;

			uint16 p0 = *(destBuffer + x + yPitch);
			uint16 p1 = *(destBuffer + nextX + yPitch);
			uint16 p2 = *(destBuffer + nextX + yNextPitch);
			uint16 p3 = *(destBuffer + x + yNextPitch);

			uint16 p0Color =
				(uint16)((p0 & 0xF800) * p0Inter) |
				(uint16)((p0 & 0x07E0) * p0Inter) |
				(uint16)((p0 & 0x001F) * p0Inter);

			uint16 p1Color =
				(uint16)((p1 & 0xF800) * p1Inter) |
				(uint16)((p1 & 0x07E0) * p1Inter) |
				(uint16)((p1 & 0x001F) * p1Inter);

			uint16 p2Color =
				(uint16)((p2 & 0xF800) * p2Inter) |
				(uint16)((p2 & 0x07E0) * p2Inter) |
				(uint16)((p2 & 0x001F) * p2Inter);

			uint16 p3Color =
				(uint16)((p3 & 0xF800) * p3Inter) |
				(uint16)((p3 & 0x07E0) * p3Inter) |
				(uint16)((p3 & 0x001F) * p3Inter);

			uint16 totalColor = p0Color + p1Color + p2Color + p3Color;
			color.Set(totalColor);

			return color;
		};
		case ARGB_1555:
		{
			uint16* destBuffer = (uint16*)datas;

			uint32 yPitch = y * width;
			uint32 yNextPitch = nextY * width;

			uint16 p0 = *(destBuffer + x + yPitch);
			uint16 p1 = *(destBuffer + nextX + yPitch);
			uint16 p2 = *(destBuffer + nextX + yNextPitch);
			uint16 p3 = *(destBuffer + x + yNextPitch);

			uint16 p0Color =
				(uint16)((p0 & 0x7C00) * p0Inter) |
				(uint16)((p0 & 0x03E0) * p0Inter) |
				(uint16)((p0 & 0x001F) * p0Inter);

			uint16 p1Color =
				(uint16)((p1 & 0x7C00) * p1Inter) |
				(uint16)((p1 & 0x03E0) * p1Inter) |
				(uint16)((p1 & 0x001F) * p1Inter);

			uint16 p2Color =
				(uint16)((p2 & 0x7C00) * p2Inter) |
				(uint16)((p2 & 0x03E0) * p2Inter) |
				(uint16)((p2 & 0x001F) * p2Inter);

			uint16 p3Color =
				(uint16)((p3 & 0x7C00) * p3Inter) |
				(uint16)((p3 & 0x03E0) * p3Inter) |
				(uint16)((p3 & 0x001F) * p3Inter);

			uint16 totalColor = p0Color + p1Color + p2Color + p3Color;
			color.Set(totalColor);

			return color;
		};
		case RGB_24:
		{
			uint32 yPitch = y * 3 * width;
			uint32 yNextPitch = nextY * 3 * width;
			uint32 xPitch = x * 3;
			uint32 xNextPitch = nextX * 3;

			uint8* p0 = datas + yPitch + xPitch;
			uint8* p1 = datas + yPitch + xNextPitch;
			uint8* p2 = datas + yNextPitch + xNextPitch;
			uint8* p3 = datas + yNextPitch + xPitch;

			float32 p00 = p0[0] * p0Inter;
			float32 p01 = p0[1] * p0Inter;
			float32 p02 = p0[2] * p0Inter;

			uint32 p0Color = uint32(((uint32)(p00) << 16) + ((uint32)(p01) << 8) + p02);

			float32 p10 = p1[0] * p1Inter;
			float32 p11 = p1[1] * p1Inter;
			float32 p12 = p1[2] * p1Inter;

			uint32 p1Color = uint32(((uint32)(p10) << 16) + ((uint32)(p11) << 8) + p12);

			float32 p20 = p2[0] * p2Inter;
			float32 p21 = p2[1] * p2Inter;
			float32 p22 = p2[2] * p2Inter;

			uint32 p2Color = uint32(((uint32)(p20) << 16) + ((uint32)(p21) << 8) + p22);

			float32 p30 = p3[0] * p3Inter;
			float32 p31 = p3[1] * p3Inter;
			float32 p32 = p3[2] * p3Inter;

			uint32 p3Color = uint32(((uint32)(p30) << 16) + ((uint32)(p31) << 8) + p32);

			uint32 totalColor = p0Color + p1Color + p2Color + p3Color;
			color.Set(totalColor);

			return color;
		};
		case ARGB_32:
		{
			uint32 xPitch = x * 4;
			uint32 xNextPitch = nextX * 4;
			uint32 yPitch = y * 4 * width;
			uint32 yNextPitch = nextY * 4 * width;

			uint8* p0 = datas + xPitch + yPitch;
			uint8* p1 = datas + xNextPitch + yPitch;
			uint8* p2 = datas + xNextPitch + yNextPitch;
			uint8* p3 = datas + nextX + yNextPitch;

			uint32 p0Color =
				(((uint32)(p0[0] * p0Inter)) << 24) |
				(((uint32)(p0[1] * p0Inter)) << 16) |
				(((uint32)(p0[2] * p0Inter)) << 8) |
				((uint32)(p0[3] * p0Inter));

			uint32 p1Color =
				(((uint32)(p1[0] * p1Inter)) << 24) |
				(((uint32)(p1[1] * p1Inter)) << 16) |
				(((uint32)(p1[2] * p1Inter)) << 8) |
				((uint32)(p1[3] * p1Inter));

			uint32 p2Color =
				(((uint32)(p2[0] * p2Inter)) << 24) |
				(((uint32)(p2[1] * p2Inter)) << 16) |
				(((uint32)(p2[2] * p2Inter)) << 8) |
				((uint32)(p2[3] * p2Inter));

			uint32 p3Color =
				(((uint32)(p3[0] * p3Inter)) << 24) |
				(((uint32)(p3[1] * p3Inter)) << 16) |
				(((uint32)(p3[2] * p3Inter)) << 8) |
				((uint32)(p3[3] * p3Inter));

			uint32 totalColor = p0Color + p1Color + p2Color + p3Color;
			color.Set(totalColor);

			return color;
		};
		}
	}

#pragma endregion

	return color;
}

void Texture::SetPixel(uint16 x, uint16 y, ColorU32 color, float32 blend)const
{
	if (x >= width || y >= height)
		return;

	switch (colorFormat)
	{
	case RGB_565:
	{
		uint16* destBuffer = (uint16*)(datas + y * pitch + (x << 1));
		*destBuffer = color.ToRGB565();
	}break;
	case ARGB_1555:
	{
		uint16* destBuffer = (uint16*)(datas + y * pitch + (x << 1));
		*destBuffer = color.ToRGB565();
	}break;
	case RGB_24:
	{
		uint8* destBuffer = datas + (y * pitch) + (x * 3);
		destBuffer[0] = (uint8)color.GetRed();
		destBuffer[1] = (uint8)color.GetGreen();
		destBuffer[2] = (uint8)color.GetBlue();
	}break;
	case ARGB_32:
	{
		// 这里需要混合
		uint32 indexCount = (y * pitch) + (x << 2);
		uint32* destBuffer = (uint32*)(datas + indexCount);

		uint32 c = 0;

		if (blend == 1.0F)
		{
			c = PixelBlend32(*destBuffer, color.GetColor());
		}
		else if (blend > 0.0F && blend < 1.0F)
		{
			c = PixelBlend32(*destBuffer, color.GetColor(), blend);
		}
		else
			c = color.GetColor();

		*destBuffer = c;
	}break;
	}
}

void Texture::GenerateMipping()
{
	uint16 mip_width = width;
	uint16 mip_height = height;

	mipLevel = uint16(NearestPowerOfTwo(width));
	mipMaps = (Texture**)malloc(mipLevel * sizeof(Texture));
	mipMaps[0] = this;

	for (int32 i = 1;i < mipLevel;i++)
	{
		mip_width = uint16(mip_width * 0.5F);
		mip_height = uint16(mip_height * 0.5F);

		mipMaps[i] = (Texture*)malloc(sizeof(Texture));
		mipMaps[i]->CreateTexture(colorFormat, mip_width, mip_height);

		for (int32 x = 0;x < mip_width;x++)   // 类似列
		{
			for (int32 y = 0;y < mip_height;y++) // 类似行
			{
				uint32
					r0, g0, b0, a0,
					r1, g1, b1, a1,
					r2, g2, b2, a2,
					r3, g3, b3, a3;

				uint32 avg_r, avg_g, avg_b, avg_a;

				if (colorFormat == ARGB_1555 || colorFormat == RGB_565)
				{
					uint16* srcBuffer = (uint16*)mipMaps[i - 1]->Lock();
					uint16* destBuffer = (uint16*)mipMaps[i]->Lock();

					// 获取src的4个方格颜色
					uint16 perWidth = mipMaps[i - 1]->GetWidth();

					uint16 p0 = srcBuffer[(x * 2) + (perWidth * y * 2)];
					uint16 p1 = srcBuffer[((x + 1) * 2) + (perWidth * y * 2)];
					uint16 p2 = srcBuffer[((x + 1) * 2) + (perWidth * (y + 1) * 2)];
					uint16 p3 = srcBuffer[(x * 2) + (perWidth * (y + 1) * 2)];

					uint16 rShift = 10;
					uint32 gMask = 0x1F;
					uint16 gValue = 31;

					if (colorFormat == RGB_565)
					{
						rShift = 11;
						gMask = 0x3F;
						gValue = 61;
					}

					r0 = (p0 >> rShift) & 0x1F;
					g0 = (p0 >> 5) & gMask;
					b0 = (p0 & 0x1F);

					r1 = (p1 >> rShift) & 0x1F;
					g1 = (p1 >> 5) & gMask;
					b1 = (p1 & 0x1F);

					r2 = (p2 >> rShift) & 0x1F;
					g2 = (p2 >> 5) & gMask;
					b2 = (p2 & 0x1F);

					r3 = (p3 >> rShift) & 0x1F;
					g3 = (p3 >> 5) & gMask;
					b3 = (p3 & 0x1F);

					avg_r = uint32((r0 + r1 + r2 + r3) * 0.25F);
					avg_g = uint32((g0 + g1 + g2 + g3) * 0.25F);
					avg_b = uint32((b0 + b1 + b2 + b3) * 0.25F);

					if (avg_r > 31) avg_r = 31;
					if (avg_g > gValue)avg_g = gValue;
					if (avg_b > 31)avg_b = 31;

					uint16 color = 0;

					if (colorFormat == RGB_565)
					{
						color = uint16((avg_r << 11) | (avg_g << 5) | (avg_b));
					}
					else if (colorFormat == ARGB_1555)
					{
						color = uint16(0x8000 | (avg_r << 10) | (avg_g << 5) | (avg_b));
					}

					destBuffer[x + y * mip_width] = color;
				}
				else if (colorFormat == ARGB_32 || colorFormat == RGB_24)
				{
					uint8* srcBuffer = (uint8*)mipMaps[i - 1]->Lock();
					uint8* destBuffer = (uint8*)mipMaps[i]->Lock();

					uint16 bytePerPixel = colorFormat == ARGB_32 ? 4 : 3;
					uint16 pitch = mipMaps[i - 1]->GetWidth() * bytePerPixel;

					uint8* p0 = &srcBuffer[(x * 2 * bytePerPixel) + (y * 2 * pitch)];
					uint8* p1 = &srcBuffer[((x + 1) * 2 * bytePerPixel) + (y * 2 * pitch)];
					uint8* p2 = &srcBuffer[((x + 1) * 2 * bytePerPixel) + ((y + 1) * 2 * pitch)];
					uint8* p3 = &srcBuffer[(x * 2 * bytePerPixel) + ((y + 1) * 2 * pitch)];

					if (colorFormat == ARGB_32)
					{
						a0 = p0[0];r0 = p0[1];g0 = p0[2];b0 = p0[3];
						a1 = p1[0];r1 = p1[1];g1 = p1[2];b1 = p1[3];
						a2 = p2[0];r2 = p2[1];g2 = p2[2];b2 = p2[3];
						a3 = p3[0];r3 = p3[1];g3 = p3[2];b3 = p3[3];
					}
					else
					{
						r0 = p0[0];g0 = p0[1];b0 = p0[2];
						r1 = p1[0];g1 = p1[1];b1 = p1[2];
						r2 = p2[0];g2 = p2[1];b2 = p2[2];
						r3 = p3[0];g3 = p3[1];b3 = p3[2];
					}

					avg_a = uint32((a0 + a1 + a2 + a3) * 0.25F);
					avg_r = uint32((r0 + r1 + r2 + r3) * 0.25F);
					avg_g = uint32((g0 + g1 + g2 + g3) * 0.25F);
					avg_b = uint32((b0 + b1 + b2 + b3) * 0.25F);

					if (avg_a > 255)avg_a = 255;
					if (avg_r > 255)avg_r = 255;
					if (avg_g > 255)avg_g = 255;
					if (avg_b > 255)avg_b = 255;

					uint32 xIndex = x * bytePerPixel;
					uint32 yIndex = y * mipMaps[i]->GetPitch();

					if (colorFormat == ARGB_32)
					{
						destBuffer[xIndex + yIndex + 0] = uint8(avg_a);
						destBuffer[xIndex + yIndex + 1] = uint8(avg_r);
						destBuffer[xIndex + yIndex + 2] = uint8(avg_g);
						destBuffer[xIndex + yIndex + 3] = uint8(avg_b);
					}
					else if (colorFormat == RGB_24)
					{
						destBuffer[xIndex + yIndex + 0] = uint8(avg_r);
						destBuffer[xIndex + yIndex + 1] = uint8(avg_g);
						destBuffer[xIndex + yIndex + 2] = uint8(avg_b);
					}
				}
			}
		}
	}
}


/// <summary>
///  生成柏林噪声
/// </summary>
/// <param name="scale">缩放越小频率越大</param>
/// <param name="octaves">纹理叠加的张数</param>
/// <param name="falloff">振幅越大地形高低越明显	</param>
void Texture::GeneratePerlinNoise(
	float32 scale, float32 octaves, float32 falloff)
{
	if (datas == 0)
		return;

	float32* tempDatas = (float32*)Lock();

	static PerlinNoise noise;

	for (int32 y = 0;y < height;y++)
	{
		for (int32 x = 0;x < width;x++)
		{
			float32 accum = 0;
			float32 frequency = scale;
			float32 amplitude = 1.0F;

			for (int32 i = 0;i < octaves;i++)
			{
				accum += noise.Noise(float32(x), float32(y), frequency) * amplitude;
				frequency *= 2.0F;
				amplitude *= falloff;
			}

			//Debug::LogError(Convert(accum));

			tempDatas[y * width + x] = accum;
		}
	}
}

/// <summary>
/// 由高度图生成法线贴图
/// 参考:https://zhuanlan.zhihu.com/p/68328851
/// </summary>
void Texture::GenerateNormalMap(Texture* heightMap, float32 amplitude)
{
	if (!heightMap)
		return;

	if (heightMap->GetColorFormat() != ARGB_1555 &&
		heightMap->GetColorFormat() != ARGB_32)
	{
		Debug::LogError("错误：不支持用于制作法线贴图的纹理颜色格式");
		return;
	}
	
	amplitude = amplitude / 255.0F;

	int32 _width = heightMap->GetWidth();
	int32 _hegiht = heightMap->GetHeight();

	float32 vh = float32(_hegiht / _width);
	float32 hh = float32(_width / _hegiht);

	if (heightMap->GetColorFormat() == ARGB_32)
	{
		uint32* p = (uint32*)heightMap->Lock();

		uint32* in = new uint32[_width * _hegiht];
		memcpy(in, p, _hegiht * _width * 4);

		for (int32 y = 0;y < _hegiht;y++)
		{
			for (int32 x = 0;x < _width;x++)
			{
				Vector3 h1(
					(x - 1) * hh,
					ReadHeightMap32(x-1, y, _width, _hegiht, in) * amplitude,
					y * vh);

				Vector3 h2(
					(x + 1) * hh,
					ReadHeightMap32(x + 1, y, pitch, _hegiht, in) * amplitude,
					y * vh);

				Vector3 v1(
					x * hh,
					ReadHeightMap32(x, y + 1, _width, _hegiht, in) * amplitude,
					(y + 1) * vh);

				Vector3 v2(
					x * hh,
					ReadHeightMap32(x, y - 1, _width, _hegiht, in) * amplitude,
					(y - 1) * vh);

				Vector3 s = v1 - v2;
				Vector3 t = h1 - h2;

				Vector3 normal = Cross(s, t);
				normal.Normalize();
				normal *= 0.5F;
				normal += Vector3(0.5F, 0.5F, 0.5F);
				normal *= 255.0F;

				float32 height = ReadHeightMap32(x,y,_width, _hegiht, in);

				p[y * _width + x] = ColorU32(
					uint32(height),
					uint32(normal.x),
					uint32(normal.y),
					uint32(normal.z)).GetColor();
			}
		}
		delete[] in;
		heightMap->Unlock();
	}
	else if (heightMap->GetColorFormat() == ARGB_1555)
	{
		uint16* p = (uint16*)heightMap->Lock();

		uint16* in = new uint16[_width * _hegiht];
		memcpy(in, p, _hegiht * _width * 2);

		for (int32 y = 0;y < _hegiht;y++)
		{
			for (int32 x = 0;x < _width;x++)
			{
				Vector3 h1(
					(x - 1) * hh,
					ReadHeightMap16(x - 1, y, _width, _hegiht, in) * amplitude,
					y * vh);

				Vector3 h2(
					(x + 1) * hh,
					ReadHeightMap16(x + 1, y, pitch, _hegiht, in) * amplitude,
					y * vh);

				Vector3 v1(
					x * hh,
					ReadHeightMap16(x, y + 1, _width, _hegiht, in) * amplitude,
					(y + 1) * vh);

				Vector3 v2(
					x * hh,
					ReadHeightMap16(x, y - 1, _width, _hegiht, in) * amplitude,
					(y - 1) * vh);

				Vector3 s = v1 - v2;
				Vector3 t = h1 - h2;

				Vector3 normal = Cross(s, t);
				normal.Normalize();
				normal *= 0.5F;
				normal += Vector3(0.5F, 0.5F, 0.5F);
				normal *= 255.0F;

				float32 height = ReadHeightMap16(x, y, _width, _hegiht, in);

				p[y * _width + x] = RGBA16(uint32(normal.x), uint32(normal.y), uint32(normal.z));
			}
		}
		delete[] in;
		heightMap->Unlock();
	}

}

/// <summary>
/// a 通道 生成随机因子，
/// </summary>
/// <param name="channel"></param>
/// <param name="minRange"></param>
/// <param name="maxRange"></param>
void Texture::GenerateRandomChannelNoise(uint8 channel, uint8 minRange, uint8 maxRange)
{
	for (int32 y = 0;y < height;y++)
	{
		for (int32 x = 0;x < width;x++)
		{
			uint32 color = GetColor(x, y);
			uint8 range = uint8(RangeRandom(minRange, maxRange));

			//清空8位头字节
			//设置8位头字节
			color &= ~((0xff) << (channel * 8));
			color |= (range << (channel * 8));

			SetColor(x, y, color);
		}
	}
}

uint32 Texture::GetAlphaMask()const
{
	switch (colorFormat)
	{
	case RGB_565:   return 0;
	case ARGB_1555: return 0x1 << 15;
	case RGB_24:    return 0;
	case ARGB_32:   return 0xFF000000;
	}
	return 0;
}

uint32 Texture::GetRedMask()const
{
	switch (colorFormat)
	{
	case RGB_565:   return 0x1F << 11;
	case ARGB_1555: return 0x1F << 10;
	case RGB_24:    return 0x00FF0000;
	case ARGB_32:   return 0x00FF0000;
	}
	return 0;
}

uint32 Texture::GetGreenMask()const
{
	switch (colorFormat)
	{
	case RGB_565:   return 0x1F << 5;
	case ARGB_1555: return 0x3F << 5;
	case RGB_24:    return 0x0000FF00;
	case ARGB_32:   return 0x0000FF00;
	}
	return 0;
}

uint32 Texture::GetBlueMask()const
{
	switch (colorFormat)
	{
	case RGB_565:   return 0x1F;
	case ARGB_1555: return 0x1F;
	case RGB_24:    return 0x000000FF;
	case ARGB_32:   return 0x000000FF;
	}
	return 0;
}

void Texture::Fill(ColorU32 color)
{
	uint32 c;

	switch (colorFormat)
	{
	case RGB_565:
	{
		c = color.ToRGB565();
		c |= c << 16;

		int32 length = width * height * 2;
		uint8* colorDatas = (uint8*)Lock();

		for (int32 i = 0;i < length;i++)
		{
			colorDatas[i] = uint8(c);
		}
	}break;
	case ARGB_1555:
	{
		c = color.ToARGB1555();
		c |= c << 16;

		int32 length = width * height * 2;
		uint8* colorDatas = (uint8*)Lock();

		for (int32 i = 0;i < length;i++)
		{
			colorDatas[i] = uint8(c);
		}

	}break;
	case RGB_24:
	{
		c = color.GetColor();

		int32 length = width * height * 3;
		uint8* colorDatas = (uint8*)Lock();

		for (int32 i = 0;i < length;i++)
		{
			colorDatas[i] = uint8(c);
		}
	}break;
	case ARGB_32:
	{
		c = color.GetColor();

		int32 length = width * height;
		uint32* colorDatas = (uint32*)Lock();

		for (int32 i = 0;i < length;i++)
		{
			colorDatas[i] = uint8(c);
		}

	}break;
	}

	Unlock();
}