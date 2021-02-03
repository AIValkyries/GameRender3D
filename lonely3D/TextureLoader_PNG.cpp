#include "TextureLoader_PNG.h"
#include "UtilTools.h"
#include <png.h>
#include "Debug.h"
#include "Texture.h"

static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	//Debug::LogError("PNG Fatal Error" + (int8)msg);
}

static void png_cpexcept_warn(png_structp png_ptr, png_const_charp msg)
{
	//Debug::LogWarning("PNG Warning" + msg);
}

void PNGAPI user_read_data_fcn(
	png_structp png_ptr, png_bytep data, png_size_t length)
{
	IFile* file = (IFile*)png_get_io_ptr(png_ptr);
	bool check = file->Read(data, length);
	if (!check)
		png_error(png_ptr,"Read Error!");
}

void TextureLoader_PNG::SetGameHost(GameHost* _gameHost)
{
	gameHost = _gameHost;
}

bool TextureLoader_PNG::IsALoadableFileExtension(const string& fileName)
{
	return IsFileExtension(fileName, "png");
}

bool TextureLoader_PNG::Loader(IFile* file, IResourceItem* item)
{
	png_byte buffer[8];

	if (!file->Read(buffer, 8))
		return false;

	// 检测是否是png文件，文件署名
	if (png_sig_cmp(buffer, 0, 8))
	{
		Debug::LogError("LOAD PNG:not really a png\n"+ file->GetFullFileName());
		return false;
	}

	// 创建 png_structp 和 png_infop
	png_structp png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL,
		(png_error_ptr)png_cpexcept_error,
		(png_error_ptr)png_cpexcept_warn);

	if (!png_ptr)
	{
		Debug::LogError("LOAD PNG:png_create_read_struct失败!"+file->GetFullFileName());
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		Debug::LogError("LOAD PNG:png_create_info_struct失败!"+file->GetFullFileName());
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
	}

	png_set_read_fn(png_ptr, file, user_read_data_fcn);
	png_set_sig_bytes(png_ptr, 8);

	// 修改png参数
	png_read_info(png_ptr, info_ptr);

	uint32 width;
	uint32 height;
	int bitDepth;
	int colorType;
	{
		png_uint_32 w, h;
		png_get_IHDR(png_ptr, info_ptr,
			&w, &h, &bitDepth, &colorType,
			NULL, NULL, NULL);
		width = w;
		height = h;
	};

	if (colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// 将低位颜色转换为8位颜色
	if (bitDepth < 8)
	{
		if (colorType == PNG_COLOR_TYPE_GRAY ||
			colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		else
			png_set_packing(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// 将高位颜色转换为8位颜色
	if (bitDepth == 16)
		png_set_strip_16(png_ptr);

	// 将灰色转换为真实颜色
	if (colorType == PNG_COLOR_TYPE_GRAY ||
		colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	int intent;
	const double screen_gamma = 2.2F;

	if (png_get_sRGB(png_ptr, info_ptr, &intent))
		png_set_gamma(png_ptr, screen_gamma, 0.45455);
	else
	{
		double image_gamma;
		if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
			png_set_gamma(png_ptr, screen_gamma, image_gamma);
		else
			png_set_gamma(png_ptr, screen_gamma, 0.45455);
	}

	png_read_update_info(png_ptr, info_ptr);
	{
		png_uint_32 w, h;
		png_get_IHDR(png_ptr, info_ptr,
			&w, &h,
			&bitDepth, &colorType, NULL, NULL, NULL);
		width = w;
		height = h;
	};

	if (colorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

	Texture* texture = (Texture*)item;

	if (colorType == PNG_COLOR_TYPE_RGB_ALPHA)
		texture->CreateTexture(ARGB_32, width, height);
	else
		texture->CreateTexture(RGB_24, width, height);

	uint8** rowPointers = new uint8*[height];

	uint8* data = (uint8*)texture->Lock();

	for (uint32 i = 0;i < height;i++)
	{
		rowPointers[i] = data;
		data += texture->GetPitch();
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		delete[] rowPointers;
		texture->Unlock();
		delete texture;
		return 0;
	}

	png_read_image(png_ptr, rowPointers);
	png_read_end(png_ptr, NULL);

	delete[] rowPointers;
	texture->Unlock();

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	texture->SetFileName(IFile::GetFileName(file->GetFullFileName()));

	Debug::LogInfo("PNG_加载完成,Name=["+ texture->GetFileName() + "]");

	return true;
}