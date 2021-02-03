#pragma once
#ifndef __TextureLoader_PNG_H__
#define __TextureLoader_PNG_H__

#include "IResourceLoader.h"

class GameHost;
class TextureLoader_PNG :public IResourceLoader
{
public:
	virtual bool IsALoadableFileExtension(const string& fileName);
	virtual bool Loader(IFile* file, IResourceItem* item);
	virtual void SetGameHost(GameHost* _gameHost);
};

#endif

