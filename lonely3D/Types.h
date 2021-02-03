#pragma once
#ifndef __Types_H__
#define __Types_H__

// ��ֵģʽ
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

// ��Ⱦ״̬
typedef enum 
{
	//MOVE_BACKFACE                  = 1,             // �Ƴ�����
	TRANSPARENTT = 1,               // ͸��
	Z_BUFFER = 2,                  // ZBuffer
	SPECULAR_LIGHT = 3,            // ���þ������
	AMBIENT_LIGHT = 4,             // ������
	DIFFUSE_LIGHT = 5,             // �������

	//------------------����е�ֲ�----------------------//
	RENDER_TEXTURE = 6,

	// ���Զ�����
	RENDER_WIRE_FRAME = 7,               // �߿�
	RENDER_TRIANGLE = 8,
	RENDER_NORMAL = 9,
	RENDER_BOX = 10,


	ON_MOVE_BACKFACE = 11,    // ���������޳�
	//ON_Z_CLIP = 14,         // ����z�ü�

	// ��ɫģ��
	GOURAUD = 12,
	FLAT = 13,

	RENDER_STATE_MAX = 32,  // ���Ըı�
}RENDER_STATE_TYPE;

// ��ɫ����
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


// Ѱַģʽ
typedef enum 
{
	REPEAT = 0,
	CLAMP = 1,
	MIRROR
}WARP_MODE;

// ����ģʽ
typedef enum 
{
	FILTER_POINT = 0,
	FILTER_BILINEAR = 1
}FILTER_MODE;

// Ѱַģʽ
typedef enum 
{
	READ_ONLY,
	WRITE_ONLY,
	READ_WRITE,
	APPEND_ONLY,
	READ_AND_APPEND
}FILE_ACCESS;


// node ����bits
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
	L_POINT,           // ����
	L_STANDARD_SPOT,   // ��׼ �۹��
	//SIMPLIFY_SPOT,   // �� �۹��
	L_DIRECTIONAL,     // ���޹�
}LIGHY_SOURCE_TYPE;

#endif

