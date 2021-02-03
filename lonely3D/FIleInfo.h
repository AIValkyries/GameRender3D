#pragma once
#ifndef __FIleInfo_H__
#define __FIleInfo_H__

#include "DataType.h"
#include "IFile.h"

class FIleInfo :public IFile
{
public:
	FIleInfo();
	~FIleInfo();


	virtual IFile* Create(FILE* file, string fileName);
	virtual bool Read(void* buffer, int32 bufferCount);
	virtual void Seek(int32 offset, int32 flags);
	virtual bool Write(const void* buffer, int32 bufferCount);

	virtual uint32 GetFileSize()const { return fileSize; }
	virtual bool IsOpen()const { return file != 0; }
	virtual const string GetFullFileName()const { return fileName; }
	virtual const uint32 GetPos()const {  return ftell(file);; }


};

inline FIleInfo::FIleInfo()
{
	file = 0;
	fileSize = 0;
	fileName = "";
	pos = 0;
}

inline FIleInfo::~FIleInfo() {}

#endif
