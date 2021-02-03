#include "FIleInfo.h"

string IFile::GetFileName(const string& fileFullName)
{
	char szDrive[_MAX_DRIVE];   //磁盘名
	char szDir[_MAX_DIR];       //路径名
	char szFname[_MAX_FNAME];   //文件名
	char szExt[_MAX_EXT];       //后缀名

	_splitpath_s(fileFullName.c_str(), szDrive, szDir, szFname, szExt); //分解路径

	return szFname;
}

string IFile::GetFileDir(const string& filename)
{
	// 查找最后的正斜杠或反斜杠
	uint32 lastSlash = filename.find_last_of('/');
	/*const uint32 lastBackSlash = filename.find_last_of('\\');
	lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;*/

	if (lastSlash < filename.size())
		return filename.substr(0, lastSlash);
	else
		return filename;
}

IFile* IFile::Open(string fileName, int32 access)
{
	char file_access[3];

	FILE* file = 0;
	uint32 fileSize = 0;
	FIleInfo* info = new FIleInfo();

	switch (access)
	{
	case READ_ONLY:
		strcpy_s(file_access, "rb");
		break;
	case WRITE_ONLY:
		strcpy_s(file_access, "wb");
		break;
	case APPEND_ONLY:
		strcpy_s(file_access, "ab");
		break;
	case READ_AND_APPEND:
		strcpy_s(file_access, "a+b");
		break;
	default:
	case READ_WRITE:
		strcpy_s(file_access, "r+b");
		break;
	}

	fopen_s(&file, fileName.c_str(), file_access);
	if (file == 0)
		return NULL;
	return info->Create(file, fileName);
}

IFile* FIleInfo::Create(FILE* _file,string _fileName)
{
	file = _file;
	fileName = _fileName;

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file,0,SEEK_SET);

	return this;
}

bool FIleInfo::Read(void* buffer, int32 bufferCount)
{
	if (!IsOpen())
		return false;

	uint32 bytesRead = 0;
	bytesRead = fread(buffer, 1, bufferCount, file);
	return bytesRead == bufferCount;
}

void FIleInfo::Seek(int32 offset, int32 flags)
{
	if (!IsOpen())
		return;

	fseek(file, offset, flags);
}

bool FIleInfo::Write(const void* buffer, int32 bufferCount)
{
	if (!IsOpen())
		return false;

	uint32 bytesRead = 0;
	bytesRead = fwrite(buffer, 1, bufferCount, file);
	return bytesRead == bufferCount;
}