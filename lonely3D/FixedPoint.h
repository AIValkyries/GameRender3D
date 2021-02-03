#pragma once
#ifndef __FixedPoint64_H__
#define __FixedPoint64_H__

#include "DataType.h"

#define FIXP16_SHIFT   16
#define FIXP16_MAX     65536
#define FIXP16_MASK    0x0000ffff
#define FIXP16_WP_MASK 0xffff0000

// ¶¨µãÊý
class FixedPoint64
{
public:
	FixedPoint64() {}
	FixedPoint64(uint8 i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
	}
	FixedPoint64(int32 i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
	}
	FixedPoint64(uint32 i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
	}

	FixedPoint64(float32 f)
	{
		data = int64(f * (1 << FIXP16_SHIFT));
	}
	FixedPoint64(float64 f)
	{
		data = int64(f * (1 << FIXP16_SHIFT));
	}
	FixedPoint64(uint64 v)
	{
		data = v;
	}

	FixedPoint64(int64 v)
	{
		data = v;
	}

	FixedPoint64& operator=(const uint8& i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
		return *this;
	}
	FixedPoint64& operator=(const int32& i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
		return *this;
	}
	FixedPoint64& operator=(const uint32& i)
	{
		data = ((uint64)i << FIXP16_SHIFT);
		return *this;
	}
	FixedPoint64& operator=(const uint64& f)
	{
		data = f;
	}
	FixedPoint64& operator=(const int64& f)
	{
		data = f;
	}
	FixedPoint64& operator=(const float32& f)
	{
		data = (int64)(f * (1 << FIXP16_SHIFT));
		return *this;
	}
	FixedPoint64& operator=(const float64& f)
	{
		data = int64(f * (1 << FIXP16_SHIFT));
		return *this;
	}

	FixedPoint64& operator=(const FixedPoint64& f)
	{
		data = f.data;
		return* this;
	}

	FixedPoint64& operator +=(const FixedPoint64& f)
	{
		data += f.data;
		return *this;
	}

	FixedPoint64& operator-=(const FixedPoint64& f)
	{
		data -= f.data;
		return *this;
	}

	FixedPoint64& operator*=(const float32& k)
	{
		data = int64(data * k);
		return *this;
	}

	FixedPoint64& operator*=(const FixedPoint64& f)
	{
		data *= f.data;
		data >>= FIXP16_SHIFT;
		return* this;
	}

	FixedPoint64& operator/=(const FixedPoint64& f)
	{
		uint64 v = data << FIXP16_SHIFT;
		data = v / f.data;
		return *this;
	}

	int64 GetData()const { return data; }
	int64 GetData() { return data; }

	float32 ToFloat()const
	{
		return (data >> FIXP16_SHIFT) + (data & FIXP16_MASK) / (float32)(FIXP16_MASK);
	}

	float32 ToFloat()
	{
		return (data >> FIXP16_SHIFT) + (data & FIXP16_MASK) / (float32)(FIXP16_MASK);
	}

	uint32 ToUint32()const
	{
		return (uint32)(data >> FIXP16_SHIFT);
	}
	uint32 ToUint32()
	{
		return (uint32)(data >> FIXP16_SHIFT);
	}

	int32 ToInt32()const
	{
		return int32(data >> FIXP16_SHIFT);
	}
	int32 ToInt32()
	{
		return int32(data >> FIXP16_SHIFT);
	}


private:

	int64 data;
};

inline FixedPoint64 operator+(const FixedPoint64& a, const FixedPoint64& b)
{
	return FixedPoint64(a.GetData() + b.GetData());
}
inline  FixedPoint64 operator-(const FixedPoint64& a, const FixedPoint64& b)
{
	return FixedPoint64(a.GetData() - b.GetData());
}

inline FixedPoint64 operator*(const FixedPoint64& a, const float32& k)
{
	int64 data = a.GetData();

	int64 v = int64(data * k);

	return  FixedPoint64(v);
}

inline FixedPoint64 operator*(const FixedPoint64& a, const FixedPoint64& b)
{
	return FixedPoint64((a.GetData() * b.GetData())>> FIXP16_SHIFT);
}

inline FixedPoint64 operator/(const FixedPoint64& a, const FixedPoint64& b)
{
	uint64 v = a.GetData() << FIXP16_SHIFT;
	return FixedPoint64(v / b.GetData());
}

inline FixedPoint64 Lerp(const FixedPoint64& form,const FixedPoint64& to,float32 t)
{
	FixedPoint64 lerp1 = (1 - t) * form;
	FixedPoint64 lerp2 = t * to;

	return FixedPoint64(lerp1 + lerp2);
}

#endif

