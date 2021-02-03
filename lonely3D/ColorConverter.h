#pragma once
#ifndef __ColorConverter_H__
#define __ColorConverter_H__

#include "DataType.h"
#include "TextureLoader_BMP.h"

class ColorConverter
{
public:
	static void Convert1BitTo16Bit(
		uint8* inData, uint16* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false);

	static void Convert4BitTo16Bit(
		uint8* intData, uint16* outData,
		uint32 width, uint32 height, uint32* paletteentry,uint32 linepad = 0, bool flip = false);

	static void Convert8BitTo16Bit(
		uint8* inData, uint16* outData,
		uint32 width, uint32 height, uint32* paletteentry, uint32 linepad = 0, bool flip = false);

	static void Convert8BitTo24Bit(
		uint8* inData, uint8* outData,
		uint32 width, uint32 height, uint32* paletteentry, uint32 linepad = 0, bool flip = false);

	static void Convert8BitTo32Bit(
		uint8* inData, uint32* outData,
		uint32 width, uint32 height, uint32* paletteentry, uint32 linepad = 0, bool flip = false);

	static void Convert16BitTo16Bit(
		uint16* inData, uint16* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false);
	
	static void Convert16To24Bit(
		uint16* inData, uint8* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false);

	static void Convert16To32Bit(
		uint16* inData, uint32* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false);

	static void Convert24To24Bit(
		uint8* inData, uint8* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false, bool bgr = false);

	static void Convert32To32Bit(
		uint32* inData, uint32* outData,
		uint32 width, uint32 height, uint32 linepad = 0, bool flip = false);
};

#endif

