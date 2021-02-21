#include "TextureLoader_BMP.h"
#include "UtilTools.h"
#include "Debug.h"
#include "Texture.h"
#include "ColorConverter.h"
#include "GameHost.h"

void TextureLoader_BMP::SetGameHost(GameHost* _gameHost)
{
	gameHost = _gameHost;
}


bool TextureLoader_BMP::IsALoadableFileExtension(const string& fileName)
{
	return IsFileExtension(fileName, "bmp");
}

// ��bmpData�е����ݽ�ѹ���� ֻ����8λλͼ
//03 04               04 04 04
//05 06               06 06 06 06 06
//00 03 45 56 67 00   45 56 67
//02 78               78 78
//00 02 05 01         �ӵ�ǰλ������5��λ�ú�������һ��
//02 78               78 78
//00 00               �н���       
//09 1E               1E 1E 1E 1E 1E 1E 1E 1E 1E
//00 01               RLE����ͼ�����
void TextureLoader_BMP::Decompress8BitRLE(
	uint8*& bmpData, uint32 size,
	uint32 width, uint32 height, uint32 pitch)
{
	uint32 line = 0;
	uint32 totalSize = height * width;

	uint8* p = bmpData;
	uint8* newData = new uint8[totalSize]; 	// ����������
	uint8* d = newData;

	uint32 pSize = 0;
	uint32 dSize = 0;
	bool isDelete = false;

	while (pSize < size && dSize < totalSize)
	{
		if (*p == 0)
		{
			++p;
			++pSize;

			switch (*p)
			{
			case 0:            // �н���
			{
				++line;
				++p;
				++pSize;

				uint32 linePitch = line * width;

				d = newData + linePitch;
				dSize += linePitch;

			}break;
			case 1:           // λͼ����
			{
				delete[] bmpData;
				bmpData = newData;
				isDelete = true;

				Debug::LogError("������11111");
				return;
			}
			// 00 02 05 01 �ӵ�ǰλ������5��λ�ú�������һ��
			case 2:
			{
				++p;
				++pSize;

				d += *p;
				dSize += *p;

				++p;
				++pSize;

				d += (width * (*p));
				dSize += (width * (*p));

				++p;
				++pSize;
			}break;
			// û�о���ѹ��, 2�ֽڶ��룬�������00��ȫ
			// ���� 00 05 12 34 56 78 9A 00����ʾ12 34 56 78 9A��
			default:
			{
				uint16 count = *p;
				++p;
				++pSize;
				int32 makeup = (2 - (count % 2)) % 2;

				for (int32 i = 0;i < count;i++)
				{
					*d = *p;
					++d;
					++p;
					++dSize;
					++pSize;
				}

				for (int32 i = 0;i < makeup;i++)
				{
					++pSize;
					++p;
				}

			}break;
			}

		}
		else // ��ѹ�� 03 04 = 04 04 04
		{
			int32 count = (uint8)*p;
			++p;
			++pSize;

			uint8 color = *p;
			++p;
			++pSize;

			for (int32 i = 0;i < count;i++)
			{
				*d = color;
				++d;
				++dSize;
			}
		}
	}

	if (!isDelete)
	{
		delete[] bmpData;
		bmpData = newData;
		Debug::LogError("֡tmSIM");
	}
}

// ���뷽ʽ��
// ��2���ֽ���ɣ���һ���ֽ�ָ��������Ŀ���ڶ����ֽڰ���������ɫ������
// һ���ڸ�4λ����һ���ڵ�4λ��
// ��һ������ʹ�ø�4λ����ɫ�������ڶ���ʹ�õ�4λ����ɫ������
// ��3��ʹ�ø�4λ����ɫ�������������ơ�
// һ���ֽڰ�����2��������ɫ ֻ����4λλͼ
//03 04                   0 4 0
//05 06                   0 6 0 6 0
//00 06 45 56 67 00       4 5 5 6 6 7
//04 78                   7 8 7 8
//00 02 05 01             �ӵ�ǰλ������5��λ�ú�������һ��
//04 78                   7 8 7 8
//00 00                   �н���
//09 1E                   1 E 1 E 1 E 1 E 1
//00 01                   RLEͼ�����
void TextureLoader_BMP::Decompress4BitRLE(
	uint8* bmpData, uint32 size,
	uint32 width, uint32 height, uint32 pitch)
{
	uint32 line = 0;
	uint32 shift = 4;
	uint32 totalSize = height * width;
	
	uint8* p = bmpData;
	uint8* newData = new uint8[totalSize];
	uint8* d = newData;

	uint32 pSize = 0;
	uint32 dSize = 0;
	bool isDelete = false;

	while (pSize < size && dSize < totalSize)
	{
		if (*p == 0)
		{
			++p;
			++pSize;

			switch (*p)
			{
			case 0:              // �н���
			{
				++p;
				++pSize;
				++line;

				uint32 linePitch = line * width;

				d = newData + linePitch;
				dSize += linePitch;

			}break;
			case 1:              // λͼ����
			{
				delete[] bmpData;
				bmpData = newData;
				isDelete = true;
				return;
			};
			// 00 02 05 01 �ӵ�ǰλ������5��λ�ú�������һ��
			case 2:
			{
				++p;
				++pSize;

				uint8 x = (*p);
				++p;
				++pSize;

				uint8 y = (*p);
				++p;
				++pSize;

				uint32 r = x / 2 + y * width;

				d += r;
				dSize += r;
				shift = x % 2 == 0 ? 4 : 0;
			}break;
			// 00 06 45 56 67 00=4 5 5 6 6 7
			default: // ûѹ����
			{
				uint16 count = *p;
				++p;
				++pSize;

				int32 makeup = (2 - (count % 2)) % 2;
				int32 readShift = 4;

				for (int32 i = 0;i < count;i++)
				{
					uint32 color = (*p) & (0xF << readShift);
					readShift -= 4;
					if (readShift < 0)
					{
						++p;
						++pSize;
						readShift = 4;
					}

					uint8 mask = 0xF << shift;
					*d = ((*d) & (~mask)) | ((color << shift) & mask);
					shift -= 4;
					if (shift < 0)
					{
						shift = 4;
						++d;
						++dSize;
					}
				}

				for (int32 i = 0;i < makeup;i++)
				{
					++p;
					++pSize;
				}
			}break;
			}
		}
		else  // ��ѹ�� 03 04 = 0 4 0
		{
			uint16 count = *p;
			++p;
			++pSize;

			uint32 color1 = *p & 0x0F;
			uint32 color2 = (*p >> 4) & 0x0F;
			++p;
			++pSize;

			for (int32 i = 0;i < count;i++)
			{
				uint32 color = (shift == 0 ? color1 : color2) << shift;

				uint32 mask = 0xF << shift;
				*d = (uint8)(((*d) & (~mask)) | ((color << shift) & mask));

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
					++dSize;
				}
			}
		}
	}

	if (!isDelete)
	{
		delete[] bmpData;
		bmpData = newData;
	}

}

/// <summary>
/// λͼ�������Ĳ������
///  bmpͷ�ļ�:   �ṩ�ļ��ĸ�ʽ����С����Ϣ
///  λͼ��Ϣͷ:  �ṩͼ�����ݵĳߴ硢λƽ������ѹ����ʽ����ɫ��������Ϣ
///  ��ɫ��:      ��ѡ����ʹ����������ʾͼ�񣬵�ɫ��������������Ӧ����ɫ��ӳ���
///  λͼ���ݣ�   һ������uint8
/// </summary>
/// <param name="file"></param>
/// <returns></returns>
bool TextureLoader_BMP::Loader(IFile* file, IResourceItem* item)
{
#define BITMAP_ID 0x4d42
#define MAX_COLORS_PALETTE 256

	// λͼͷ�ļ�
	BitMapFileHeader fileHeader;
	file->Read(&fileHeader, sizeof(BitMapFileHeader));

	if (fileHeader.bfType != BITMAP_ID)
		return false;

	// λͼ��Ϣͷ
	BitMapInfoHeader infoHeader;
	file->Read(&infoHeader, sizeof(BitMapInfoHeader));

	if (infoHeader.biCompression > 2)
	{
		Debug::LogError("��֧��ѹ��ģʽ");
		return false;
	}

	uint32 pitch = 0;

	// ��ɫ�� Color palette
	bool hasPaletteentry = false;
	infoHeader.biSizeImage += (4 - (infoHeader.biSizeImage % 4)) % 4;

	uint32* paletteentry = 0;
	uint32 paletteSize = (fileHeader.bfOffBits - file->GetPos()) / 4;

	if (paletteSize)
	{
		hasPaletteentry = true;
		paletteentry = new uint32[paletteSize];

		file->Read(paletteentry, sizeof(uint32) * paletteSize);
	}
	else
		pitch = infoHeader.biSizeImage / infoHeader.biHeight;

	file->Seek(fileHeader.bfOffBits, SEEK_SET);

	// λͼ����
	uint8* bmpData = new uint8[infoHeader.biSizeImage];
	file->Read(bmpData, infoHeader.biSizeImage);

	// ��ѹ��
	switch (infoHeader.biCompression)
	{
	case 1:  // 8 bit rle
	{
		Decompress8BitRLE(
			bmpData, 
			infoHeader.biSizeImage,
			infoHeader.biWidth, 
			infoHeader.biHeight, pitch);
	}break;
	case 2: // 4 bit rle
	{
		Decompress4BitRLE(
			bmpData,
			infoHeader.biSizeImage,
			infoHeader.biWidth, 
			infoHeader.biHeight, pitch);
	}break;
	}

	Texture* texture = (Texture*)item;

	switch (infoHeader.biBitCount)
	{
	case 1:
	{
		texture->CreateTexture(ARGB_1555, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert1BitTo16Bit(
				bmpData, (uint16*)texture->Lock(),
				infoHeader.biWidth, infoHeader.biHeight, pitch, false);
		}

	}break;
	case 4:
	{			
		texture->CreateTexture(ARGB_1555, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert4BitTo16Bit(
				bmpData, (uint16*)texture->Lock(),
				infoHeader.biWidth, infoHeader.biHeight,paletteentry, pitch, false);
		}
	}break;
	case 8:
	{
		texture->CreateTexture(ARGB_1555, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert8BitTo16Bit(
				bmpData, (uint16*)texture->Lock(),
				infoHeader.biWidth, 
				infoHeader.biHeight, 
				paletteentry, 
				pitch, 
				true);
		}
	}break;
	case 16:
	{		
		texture->CreateTexture(ARGB_1555, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert16BitTo16Bit(
				(uint16*)bmpData, 
				(uint16*)texture->Lock(),
				infoHeader.biWidth, 
				infoHeader.biHeight,
				pitch, false);
		}
	}break;
	case 24:
	{		
		texture->CreateTexture(RGB_24, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert24To24Bit(
				bmpData, (uint8*)texture->Lock(),
				infoHeader.biWidth, infoHeader.biHeight, pitch, false, true);
		}
	}break;
	case 32:
	{		
		texture->CreateTexture(ARGB_32, infoHeader.biWidth, infoHeader.biHeight);
		if (texture)
		{
			ColorConverter::Convert32To32Bit(
				(uint32*)bmpData, (uint32*)texture->Lock(),
				infoHeader.biWidth, infoHeader.biHeight, pitch, false);
		}
	}break;
	}

	if (texture) texture->Unlock();

	if (bmpData)      delete[] bmpData;
	if(paletteentry)  delete[] paletteentry;

	texture->SetFileName(IFile::GetFileName(file->GetFullFileName()));

	Debug::LogInfo("BMP_�������,Name=[" + texture->GetFileName() + "]");
	Debug::LogInfo("WIDTH=" + Convert(infoHeader.biWidth) + 
		           ":HEIGHT=" + Convert(infoHeader.biHeight));

	return true;
}
