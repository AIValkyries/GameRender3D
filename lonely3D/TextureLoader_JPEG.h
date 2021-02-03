#pragma once
#ifndef __TextureLoader_JPEG_H__
#define __TextureLoader_JPEG_H__

#include "IResourceLoader.h"

extern "C"
{
#include <jpeglib.h>
#include <setjmp.h>
}

class TextureLoader_JPEG :public IResourceLoader
{
public:
	virtual bool IsALoadableFileExtension(const string& fileName);
	virtual bool Loader(IFile* file, IResourceItem* item);
	virtual void SetGameHost(GameHost* _gameHost);
};


#endif
