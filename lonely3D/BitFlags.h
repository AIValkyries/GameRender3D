#pragma once
#ifndef __BitFlags_H__
#define __BitFlags_H__

#include "DataType.h"

#define FLAG(x) (1<<(x))
#define TEST_BIT(flag,bit) ((flag.value & (FLAG(bit)))!=0)
#define SET_BIT(flag,bit) ((flag.value) |= (FLAG(bit)))
#define CLEAR_BIT(flag,bit) ((flag.value) &= (~FLAG(bit)))


template<class T>
class BitFlags
{
public:
	BitFlags();
	BitFlags(const T data);
	BitFlags(const BitFlags& src);
	~BitFlags();

	BitFlags& operator=(const BitFlags& src);
	bool operator==(const BitFlags& src);
	bool operator!=(const BitFlags& src);

	T value;

	void SetFlags(uint32 flags);
	void ClearFlags(uint32 flags);
	void ClearFlagsAll();
	bool TestFlags(uint32 flags);
	void SetBit(int32 bit);
	void ClearBit(int32 bit);
	void ClearBitAll();
	bool TestBit(int32 bit);

};

#pragma region operator or Constructor
template<class T>
inline BitFlags<T>::BitFlags() {}

template<class T>
inline BitFlags<T>::BitFlags(const T data) 
{
	value = data;
}

template<class T>
inline BitFlags<T>::BitFlags(const BitFlags& src)
{
	value = src.value;
}

template<class T>
inline BitFlags<T>::~BitFlags() {}

template<class T>
BitFlags<T>& BitFlags<T>::operator=(const BitFlags& src)
{
	value = src.value;
	return *this;
}

template<class T>
bool BitFlags<T>::operator==(const BitFlags& src)
{
	return value == src.value;
}

template<class T>
bool BitFlags<T>::operator!=(const BitFlags& src)
{
	return value != src.value;
}

#pragma endregion

template<class T>
void BitFlags<T>::SetBit(int32 bit)
{
	value |= 1 << bit;
}

template<class T>
void BitFlags<T>::ClearBit(int32 bit)
{
	value &= ~(1 << bit);
}

template<class T>
void BitFlags<T>::ClearBitAll()
{
	value = 0;
}

template<class T>
bool BitFlags<T>::TestBit(int32 bit)
{
	return value & (1 << bit);
}

template<class T>
void BitFlags<T>::SetFlags(uint32 flags)
{
	value |= flags;
}

template<class T>
void BitFlags<T>::ClearFlags(uint32 flags)
{
	value &= ~flags;
}

template<class T>
void BitFlags<T>::ClearFlagsAll()
{
	value = 0;
}

template<class T>
bool BitFlags<T>::TestFlags(uint32 flags)
{
	return (value & flags);
}


typedef BitFlags<uint8>  u8Flags;
typedef BitFlags<uint16> u16Flags;
typedef BitFlags<uint32> u32Flags;

#endif
