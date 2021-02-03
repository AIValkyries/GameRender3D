#pragma once
#ifndef __ResourcePool_H__
#define __ResourcePool_H__

#include "DataPool.h"
#include "DataType.h"
#include "IResourceLoader.h"
#include <string>
#include <map>
#include "Debug.h"
#include "ResourceLoaderManager.h"

template<class T>
class ResourcePool
{
public:
	typedef DataPool<T> DataPool;
	typedef map<string, PoolHandle> NAME_LOOKUP_MAP;

	ResourcePool();
	~ResourcePool();

	T* CreateResource(const string& fileFullName);
	T* LoadResource(const string& fileFullName);
	void DestroyResource(const string& fileFullName);
	T* GetResource(const string& fileFullName);
	PoolHandle GetPoolHandle(const string& fileFullName);
	uint32 GetGroupCount()const { return dataPool->GroupCount(); }

	void InitResourcePool(GameHost* host);

	void DestroyAll();

private:
	DataPool* dataPool;
	NAME_LOOKUP_MAP nameMaps;
};

template<class T>
inline ResourcePool<T>::ResourcePool() 
{
	
}

template<class T>
inline ResourcePool<T>::~ResourcePool() {}

template<class T>
inline  T* ResourcePool<T>::CreateResource(const string& fileFullName)
{
	IResourceItem* item = GetResource(fileFullName);

	if (!item)
	{
		PoolHandle handle = dataPool->NextHandle();
		if (VALID_HANDLE(handle))
		{
			nameMaps[IFile::GetFileName(fileFullName)] = handle;

			item = dataPool->GetMember(handle);
			item->SetResourceHandle(handle);
		}
	}

	return (T*)item;
}

template<class T>
inline T* ResourcePool<T>::LoadResource(const string& fileFullName)
{
	IResourceItem* item = GetResource(fileFullName);

	if (!item)
	{
		PoolHandle handle = dataPool->NextHandle();
		item = dataPool->GetMember(handle);

		IFile* file = IFile::Open(fileFullName, READ_ONLY);

		vector<IResourceLoader*>  resLoaders = resLoaderManager->GetAllLoaders();

		for (uint32 i = 0;i < resLoaders.size();i++)
		{
			if (resLoaders[i]->IsALoadableFileExtension(fileFullName))
			{
				resLoaders[i]->Loader(file, item);
				break;
			}
		}

		delete file;

		item->SetResourceHandle(handle);
		nameMaps[IFile::GetFileName(fileFullName)] = handle;
	}

	return (T*)item;
}


template<class T>
inline void ResourcePool<T>::DestroyResource(const string& fileFullName)
{
	PoolHandle handle = 0;
	CLEAR_HANDLE(handle);

	string fileName = IFile::GetFileName(fileFullName);
	NAME_LOOKUP_MAP::const_iterator iter = nameMaps.find(fileName);

	if (iter != nameMaps.end())
	{
		handle = iter->second;
		if (VALID_HANDLE(handle))
			dataPool->Release(handle);

		nameMaps.erase(fileName);
	}
}

template<class T>
inline T* ResourcePool<T>::GetResource(const string& fileFullName)
{
	PoolHandle handle = 0;
	CLEAR_HANDLE(handle);

	string fileName = IFile::GetFileName(fileFullName);
	NAME_LOOKUP_MAP::const_iterator iter = nameMaps.find(fileName);

	if (iter != nameMaps.end())
	{
		handle = iter->second;
		if (VALID_HANDLE(handle))
			return  dataPool->GetMember(handle);
	}

	return NULL;
}


template<class T>
inline PoolHandle ResourcePool<T>::GetPoolHandle(const string& fileFullName)
{
	string fileName = IFile::GetFileName(fileFullName);
	NAME_LOOKUP_MAP::const_iterator iter = nameMaps.find(fileName);
	if (iter != nameMaps.end())
	{
		PoolHandle handle;
		if (VALID_HANDLE(handle))
			return handle;
	}

	return INVALID_INDEX;
}

template<class T>
inline void ResourcePool<T>::DestroyAll()
{
	dataPool->DestroyAll();
	nameMaps.clear();
}

template<class T>
inline void ResourcePool<T>::InitResourcePool(GameHost* host)
{
	dataPool = new DataPool(2);
	dataPool->Create();

	vector<IResourceLoader*>  resLoaders = resLoaderManager->GetAllLoaders();
	for (uint32 i = 0;i < resLoaders.size();i++)
	{
		resLoaders[i]->SetGameHost(host);
	}
}




#endif
