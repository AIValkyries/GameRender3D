#pragma once
#ifndef __ResourceLoaderManager_H__
#define __ResourceLoaderManager_H__

#include "IResourceLoader.h"
#include "Singleton.h"
#include "SkinnedSkeleton.h"
#include "TextureLoader_BMP.h"
#include "TextureLoader_JPEG.h"
#include "TextureLoader_PNG.h"
#include "MeshLoader_X.h"
#include "MeshLoader_OGRE.h"

class ResourceLoaderManager:public Singleton<ResourceLoaderManager>
{
public:
	ResourceLoaderManager();
	vector<IResourceLoader*> GetAllLoaders() { return resLoaders; }

private:
	vector<IResourceLoader*> resLoaders;
};
inline ResourceLoaderManager::ResourceLoaderManager() 
	:Singleton<ResourceLoaderManager>(*this)
{
	// ×ÊÔ´¼ÓÔØÆ÷
	TextureLoader_BMP* bmp = new TextureLoader_BMP();
	TextureLoader_JPEG* jpeg = new TextureLoader_JPEG();
	TextureLoader_PNG* png = new TextureLoader_PNG();
	MeshLoader_X* meshX = new MeshLoader_X();
	MeshLoader_OGRE* meshOgre = new MeshLoader_OGRE();

	resLoaders.push_back(bmp);
	resLoaders.push_back(jpeg);
	resLoaders.push_back(png);
	resLoaders.push_back(meshX);
	resLoaders.push_back(meshOgre);
}


static ResourceLoaderManager* resLoaderManager 
= ResourceLoaderManager::instance();

#endif
