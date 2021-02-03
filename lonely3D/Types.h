#pragma once
#ifndef __Types_H__
#define __Types_H__

// 插值模式
typedef enum 
{
	EIM_CONSTANT = 0,
	EIM_LINEAR,
	EIM_COUNT,
}INTERPOLATION_MODE;

typedef enum 
{
	MOVE_BACKFACE = 1,
	CLIPPED = 1 << 1,

	MAX = 32
}VERTEX_STATE_TYPE;

typedef enum
{
	FLAT_SHADER = 0,
	GOURAUD_SHADER = 1,
	GOURAUD_ALPHA = 2,
	GROUND_TEXTURE_BLEND = 3,
	TERRAIN_SHADER = 4,
	TEXTURE_GOURAUD = 5,
	TEXTURE_GOURAUD_ALPHA = 6,

	SHADER_MAX = 7

}SHADER_TYPE;

// 渲染状态
typedef enum 
{
	//MOVE_BACKFACE                  = 1,             // 移除背面
	TRANSPARENTT = 1,               // 透明
	Z_BUFFER = 2,                  // ZBuffer
	SPECULAR_LIGHT = 3,            // 启用镜面光照
	AMBIENT_LIGHT = 4,             // 环境光
	DIFFUSE_LIGHT = 5,             // 漫反射光

	//------------------设计有点粗糙----------------------//
	RENDER_TEXTURE = 6,

	// 可以多个组合
	RENDER_WIRE_FRAME = 7,               // 线框
	RENDER_TRIANGLE = 8,
	RENDER_NORMAL = 9,
	RENDER_BOX = 10,


	ON_MOVE_BACKFACE = 11,    // 开启背面剔除
	//ON_Z_CLIP = 14,         // 开启z裁剪

	// 着色模型
	GOURAUD = 12,
	FLAT = 13,

	RENDER_STATE_MAX = 32,  // 可以改变
}RENDER_STATE_TYPE;

// 颜色类型
typedef enum 
{
	RGB_565 = 0,
	ARGB_1555 = 1,
	RGB_24 = 2,
	ARGB_32 = 3,
}COLOR_FORMAT;

typedef enum 
{
	CL_Debug,
	CL_WARNING,
	CL_ERROR,
	CL_NODE
}CL_LOG_LEVEL;


// 寻址模式
typedef enum 
{
	REPEAT = 0,
	CLAMP = 1,
	MIRROR
}WARP_MODE;

// 过滤模式
typedef enum 
{
	FILTER_POINT = 0,
	FILTER_BILINEAR = 1
}FILTER_MODE;

// 寻址模式
typedef enum 
{
	READ_ONLY,
	WRITE_ONLY,
	READ_WRITE,
	APPEND_ONLY,
	READ_AND_APPEND
}FILE_ACCESS;


// node 更新bits
typedef enum 
{
	NoUpdate = 1,
	UpdateRotate = 0x2,
	UpdatePosition = 0x4,
	UpdateScale = 0x8,
	UpdateAll = 0x10,
}UPDATE_BITS;

typedef enum 
{
	INDEX_16BIT,
	INDEX_32BIT
}INDEX_TYPE;

 
typedef enum 
{
	LINE_CLIP_EMPTY = 0,
	LINE_CLIP_LEFT = 0x1,        // 0001
	LINE_CLIP_RIGHT = 0x2,       // 0010
	LINE_CLIP_TOP = 0x4,        //  0100
	LINE_CLIP_BOTTOM = 0x8,     //  1000

	LINE_CLIP_TOP_RIGHT = 0x0006,
	LINE_CLIP_BOTTOM_RIGHT = 0x000a,
	LINE_CLIP_TOP_LEFT = 0x0005,
	LINE_CLIP_BOTTOM_LEFT = 0x0009
}LINE_CLIP_MODE;


typedef enum 
{
	L_POINT,           // 点光灯
	L_STANDARD_SPOT,   // 标准 聚光灯
	//SIMPLIFY_SPOT,   // 简化 聚光灯
	L_DIRECTIONAL,     // 无限光
}LIGHY_SOURCE_TYPE;

#endif

