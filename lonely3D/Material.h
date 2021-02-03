#pragma once
#ifndef __lMaterial_H__
#define __lMaterial_H__

#include "DataType.h"
#include "Color.h"

struct lMaterial
{
	ColorF AmbientColor;
	ColorF DiffuseColor;
	ColorF SpecularColor;
	ColorF EmissiveColor;
	float32 Power;
};

#endif
