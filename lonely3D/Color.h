#pragma once
#ifndef __lColor_H__
#define __lColor_H__

#include "DataType.h"
#include "lMathf.h"
#include "Types.h"

inline uint16 RGBA16(uint32 r, uint32 g, uint32 b, uint32 a = 0xFF)
{
	return (uint16)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}


inline uint16 RGB_565_TO_ARGB_1555(uint16 rgb565)
{
	uint16 rgb_1555 =
		(0x8000) |
		((rgb565 >> 11) & 0x1F) |
		((rgb565 >> 6) & 0x1F) | 
		(rgb565 & 0x1F);
	return rgb_1555;
};

inline uint32 RGB_565_TO_RGB_24(uint16 rgb565)
{
	uint32 rgb24 = 0x00000000 |
		(((rgb565 >> 11) & 0x1F) << 16) |
		(((rgb565 >> 5) & 0x3F) << 8) |
		((rgb565 & 0x1F));

	return rgb24;
}

inline uint32 RGB_565_TO_ARGB_32(uint16 rgb565)
{
	return 0xFF000000 |
		((rgb565 & 0xF800) << 8) |
		((rgb565 & 0x07E0) << 5) |
		((rgb565 & 0x001F) << 3);
}

inline uint16 ARGB_1555_TO_RGB_565(uint16 rgb1555)
{
	uint16 rgb565 = 
		((rgb1555 >> 10) & 0x1F) |
		((rgb1555 >> 5) & 0x1F) |
		(rgb1555 & 0x1F);
	return rgb565;
}

inline uint32 ARGB_1555_TO_RGB_24(uint16 rgb1555)
{
	uint32 rgb24 = 0x00000000 |
		((rgb1555 >> 10) & 0x1F) << 16 |
		((rgb1555 >> 5) & 0x1F) << 8|
		(rgb1555 & 0x1F);

	return rgb24;
}

inline uint32 ARGB_1555_TO_ARGB_32(uint16 rgb1555)
{
	// 7C00 = 15位，5个1位
	// 7000 = 15位，3个1位
	// 3E0 = 10位，5个1位
	// 380 = 10位，3个1位
	// 1F = 5位，全1位
	// 1C = 5位，3个1位
	return (
		((( -(int32) rgb1555 & 0x00008000) >> (int32)31) & 0xFF000000) |
		((rgb1555 & 0x00007C00) << 9) |
		((rgb1555 & 0x00007000) << 4) |

		((rgb1555 & 0x000003E0) << 6) |
		((rgb1555 & 0x00000380) << 1) |

		((rgb1555 & 0x0000001F) << 3) |
		((rgb1555 & 0x0000001C) >> 2)
		);
}


inline uint32 RGB_24_TO_AARGB_32(uint32 rgb24)
{
	uint32 rgb32 = 0x00000000 | rgb24;
	return rgb32;
}

inline uint16 ARGB_32_TO_RGB_565(uint32 rgb32)
{
	uint16 rgb_565 = (uint16)((rgb32 & 0x00F80000) >> 8 |
			(rgb32 & 0x0000FC00) >> 5 |
			(rgb32 & 0x000000F8) >> 3);

	return rgb_565;
}

// x 为保留通道
inline uint16 X8R8G8B8_TO_A1R5G5B5(uint32 rgb32)
{
	return (uint16)(0x8000 |
		(rgb32 & 0x00F80000) >> 9 |
		(rgb32 & 0x0000F800) >> 6 |
		(rgb32 & 0x000000F8) >> 3);
}

inline uint16 ARGB_32_TO_ARGB_1555(uint32 rgb32)
{
	return (uint16)((rgb32 & 0x80000000) >> 16 |
		(rgb32 & 0x00F80000) >> 9 |
		(rgb32 & 0x0000F800) >> 6 |
		(rgb32 & 0x000000F8) >> 3);
}

inline uint32 ARGB_32_TO_RGB_24(uint32 rgb32)
{
	uint32 rgb24 = rgb32 & 0x00FFFFFF;
	return rgb24;
}

inline uint32 GetRed(uint16 color)
{
	return ((color >> 10) & 0x1F);
}


inline uint32 GetGreen(uint16 color)
{
	return ((color >> 5) & 0x1F);
}

inline uint32 GetBlue(uint16 color)
{
	return (color & 0x1F);
}

inline uint32 GetAverage(uint16 color)
{
	return ((GetRed(color) << 3) + (GetGreen(color) << 3) + (GetBlue(color) << 3)) / 3;
}


inline uint32 GetRed(uint32 c)
{
	return (c >> 16) & 0xff;
}

inline uint32 GetGreen(uint32 c)
{
	return (c >> 8) & 0xff;
}

inline uint32 GetBlue(uint32 c)
{
	return (c) & 0xff;
}

inline uint32 GetAverage(uint32 color)
{
	return ((GetRed(color)) + (GetGreen(color)) + (GetBlue(color))) / 3;
}

inline float32 Intensity32(uint32 color)
{
	float32 r = (float32)GetRed(color);
	float32 g = (float32)GetGreen(color);
	float32 b = (float32)GetBlue(color);

	const float32 average = (r + g + b) / 3;
	return average / 255.0F;
}

inline float32 Intensity16(uint16 color)
{
	float32 r = (float32)GetRed(color);
	float32 g = (float32)GetGreen(color);
	float32 b = (float32)GetBlue(color);

	const float32 average = (r + g + b) / 3;
	return average / 255.0F;
}

// 0.2126 
inline float32 GetLuminance(uint32 color)
{
	return (GetRed(color) * 0.2126f) + GetGreen(color) * 0.59f + GetBlue(color) * 0.11f;
}

class ColorF
{
public:
	ColorF() : a(0.0F), r(0.0F), g(0.0F), b(0.0F) {}
	ColorF(float32 value)
	{
		a = value;r = value;g = value;b = value;
	}
	ColorF(float32 _a, float32 _r, float32 _g, float32 _b)
	{
		Set(_a, _r, _g, _b);
	}
	ColorF(float32 _r, float32 _g, float32 _b)
	{
		Set(_r, _g, _b);
	}

	ColorF operator+(const ColorF& color)
	{
		ColorF c;
		c.r = r - color.r;
		c.g = g - color.g;
		c.b = b - color.b;
		c.a = a - color.a;

		return c;
	}

	ColorF& operator+=(const ColorF& color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		a += color.a;

		return *this;
	}

	ColorF operator-(const ColorF& color)
	{
		ColorF c;
		c.r = r - color.r;
		c.g = g - color.g;
		c.b = b - color.b;
		c.a = a - color.a;

		return c;
	}

	ColorF& operator*=(const ColorF& color)
	{
		r *= color.r;
		g *= color.g;
		b *= color.b;
		a *= color.a;

		Clamp(r, 0.0F, 1.0F);
		Clamp(g, 0.0F, 1.0F);
		Clamp(b, 0.0F, 1.0F);
		Clamp(a, 0.0F, 1.0F);

		return *this;
	}

	ColorF operator*(const ColorF& color)
	{
		ColorF c;
		c.r = r * color.r;
		c.g = g * color.g;
		c.b = b * color.b;
		c.a = a * color.a;

		Clamp(c.r, 0.0F, 1.0F);
		Clamp(c.g, 0.0F, 1.0F);
		Clamp(c.b, 0.0F, 1.0F);
		Clamp(c.a, 0.0F, 1.0F);

		return c;
	}

	ColorF operator*(const float32& k)
	{
		ColorF c;
		c.r = Clamp(r * k, 0.0F, 1.0F);
		c.g = Clamp(g * k, 0.0F, 1.0F);
		c.b = Clamp(b * k, 0.0F, 1.0F);
		c.a = Clamp(a * k, 0.0F, 1.0F);
		return c;
	}

	ColorF& AddEqual(ColorF color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		a += color.a;

		Clamp(r, 0.0F, 1.0F);
		Clamp(g, 0.0F, 1.0F);
		Clamp(b, 0.0F, 1.0F);
		Clamp(a, 0.0F, 1.0F);

		return *this;
	}

	void Set(float32 _a, float32 _r, float32 _g, float32 _b)
	{
		if (_r > 1)
			r = _r / 255.0F;
		else
			r = _r;

		if (_g > 1.0F)
			g = _g / 255.0F;
		else
			g = _g;

		if (_b > 1.0F)
			b = _b / 255.0F;
		else
			b = _b;

		if (_a > 1.0F)
			a = _a / 255.0F;
		else
			a = _a;
	}

	void Set(float32 _r, float32 _g, float32 _b)
	{
		if (_r > 1)
			r = _r / 255.0F;
		else
			r = _r;

		if (_g > 1.0F)
			g = _g / 255.0F;
		else
			g = _g;

		if (_b > 1.0F)
			b = _b / 255.0F;
		else
			b = _b;

		a = 1.0F;
	}

	// R=0, G=1, B=2, A=3
	void SetColorComponentValue(int32 index, float32 value)
	{
		switch (index)
		{
		case 0:r = value; break;
		case 1:g = value; break;
		case 2:b = value; break;
		case 3:a = value; break;
		}
	}

	void SetRed(float32 _r)
	{
		r = _r;
	}
	void SetGreen(float32 _g)
	{
		g = _g;
	}
	void SetBlue(float32 _b)
	{
		b = _b;
	}
	void SetAlpha(float32 _a)
	{
		a = _a;
	}

	void Saturate()
	{
		r = Min(r, 1.0F);
		g = Min(g, 1.0F);
		b = Min(b, 1.0F);
		a = Min(a, 1.0F);
	}

	float32& GetRed() { return r; }
	float32& GetGreen() { return g; }
	float32& GetBlue() { return b; }
	float32& GetAlpha() { return a; }

	float32 GetRed() const{ return r; }
	float32 GetGreen() const { return g; }
	float32 GetBlue() const { return b; }
	float32 GetAlpha()  const { return a; }

private:
	float32 r, g, b, a;
};



//色彩三要素:H、S、L(Lightness) （色相、饱和度、亮度）
class ColorU32
{
public:
	ColorU32() :color(0) {}
	ColorU32(uint32 r, uint32 g, uint32 b)
	{
		color = (0xFF << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xff);
	}
	ColorU32(uint32 a, uint32 r, uint32 g, uint32 b)
	{
		color = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xff);
	}
	ColorU32(uint32 src) :color(src) {}
	ColorU32(const ColorF& c)
	{
		uint8 a = (uint8)(c.GetAlpha() * 255);
		uint8 r = (uint8)(c.GetRed() * 255);
		uint8 g = (uint8)(c.GetGreen() * 255);
		uint8 b = (uint8)(c.GetBlue() * 255);

		color = ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
	}

	ColorF ToColorF()
	{
		float32 inv_avg = 1.0F / 255.0F;
		float32 r = inv_avg * GetRed();
		float32 g = inv_avg * GetGreen();
		float32 b = inv_avg * GetBlue();
		float32 a = inv_avg * GetAlpha();
		return ColorF(a, r, g, b);
	}

	ColorU32& operator=(const ColorU32& other)
	{
		color = other.color;
		return *this;
	}
	ColorU32& operator=(const uint32& c)
	{
		color = c;
		return *this;
	}

	ColorU32& operator=(const ColorF& other)
	{
		uint8 a = uint8(other.GetAlpha() * 255);
		uint8 r = uint8(other.GetRed() * 255);
		uint8 g = uint8(other.GetGreen() * 255);
		uint8 b = uint8(other.GetBlue() * 255);

		color = ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
		
		return *this;
	}
	ColorU32 operator+(const ColorU32& src)const
	{
		return ColorU32(
			Min(GetAlpha() + src.GetAlpha(), 255),
			Min(GetRed() + src.GetRed(), 255),
			Min(GetGreen() + src.GetGreen(), 255),
			Min(GetBlue() + src.GetBlue(), 255));
	}

	ColorU32 operator*(const float32& k)const
	{
		ColorU32 c;
		c.color = (uint32)(color * k);

		return c;
	}

	ColorU32& operator*=(const ColorU32& other)
	{
		uint8 a = Clamp(GetAlpha() * other.GetAlpha(), 0, 255);
		uint8 r = Clamp(GetRed() * other.GetRed(), 0, 255);
		uint8 g = Clamp(GetGreen() * other.GetGreen(), 0, 255);
		uint8 b = Clamp(GetBlue() * other.GetBlue(), 0, 255);

		color = (a << 24) | (r << 16) | (g << 8) | b;

		return *this;
	}

	bool operator ==(const ColorU32& other)const
	{
		return color == other.color;
	}
	bool operator !=(const ColorU32& other)const
	{
		return color != other.color;
	}
	bool operator>(const ColorU32& other)const
	{
		return color > other.color;
	}
	bool operator<(const ColorU32& other)const
	{
		return color < other.color;
	}

	bool operator ==(const ColorU32& other)
	{
		return color == other.color;
	}
	bool operator !=(const ColorU32& other)
	{
		return color != other.color;
	}
	bool operator>(const ColorU32& other)
	{
		return color > other.color;
	}
	bool operator<(const ColorU32& other)
	{
		return color < other.color;
	}

	ColorU32 operator*(const ColorU32& other)const
	{
		ColorU32 c;

		uint32 u = color;
		uint32 v = other.color;

		uint32 result = (((u & 0x000000ff) * ((v & 0x000000ff) + 1)) >> 8) & 0x000000ff;
		result |= (((u & 0x0000ff00) >> 8) * (((v & 0x0000ff00) >> 8) + 1)) & 0x0000ff00;
		result |= (((u & 0x00ff0000) * (((v & 0x00ff0000) >> 16) + 1)) >> 8) & 0x00ff0000;
		result |= (((u & 0xff000000) >> 8) * (((v & 0xff000000) >> 24) + 1)) & 0xff000000;

		c.color = result;

		return c;
	}

	ColorU32& operator+=(const ColorU32& src)
	{
		uint8 a = GetAlpha();
		uint8 r = GetRed();
		uint8 g = GetGreen();
		uint8 b = GetBlue();

		uint8 sa = src.GetAlpha();
		uint8 sr = src.GetRed();
		uint8 sg = src.GetGreen();
		uint8 sb = src.GetBlue();

		uint8 ca = Clamp(a + sa, 0, 255);
		uint8 cr = Clamp(r + sr, 0, 255);
		uint8 cg = Clamp(g + sg, 0, 255);
		uint8 cb = Clamp(b + sb, 0, 255);

		color = (ca << 24) | (cr << 16) | (cg << 8) | cb;

		return *this;
	}

	ColorU32& AddEqual(const ColorU32& src)
	{
		uint8 a = GetAlpha();
		uint8 r = GetRed();
		uint8 g = GetGreen();
		uint8 b = GetBlue();

		uint8 sa = src.GetAlpha();
		uint8 sr = src.GetRed();
		uint8 sg = src.GetGreen();
		uint8 sb = src.GetBlue();

		uint8 ca = Clamp(a + sa, 0, 255);
		uint8 cr = Clamp(r + sr, 0, 255);
		uint8 cg = Clamp(g + sg, 0, 255);
		uint8 cb = Clamp(b + sb, 0, 255);

		color = (ca << 24) | (cr << 16) | (cg << 8) | cb;

		return *this;
	}

	void SetData(const void* data, int32 _format)
	{
		switch (_format)
		{
		case RGB_565:
			color = RGB_565_TO_ARGB_32(*(uint16*)data);
			break;
		case ARGB_1555:
			color = ARGB_1555_TO_ARGB_32(*(uint16*)data);
			break;
		case RGB_24:
			color = *(uint32*)data;
			break;
		case ARGB_32:
			color = *(uint32*)data;
			break;
		default:
			color = 0xFFFFFFFF;
			break;
		}
	}

	void GetData(void* data, int32 _format)
	{
		switch (_format)
		{
		case RGB_565:
		{
			uint16* dest = (uint16*)data;
			*dest = ARGB_32_TO_RGB_565(color);
		}break;
		case ARGB_1555:
		{
			uint16* dest = (uint16*)data;
			*dest = ARGB_32_TO_ARGB_1555(color);
		}break;
		case RGB_24:
		{
			uint32* dest = (uint32*)data;
			*dest = ARGB_32_TO_RGB_24(color);
		}break;
		case ARGB_32:
		{
			uint32* dest = (uint32*)data;
			*dest = color;
		}break;
		default:
		break;
		}
	}

	void Set(uint32 a, uint32 r, uint32 g, uint32 b)
	{
		color = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
	}

	void Set(uint32 r, uint32 g, uint32 b)
	{
		color = (0xFF000000) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
	}

	void Set(uint32 src)
	{
		color = src;
	}

	void SetAlpha(uint32 alpha)
	{
		uint32 a = (alpha & 0xFF) << 24;
		color = a | (color & 0x00FFFFFF);
	}

	void SetRed(uint32 red)
	{
		uint32 r = (red & 0xFF) << 16;
		color = r | (color & 0xFF00FFFF);
	}

	void SetGreen(uint32 green)
	{
		uint32 g = (green & 0xFF) << 8;
		color = g | (color & 0xFFFF00FF);
	}

	void SetBlue(uint32 blue)
	{
		uint32 b = (blue & 0xFF);
		color = b | (color & 0xFFFFFF00);
	}

	uint32 GetAverage()const
	{
		return (GetRed() + GetGreen() + GetBlue()) / 3;
	}

	uint8 GetAlpha()const { return (color >> 24); }
	uint8 GetRed()const { return ((color >> 16) & 0xFF); }
	uint8 GetGreen()const { return ((color >> 8) & 0xFF); }
	uint8 GetBlue()const { return (color & 0xFF); }

	uint32 GetColor()const { return color; }
	uint32 InversionColor()const
	{
		uint32 c = 0;
		
		c = uint32(GetBlue() << 24) + uint32(GetGreen() << 16) + uint32(GetRed() << 8) + uint32(GetAlpha());
		return c;
	}
	// 亮度 [0,255] 亮度
	float32 GetLightness()const
	{
		uint8 r = GetRed();
		uint8 g = GetGreen();
		uint8 b = GetBlue();

		uint8 maxValue = Max(r, g);
		maxValue = Max(maxValue, b);

		uint8 minValue = Min(r, g);
		minValue = Min(minValue, b);

		return 0.5f * (float32)(maxValue + minValue);
	}

	uint16 ToARGB1555()const { return ARGB_32_TO_ARGB_1555(color); }
	uint16 ToRGB565()const { return ARGB_32_TO_RGB_565(color); }
	uint32 ToRGB24()const { return ARGB_32_TO_RGB_24(color); }

	void Saturate()
	{
		SetRed(Min(GetRed(), 255));
		SetGreen(Min(GetGreen(), 255));
		SetBlue(Min(GetBlue(), 255));
		SetAlpha(Min(GetAlpha(), 255));
	}

private:
	uint32 color;
};


inline ColorF Lerp(const ColorF& c1, const ColorF& c2, const float32& t)
{
	ColorF c;
	
	c.SetAlpha(Lerp(c1.GetAlpha(), c2.GetAlpha(), t));
	c.SetRed(Lerp(c1.GetRed(), c2.GetRed(), t));
	c.SetGreen(Lerp(c1.GetGreen(), c2.GetGreen(), t));
	c.SetBlue(Lerp(c1.GetBlue(), c2.GetBlue(), t));

	return c;
}

inline ColorU32 Lerp(const ColorU32& c1, const ColorU32& c2, const float32& t)
{
	ColorU32 c;

	c.SetAlpha(Lerp(c1.GetAlpha(), c2.GetAlpha(), t));
	c.SetRed(Lerp(c1.GetRed(), c2.GetRed(), t));
	c.SetGreen(Lerp(c1.GetGreen(), c2.GetGreen(), t));
	c.SetBlue(Lerp(c1.GetBlue(), c2.GetBlue(), t));

	return c;
}


#endif
