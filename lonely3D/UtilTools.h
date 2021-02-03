#pragma once
#ifndef __lUtilTools_H__
#define __lUtilTools_H__

#include "DataType.h"
#include "Texture.h"
#include <string>
#include "Material.h"

using namespace std;


#define SafeDelete(p)      { if(p!=NULL) { delete   (p);     (p)=NULL; } }
#define SafeDeleteArray(p) { if(p!=NULL) { delete[] (p);     (p)=NULL; } }

inline bool IsFileExtension(const string& fileName, const string& ext, const string& ext2 = "")
{
	int32 extPos = fileName.find_last_of(".");
	if (extPos <= 0)
		return false;

	extPos += 1;

	string extName = fileName.substr(extPos, fileName.length());
	if (extName == ext)
	{
		return true;
	}
	if (extName == ext2)
	{
		return true;
	}

	return false;
}

//这是错误的写法 TODO
inline void CopyTo(Texture* dest, Texture* source)
{
	if (!dest || !source)
		return;

	for (int32 y = 0; y < dest->GetHeight();y++)
	{
		for (int32 x = 0;x < dest->GetWidth();x++)
		{
			uint32 c = source->GetColor(x, y);
			dest->SetColor(x, y, c);
		}
	}
}

const ColorU32 WHITE(255, 255, 255, 255);
const ColorU32 RED(0, 255, 0, 0);
const ColorU32 GREEN(0, 0, 255, 0);
const ColorU32 BLUE(0, 0, 0, 255);
const ColorU32 YELLOW(0, 255, 255, 0);
const ColorU32 CYAN(0, 0, 255, 255);
const ColorU32 BLACK(0, 0, 0, 0);
const ColorU32 GRAY(0, 150, 150, 150);

const ColorF WHITE_F(1.0F, 1.0F, 1.0F, 1.0F);
const ColorF RED_F(0, 1.0F, 0, 0);
const ColorF GREEN_F(0, 0, 1.0F, 0);
const ColorF BLUE_F(0, 0, 0, 1.0F);
const ColorF YELLOW_F(0, 1.0F, 1.0F, 0);
const ColorF CYAN_F(0, 0, 1.0F, 1.0F);
const ColorF BLACK_F(0, 0, 0, 0);
const ColorF GRAY_F(0, 0.58F, 0.58F, 0.58F);

inline lMaterial InitMtrl(ColorF a, ColorF d, ColorF s, ColorF e, float32 p)
{
	lMaterial material;
	material.AmbientColor = a;
	material.DiffuseColor = d;
	material.SpecularColor = s;
	material.EmissiveColor = e;
	material.Power = p;

	return material;
}


const lMaterial WHITE_MTRL = InitMtrl(WHITE_F, WHITE_F, WHITE_F, BLACK_F, 2.0F);
const lMaterial RED_MTRL = InitMtrl(RED_F, RED_F, RED_F, BLACK_F, 2.0f);
const lMaterial GREEN_MTRL = InitMtrl(GREEN_F, GREEN_F, GREEN_F, BLACK_F, 2.0f);
const lMaterial BLUE_MTRL = InitMtrl(BLUE_F, BLUE_F, BLUE_F, BLACK_F, 2.0f);
const lMaterial YELLOW_MTRL = InitMtrl(YELLOW_F, YELLOW_F, YELLOW_F, BLACK_F, 2.0f);

  


#endif
