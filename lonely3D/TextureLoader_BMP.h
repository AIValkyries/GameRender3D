#pragma once
#ifndef __TextureLoader_BMP_H__
#define __TextureLoader_BMP_H__

#include "IResourceLoader.h"

class GameHost;
class TextureLoader_BMP :public IResourceLoader
{
public:
	
	// 14 byte
    //设定为 1 字节对齐
#pragma pack(push)
#pragma pack(1)
	typedef struct 
	{
		uint16 bfType;              //	BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
									//	BA - OS/2 Bitmap Array
									//	CI - OS/2 Color Icon
									//	CP - OS/2 Color Pointer
									//	IC - OS/2 Icon
									//	PT - OS/2 Pointer

		uint32 bfSize;
		uint16 bfReserved1;
		uint16 bfReserved2;
		uint32 bfOffBits;
	}BitMapFileHeader;

	// 40 byte
    //设定为 1 字节对齐
    typedef struct 
	{
		uint32 bfSize;   // 这个结构所需要的字数
		uint32 biWidth;
		uint32 biHeight;
		uint16 biPlanes;  //总会被设为1
		uint16 biBitCount;
		uint32 biCompression;
		uint32 biSizeImage;
		uint32 biXPelsPerMeter;
		uint32 biYPelsPerMeter;
		uint32 biClrUsed;
		uint32 biClrImportant;
	}BitMapInfoHeader;

#pragma pack(pop)

	typedef struct 
	{
		uint8 peRed;
		uint8 peGreen;
		uint8 peBlue;
		uint8 peFlags;
	}Paletteentry;

	virtual bool IsALoadableFileExtension(const string& fileName);
	virtual bool Loader(IFile* file, IResourceItem* item);
	virtual void SetGameHost(GameHost* _gameHost);

private:
	void Decompress8BitRLE(
		uint8*& bmpData, uint32 size,
		uint32 width, uint32 height, uint32 pitch);

	void Decompress4BitRLE(
		uint8* bmpData, uint32 size,
		uint32 width, uint32 height, uint32 pitch);
};


#endif
