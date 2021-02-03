#pragma once
#ifndef __Texture_H__
#define __Texture_H__

#include "DataType.h"
#include "Color.h"
#include "lMathf.h"
#include <string>
#include "IResourceItem.h"
#include "Types.h"
#include "Rect.h"
#include "Debug.h"


using namespace std;

/*
*  uv ���귶Χ
   (0,0)------(1,0)
	|         | 
	|         |  
	|         | 
   (0,1)-----(1,1)
*/
class Texture :public IResourceItem
{
public:
	Texture();
	Texture(uint16 _width, uint16 _height, COLOR_FORMAT _colorFormat);
	~Texture();

	void CreateTexture(
		COLOR_FORMAT _colorFormat,
		uint32 _width, uint32 _height);

	void CreateTexture(
		COLOR_FORMAT _colorFormat, 
		uint32 _width, uint32 _height, 
		void* data);

	/// <summary>
	///  
	/// </summary>
	/// <param name="scale">����ԽСƵ��Խ��</param>
	/// <param name="octaves">������ӵ�����</param>
	/// <param name="falloff">���Խ����θߵ�Խ����	</param>
	void GeneratePerlinNoise(float32 scale,float32 octaves,float32 falloff);
	void GenerateMipping();
	void GenerateNormalMap(Texture* heightMap,float32 amplitude);

	// �������ͨ��
	void GenerateRandomChannelNoise(uint8 channel,uint8 minRange,uint8 maxRange);

	Texture* GetTextureByMipLevel(uint16 level) { if (mipLevel > level) return mipMaps[level];return 0; }
	uint16 GetMipLevel()const { return mipLevel; }

	ColorU32 GetColorU32(int32 x,int32 y);
	uint32 GetColor(int32 x, int32 y);
	void SetColor(int32, int32 y, uint32 color);

	ColorF GetPixelF(float32 u,float32 v)const;
	ColorU32 GetPixel(float32 u, float32 v)const;
	void SetPixel(uint16 x, uint16 y, ColorU32 color, float32 blend = 0.0F)const;

	// ˫���Թ���
	uint16 GetBilinearSampling16(uint16 x, uint16 y, float32 u, float32 v)const;
	uint32 GetBilinearSampling32(uint16 x, uint16 y, float32 u, float32 v)const;

	// ��ȡÿ���ض���bit������ɫ����
	static uint16 GetBitsPerPixelFromFormat(COLOR_FORMAT colorFormat)
	{
		switch (colorFormat)
		{
		case RGB_565:   return 16;
		case ARGB_1555: return 16;
		case RGB_24:    return 24;
		case ARGB_32:   return 32;
		}
		return 0;
	}

	void* Lock() { return datas; }
	void Unlock() {}

	void SetFilterMode(const int32 _filterMode)
	{
		filterMode = _filterMode;
	}

	void SetWarpMode(const int32 _warpMode)
	{
		warpMode = _warpMode;
	}

	void SetFileName(const string _fileName) { fileName = _fileName; }
	const string& GetFileName()const { return fileName; }

	int32 GetWarpMode()const { return warpMode; }
	int32 GetFilterMode()const { return filterMode; }
	COLOR_FORMAT GetColorFormat()const { return colorFormat; }

	uint16 GetWidth()const { return width; }
	uint16 GetHeight()const { return height; }

	uint16 GetBytesPerPixel()const { return bytesPerPixel; }
	uint32 GetBitsPerPixel()const
	{
		switch (colorFormat)
		{
		case ARGB_1555:
			return 16;
		case RGB_565:
			return 16;
		case RGB_24:
			return 24;
		case ARGB_32:
			return 32;
		}

		return 0;
	}
	uint32 GetPitch()const { return pitch; }
	uint32 GetLength()const { return length; }

	uint32 GetAlphaMask()const;
	uint32 GetRedMask()const;
	uint32 GetGreenMask()const;
	uint32 GetBlueMask()const;

	Rect* GetRect()
	{
		if (rect == 0)
		{
			rect = new Rect();
			rect->x = 0;
			rect->y = 0;
			rect->width = width;
			rect->height = height;
		}
		return rect;
	}
	void Fill(ColorU32 color);

protected:
	uint16 width;
	uint16 height;
	COLOR_FORMAT colorFormat;
	uint8* datas;
	uint32 length;           // ���ֽ�

private:
	string fileName;
	uint32 pitch;            // ÿ���ֽ���
	int32 warpMode;
	int32 filterMode;

	uint16 bytesPerPixel;  // ÿ���ض����ֽ�
	Rect* rect;

	Texture** mipMaps;
	uint16 mipLevel;

	void InitData();
};

inline Texture::Texture()
{
	rect = 0;
	fileName = "Texture";
	datas = 0;
	warpMode = REPEAT;
	filterMode = FILTER_POINT;
	colorFormat = RGB_24;

	width = 0;
	height = 0;

	bytesPerPixel = 0;
	pitch = 0;
	length = 0;

	mipMaps = 0;
	mipLevel = 0;
}

inline Texture::Texture(
	uint16 _width, uint16 _height,
	COLOR_FORMAT _colorFormat)
{
	width = _width;
	height = _height;
	colorFormat = _colorFormat;

	InitData();
}




#endif

