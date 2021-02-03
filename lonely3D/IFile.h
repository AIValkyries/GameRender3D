#pragma once
#ifndef __IFile_H__
#define __IFile_H__

#include "DataType.h"
#include <iostream>
#include <string>
#include "Types.h"

using namespace std;

class IFile
{
public:
	static IFile* Open(string fileName, int32 access);
	static string GetFileName(const string& fileFullName);
	static string GetFileDir(const string& filename);

	virtual IFile* Create(FILE* file, string fileName) = 0;
	virtual bool Read(void* buffer, int32 bufferCount) = 0;
	virtual void Seek(int32 offset, int32 flags) = 0;
	virtual bool Write(const void* buffer, int32 bufferCount) = 0;


	virtual uint32 GetFileSize()const = 0;
	virtual bool IsOpen()const = 0;
	virtual const string GetFullFileName()const = 0;
	virtual const uint32 GetPos()const = 0;
protected:
	FILE* file;
	uint32 fileSize;
	string fileName;
	uint32 pos;

};

#endif

