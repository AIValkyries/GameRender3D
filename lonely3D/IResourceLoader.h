#pragma once
#ifndef __IResourceLoader_H__
#define __IResourceLoader_H__

#include "DataType.h"
#include "IFile.h"
#include "IResourceItem.h"
#include <string>

using namespace std;
class GameHost;
 
// ��Դ��������Ŀǰ��ԴΪ texture OR mesh
class IResourceLoader
{
public:
	// �жϺ�׺��
	virtual bool IsALoadableFileExtension(const string& fileName) = 0;
	virtual bool Loader(IFile* file, IResourceItem* item) = 0;
	virtual void SetGameHost(GameHost* _gameHost) = 0;
protected:
	GameHost* gameHost;
};



#endif

