#include "TextureLoader_JPEG.h"
#include "UtilTools.h"
#include "Texture.h"
#include "Debug.h"
#include "ColorConverter.h"

static void init_source(j_decompress_ptr cinfo)
{

}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{

	return 1;
}

static void skip_input_data(j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr* src = cinfo->src;
	if (count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}

static boolean resync_to_restart(j_decompress_ptr cinfo, int desired)
{

	return 1;
}

static void term_source(j_decompress_ptr cinfo)
{

}

static void error_exit(j_common_ptr cinfo)
{
	cinfo->err->output_message(cinfo);
}

void TextureLoader_JPEG::SetGameHost(GameHost* _gameHost)
{
	gameHost = _gameHost;
}

bool TextureLoader_JPEG::IsALoadableFileExtension(const string& fileName)
{
	return IsFileExtension(fileName, "jpg", "jpeg");
}


// 输入:jpg文件，输出：RGB数据
bool TextureLoader_JPEG::Loader(IFile* file, IResourceItem* item)
{
	uint32 fileSize = file->GetFileSize();
	uint8* input = new uint8[fileSize];
	file->Read(input, fileSize);

	struct jpeg_decompress_struct  com_cinfo;
	struct jpeg_error_mgr com_jerr;

	com_cinfo.err = jpeg_std_error(&com_jerr);

	// 创建解压缩对象
	jpeg_create_decompress(&com_cinfo);

	jpeg_source_mgr jsrc;
	jsrc.bytes_in_buffer = fileSize;
	jsrc.next_input_byte = (JOCTET*)input;
	com_cinfo.src = &jsrc;

	// 初始化注册函数
	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_input_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = resync_to_restart;
	jsrc.term_source = term_source;

	//读取头
	jpeg_read_header(&com_cinfo, TRUE);

	// 为解压缩设定参数
	bool useCMYK = false;
	if (com_cinfo.jpeg_color_space == JCS_CMYK)
	{
		com_cinfo.out_color_space = JCS_CMYK;
		com_cinfo.out_color_components = 4;
		useCMYK = true;
	}
	else
	{
		com_cinfo.out_color_space = JCS_RGB;
		com_cinfo.out_color_components = 3;
	}

	com_cinfo.output_gamma = 2.2F;
	com_cinfo.do_fancy_upsampling = false;

	// 开始解压缩
	jpeg_start_decompress(&com_cinfo);

	uint16 rowspan = com_cinfo.image_width * com_cinfo.out_color_components;
	uint32 width   = com_cinfo.image_width;
	uint32 height  = com_cinfo.image_height;

	uint8* output = new uint8[rowspan * height];

	uint8** rowPtr = new uint8*[height];

	for (uint32 i = 0;i < height;i++)
		rowPtr[i] = &output[i * rowspan];

	uint32 read = 0;

	while (com_cinfo.output_scanline < com_cinfo.output_height)
	{
		read += jpeg_read_scanlines(
			&com_cinfo,
			&rowPtr[read],
			com_cinfo.output_height - read);
	}

	delete[] rowPtr;

	jpeg_finish_decompress(&com_cinfo);

	//释放JPEG解压缩对象
	//这是重要的一步，因为它将释放大量内存。
	jpeg_destroy_decompress(&com_cinfo);

	Texture* texture = (Texture*)item;

	// c:青色   Cyan
	// m:洋红色 Magenta
	// y:黄色   Yellow
	// k:黑色   Black
	if (useCMYK)
	{
		texture->CreateTexture(RGB_24, width, height);

		uint32 size = 3 * width * height;
		uint8* data = (uint8*)texture->Lock();

		if (data)  // 将cmyk转换成rgb
		{
			for (uint32 i = 0, j = 0;i < size;i += 3, j += 4)
			{
				data[i + 0] = uint8(((char)output[j + 2] * output[j + 3]) / 255.0F);
				data[i + 1] = uint8(((char)output[j + 1] * output[j + 3]) / 255.0F);
				data[i + 2] = uint8(((char)output[j + 0] * output[j + 3]) / 255.0F);
			}
		}

		texture->Unlock();
		delete[] output;
	}
	else
	{
		texture->CreateTexture(RGB_24, width, height, output);

		//ColorConverter::Convert24To24Bit(
		//	output, (uint8*)texture->Lock(),
		//	width, height, rowspan, true, false);
	}

	delete[] input;

	texture->SetFileName(IFile::GetFileName(file->GetFullFileName()));

	Debug::LogInfo("JPEG_加载完成,Name=["+ texture->GetFileName() + "]");

	return true;
}