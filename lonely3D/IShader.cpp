#include "IShader.h"
#include "Debug.h"


void IShader::DrawTexture()
{
	if (!it[0])
	{
		Debug::LogError("Texture 为 Null");
		return;
	}
	uint8* dest_addr = (uint8*)renderTarget->Lock();
	uint8* source_addr = (uint8*)it[0]->Lock();

	int32 pitch = it[0]->GetPitch();
	int32 height = it[0]->GetHeight();
	int32 width = it[0]->GetWidth();

	if (height> renderTarget->GetHeight())
	{
		height = renderTarget->GetHeight();
	}

	if (width> renderTarget->GetWidth())
	{
		width = renderTarget->GetWidth();
	}

	if (renderState == 0)
	{
		for (int32 y = 0;y < height;y++)
		{
			for (int32 x = 0;x < width;x++)
			{
				uint32 c = it[0]->GetColor(x, y);
				renderTarget->SetColor(x, y, c);
			}
		}
		return;
	}

	if (renderState[TRANSPARENTT])
	{
		int32 pixel_x;
		uint8 pixel;

		for (int32 y = 0;y < height;y++)
		{
			for (pixel_x = 0; pixel_x < width; pixel_x++)
			{
				if ((pixel = source_addr[pixel_x]) != 0)
					dest_addr[pixel_x] = pixel;
			}

			dest_addr += renderTarget->GetPitch();
			source_addr += it[0]->GetPitch();
		}
	}
	else
	{
		for (int32 y = 0;y < height;y++)
		{
			for (int32 x = 0;x < width;x++)
			{
				uint32 c = it[0]->GetColor(x,y);
				renderTarget->SetColor(x, y, c);
			}
		}
	}

}

void IShader::Draw_Bresenham_Line16_Decal(
	Texture* image, uint16 color,
	Vector2& startPos, Vector2& endPos)
{
	float32 dx, dy, dy2, dx2, error;
	int32 yInc, xInc, sy, sx, ey, ex;

	xInc = 1;
	yInc = image->GetWidth();

	sy = int32(startPos.y);
	sx = int32(startPos.x);
	ey = int32(endPos.y);
	ex = int32(endPos.x);

	uint16 imageHeight = image->GetHeight();
	uint16 imageWidth = image->GetWidth();
	
	sy = Clamp((uint16)sy, 0, imageHeight - 1);
	sx = Clamp((uint16)sx, 0, imageWidth - 1);

	dx = float32((ex - sx));
	dy = float32((ey - sy));

	if (dx < 0)
	{
		dx = -dx;
		xInc = -xInc;
	}

	if (dy < 0)
	{
		dy = -dy;
		yInc = -yInc;
	}

	if (dy > dx)
	{
		float32 tempf;
		tempf = dx;
		dx = dy;
		dy = tempf;

		int32 temp;
		temp = xInc;
		xInc = yInc;
		yInc = temp;
	}

	dx2 = dx * 2.0F;
	dy2 = dy * 2.0F;
	error = dy2 - dx;

	int32 startOffset = sy * imageWidth + sx;
	uint16* data = ((uint16*)image->Lock()) + startOffset;
	
	int32 totalIndex = image->GetLength() / 4;

	int32 index = startOffset;
	int32 tempDx = (int32)dx;

	for (int32 i = 0;i < tempDx;i++)
	{
		if (index > totalIndex)
			break;

		*data = color;
		if (error >= 0)
		{
			data += yInc;	
			error -= dx2;
			index += yInc;
		}
		error += dy2;
		data += xInc;

		index += xInc;
	}
}

void IShader::Draw_Bresenham_Line24_Decal(
	Texture* image, uint32 color,
	Vector2& startPos, Vector2& endPos)
{
	int32 
		dy, dx, dy2, dx2, error, 
		sy, sx, yInc, xInc;

	sy = int32(startPos.y + 0.5F);
	sx = int32(startPos.x + 0.5F);

	uint16 imageHeight = image->GetHeight();
	uint16 imageWidth = image->GetWidth();

	sy = Clamp((uint16)sy, 0, imageHeight - 1);
	sx = Clamp((uint16)sx, 0, imageWidth - 1);

	dx = int32((endPos.x - startPos.x) + 0.5F);
	dy = int32((endPos.y - startPos.y) + 0.5F);

	yInc = image->GetPitch();
	xInc = 4;

	if (dx < 0)
	{
		dx = -dx;
		xInc = -xInc;
	}

	if (dy < 0)
	{
		dy = -dy;
		yInc = -yInc;
	}
	
	if (dy > dx)
	{
		int32 temp;
		temp = dx;
		dx = dy;
		dy = temp;
		temp = xInc;
		xInc = yInc;
		yInc = temp;
	}

	dy2 = dy << 1;
	dx2 = dx << 1;
	error = dy2 - dx;

	uint32* data = (uint32*)((uint8*)image->Lock() + sy * image->GetPitch() + (sx << 2));

	int32 tempDx = (int32)dx;

	for (int32 i = 0;i < tempDx;i++)
	{
		*data = color;
		if (error >= 0)
		{
			error -= dx2;
			data = (uint32*)((uint8*)data + yInc);
		}

		error += dy2;
		data = (uint32*)((uint8*)data + xInc);
	}

}


void IShader::Draw_Bresenham_Line32_Decal(
	Texture* image, uint32 color,
	Vector2& startPos, Vector2& endPos)
{
	float32 dx, dy, dx2, dy2, error;

	int32
		sy, sx, yInc, xInc;

	sy = int32(startPos.y + 0.5F);
	sx = int32(startPos.x + 0.5F);

	uint16 imageHeight = image->GetHeight();
	uint16 imageWidth = image->GetWidth();

	sy = Clamp((uint16)sy, 0, imageHeight - 1);
	sx = Clamp((uint16)sx, 0, imageWidth - 1);

	dx = (endPos.x - startPos.x);
	dy = (endPos.y - startPos.y);

	yInc = imageWidth;
	xInc = 1;

	if (dx < 0)
	{
		dx = -dx;
		xInc = -xInc;
	}

	if (dy < 0)
	{
		dy = -dy;
		yInc = -yInc;
	}

	if (dy > dx)
	{
		float32 tempf;
		tempf = dx;
		dx = dy;
		dy = tempf;

		int32 temp;
		temp = xInc;
		xInc = yInc;
		yInc = temp;
	}

	dx2 = dx * 2;
	dy2 = dy * 2;
	error = dy2 - dx;

	int32 startOffset = sy * imageWidth + sx;
	uint32* data = ((uint32*)image->Lock()) + startOffset;

	int32 totalIndex = image->GetLength() / 4;
	int32 index = startOffset;

	int32 tempDx = (int32)dx;

	for (int32 i = 0;i < tempDx;i++)
	{
		if (index > totalIndex) break;

		*data = color;
		if (error >= 0)
		{
			error -= dx2;
			data += yInc;
			index += yInc;
		}
		error += dy2;
		data += xInc;

		index += xInc;
	}
}

void IShader::DrawLine(
	const Vertex* v0,
	const Vertex* v1,
	const ColorU32& color)
{
	if (renderTarget == 0)
		return;
	Vector2 startPos;
	Vector2 endPos;

	Vector2 p0(v0->position.x, v0->position.y);
	Vector2 p1(v1->position.x, v1->position.y);

	if (!CS_LineClip(p0, p1, startPos, endPos, *renderTarget->GetRect()))
		return;

	switch (renderTarget->GetColorFormat())
	{
	case RGB_565:
	{
		Draw_Bresenham_Line16_Decal(
			renderTarget,
			color.ToRGB565(),
			startPos,
			endPos);
	}break;
	case ARGB_1555:
	{
		Draw_Bresenham_Line16_Decal(
			renderTarget,
			color.ToARGB1555(),
			startPos,
			endPos);
	}break;
	case RGB_24:
	{
		Draw_Bresenham_Line24_Decal(
			renderTarget,
			color.ToRGB24(),
			startPos,
			endPos);
	}break;
	case ARGB_32:
	{
		Draw_Bresenham_Line32_Decal(
			renderTarget,
			color.GetColor(),
			startPos,
			endPos);
	}break;
	}

}

// 画线 Bresenham 直线算法
void IShader::DrawLine(
	const Vector2& v0,
	const Vector2& v1,
	const ColorU32& color)
{
	if (renderTarget == 0)
		return;

	Vector2 startPos;
	Vector2 endPos;

	if (!CS_LineClip(v0, v1, startPos, endPos, *renderTarget->GetRect()))
		return;

	switch (renderTarget->GetColorFormat())
	{
	case RGB_565:
	{
		Draw_Bresenham_Line16_Decal(
			renderTarget, 
			color.ToRGB565(), 
			startPos,
			endPos);
	}break;
	case ARGB_1555:
	{
		Draw_Bresenham_Line16_Decal(
			renderTarget,
			color.ToARGB1555(),
			startPos,
			endPos);
	}break;
	case RGB_24:
	{
		Draw_Bresenham_Line24_Decal(
			renderTarget,
			color.ToRGB24(),
			startPos,
			endPos);
	}break;
	case ARGB_32:
	{
		Draw_Bresenham_Line32_Decal(
			renderTarget,
			color.GetColor(),
			startPos,
			endPos);
	}break;
	}
}

// 使用 Cohen-Sutherland 算法也称之为编码裁剪算法
// 顺序上下右左 = D3D2D1D0  D3=上，D2=下，D1=右，D0=左
// 一、三种情况 
// 1.全部在窗口外，称之为简弃
// 2.全部在窗口内，称之为简取
// 3.一部分在外，一部分在内
///            |               |
///     1001   |     1000      |    1010
///  ----------|---------------|-------------
///     0001   |     0000      |    0010
///  ----------|---------------|--------------
///     0101   |     0100      |    0110
///            |               |
/// 为什么是这个编码呢？因为 D3D2D1D0，if(D3==Top) D3=1 else D3=0
/// 其他的也是如此,则9个编码可以自己算出来
/// 二、
/// 将v0和v1分别进行编码得到code1与code2
/// 如果code1 | code2 =  0 简取
/// 如果code1 & code2 != 0 简弃
/// 三、对一般在内，一般在外的线段进行裁剪
bool IShader::CS_LineClip(
	const Vector2& v0, 
	const Vector2& v1, 
	Vector2& p0,
	Vector2& p1,
	const Rect& rect)
{
	p0 = v0;
	p1 = v1;
 
	uint32 code1 = GetClipMode(p0, rect);
	uint32 code2 = GetClipMode(p1, rect);

	uint32 code = 0;

	float32 x = p0.x, y = p0.y;
	float32 x1 = p1.x, y1 = p1.y;

	float32 xMin = rect.x;
	float32 yMin = rect.y;
	float32 xMax = rect.GetXMax();
	float32 yMax = rect.GetYMax();

	if ((code1 & code2) != 0) // 简弃 
		return false;

	if (code1 == 0 && code2 == 0)
		return true;

	switch(code1)
	{
	case LINE_CLIP_LEFT:
	{
		x = xMin;
		float32 t = (p1.y - p0.y) / (p1.x - p0.x);
		y = p0.y + 0.5F + (xMin - p0.x) * t;
	}break;
	case LINE_CLIP_RIGHT:
	{
		x = xMax;
		float32 t = (p1.y - p0.y) / (p1.x - p0.x);
		y = p0.y + 0.5F + (xMax - p0.x) * t;
	}break;

	case LINE_CLIP_TOP:
	{
		y = yMax;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = p0.x + 0.5F + (yMax - p0.y) * t;

	}break;

	case LINE_CLIP_BOTTOM:
	{
		y = yMin;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = p0.x + 0.5F + (yMin - p0.y) * t;
	}break;


	/*case LINE_CLIP_TOP_LEFT:
	{
		y = yMax;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = p0.x + 0.5F + (yMax - p0.y) * t;

		if (x < xMin || x > xMax)
		{
			x = xMin;
			t = (p1.y - p0.y) / (p1.x - p0.x);
			y = p0.y + 0.5F + (xMin - p0.x) * t;
		}

	}break;
	case LINE_CLIP_TOP_RIGHT:
	{
		y = yMax;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = p0.x + 0.5F + (yMax - p0.y) * t;

		if (x < xMin || x > xMax)
		{
			x = xMax;
			t = (p1.y - p0.y) / (p1.x - p0.x);
			y = p0.y + 0.5F + (xMax - p0.x) * t;
		}

	}break;
	case LINE_CLIP_BOTTOM_LEFT:
	{
		y = yMin;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = p0.x + 0.5F + (yMin - p0.y) * t;

		if (x < xMin || x > xMax)
		{
			x = xMin;
			t = (p1.y - p0.y) / (p1.x - p0.x);
			y = p0.y + 0.5F + (xMin - p0.x) * t;
		}

	}break;
	case LINE_CLIP_BOTTOM_RIGHT:
	{
		y = yMin;
		float32 t = (p1.x - p0.x) / (p1.y - p0.y);
		x = (p0.x) + 0.5F + (yMin - p0.y) * t;

		if (x < xMin || x > xMax)
		{
			x = xMax;
			t = (p1.y - p0.y) / (p1.x - p0.x);
			y = p0.y + 0.5F + (xMax - p0.x) * t;
		}

	}break;*/

	}

	switch (code2)
	{
	case LINE_CLIP_LEFT:
	{
		x1 = xMin;
		float32 t = (p0.y - p1.y) / (p0.x - p1.x);
		y1 = p1.y + (xMin - p1.x) * t;
	}break;
	case LINE_CLIP_RIGHT:
	{
		x1 = xMax;
		float32 t = (p0.y - p1.y) / (p0.x - p1.x);
		y1 = p1.y + 0.5F + (xMax - p1.x) * t;

	}break;
	case LINE_CLIP_TOP:
	{
		y1 = yMax;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = p1.x + (yMax - p1.y) * t;
	}break;
	case LINE_CLIP_BOTTOM:
	{
		y1 = yMin;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = p1.x + (yMin - p1.y) * t;
	}break;



	case LINE_CLIP_TOP_RIGHT:
	{
		y1 = yMax;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = p1.x + (yMax - p1.y) * t;

		if (x1 < xMin || x1 > xMax)
		{
			x1 = xMax;
			t = (p0.y - p1.y) / (p0.x - p1.x);
			y1 = p1.y + (xMax - p1.x) * t;
		}

	}break;
	case LINE_CLIP_TOP_LEFT:
	{
		y1 = yMax;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = (p1.x) + (yMax - p1.y) * t;

		if (x1 < xMin || x1 > xMax)
		{
			x1 = xMin;
			t = (p0.y - p1.y) / (p0.x - p1.x);
			y1 = p1.y + (xMin - p1.x) * t;
		}

	}break;
	case LINE_CLIP_BOTTOM_RIGHT:
	{
		y1 = yMin;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = (p1.x + (yMin - p1.y) * t);

		if (x1 < xMin || x1 > xMax)
		{
			x1 = xMax;
			t = (p0.y - p1.y) / (p0.x - p1.x);
			y1 = p1.y + (xMax - p1.x) * t;
		}
	}break;
	case LINE_CLIP_BOTTOM_LEFT:
	{
		y1 = yMin;
		float32 t = (p0.x - p1.x) / (p0.y - p1.y);
		x1 = p1.x + (yMin - p1.y) * t;

		if (x1 < xMin || x1 > xMax)
		{
			x1 = xMin;
			t = (p0.y - p1.y) / (p0.x - p1.x);
			y1 = p1.y + (xMin - p1.x) * t;
		}
	}break;

	}

	if ((x < xMin) || (x > xMax) ||
		(y < yMin) || (y > yMax) ||
		(x1 < xMin) || (x1 > xMax) ||
		(y1 < yMin) || (y1 > yMax))
	{
		return false;
	} 
 
	p0.x = x;
	p0.y = y;

	p1.x = x1;
	p1.y = y1;

	return true;
}


int IShader::GetClipMode(const Vector2& v, const Rect& rect)
{
	uint32 code = LINE_CLIP_EMPTY;

	if (v.x < rect.x)
		code |= LINE_CLIP_LEFT;
	else if (v.x > rect.GetXMax())
		code |= LINE_CLIP_RIGHT;

	if (v.y < rect.y)
		code |= LINE_CLIP_BOTTOM;
	else if (v.y > rect.GetYMax())
		code |= LINE_CLIP_TOP;

	return code;
}