#pragma once

#ifndef __DataType_H__
#define __DataType_H__

#include <string>
using namespace std;

typedef string String;

typedef signed long int32;
typedef signed short int16;
typedef signed char int8;
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef float float32;
typedef double float64;

#define MIN_INT_32 0x80000000
#define MAX_INT_32 0x7fffffff
#define MIN_INT_16 0x8000
#define MAX_INT_16 0x7fff
#define MIN_INT_8 0x80
#define MAX_INT_8 0x7f

#define MIN_UINT_32 0
#define MAX_UINT_32 0xffffffff
#define MIN_UINT_16 0
#define MAX_UINT_16 0xffff
#define MIN_UINT_8 0
#define MAX_UINT_8 0xff

const float32 MAX_FLOAT_32 = static_cast<float>(3.4E+38);
const float32 MIN_FLOAT_32 = static_cast<float>(-3.4E+38);

#endif
