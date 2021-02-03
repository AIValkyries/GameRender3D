#pragma once
#ifndef __DataPool_H__
#define __DataPool_H__

#include "DataType.h"
#include <vector>

using namespace std;

#define INVALID_INDEX 0xFFFF
#define CLEAR_HANDLE(h)(h=INVALID_INDEX)
#define VALID_HANDLE(h)(h!=INVALID_INDEX)
#define POOL_SHIFT 16
#define POOL_ITEM_INDEX_MASK 0xFFFF
typedef uint32 PoolHandle;

template<class T>
class PoolGroup
{
public:
	PoolGroup(uint16 _maxCount);
	~PoolGroup();

	void Create(uint16 _groupIndex);
	void Destroy();

	uint16 NextMember();
	void Release(uint16 index);

	T& GetMember(uint16 index)const;
	T* GetMemberPtr(uint16 index);

	bool IsOpen()const;
	uint16 TotalOpenCount()const { return totalOpen; }

private:
	T* members;
	uint16* nextOpenList;

	uint16 maxCount;
	uint16 nextIndex;
	uint16 groupIndex;
	uint16 totalOpen;
};

template<class T>
inline PoolGroup<T>::PoolGroup(uint16 _maxCouont)
{
	maxCount = _maxCouont;
}
template<class T>
inline PoolGroup<T>::~PoolGroup() {}

template<class T>
inline void PoolGroup<T>::Create(uint16 _groupIndex)
{
	groupIndex = _groupIndex;

	members = new T[maxCount];
	nextOpenList = new uint16[maxCount];

	int32 i = 0;
	for (;i < maxCount - 1;i++)
	{
		nextOpenList[i] = uint16(i + 1);
	}

	nextOpenList[i] = uint16(i);
	nextIndex = 0;
	totalOpen = maxCount;
}

template<class T>
inline void PoolGroup<T>::Destroy()
{
	if (members)
	{
		delete[] members;
		members = 0;
	}
	if (nextOpenList)
	{
		delete[] nextOpenList;
		nextOpenList = 0;
	}
}

template<class T>
inline uint16 PoolGroup<T>::NextMember()
{
	uint16 slot = nextIndex;
	nextIndex = nextOpenList[slot];
	totalOpen--;

	nextOpenList[slot] = INVALID_INDEX;

	return slot;
}

template<class T>
inline void PoolGroup<T>::Release(uint16 index)
{
	nextOpenList[index] = totalOpen ? nextIndex : index;
	nextIndex = index;
	totalOpen++;
}

template<class T>
inline T& PoolGroup<T>::GetMember(uint16 index)const
{
	if (nextOpenList[index] != INVALID_INDEX)
		return NULL;

	return members[index];
}

template<class T>
inline T* PoolGroup<T>::GetMemberPtr(uint16 index)
{
	if (nextOpenList[index] != INVALID_INDEX)
		return NULL;

	return &members[index];
}

template<class T>
inline bool PoolGroup<T>::IsOpen()const
{
	return totalOpen > 0;
};

// 这是一种数据结构，数据池
template<class T>
class DataPool
{
public:
	DataPool();
	DataPool(uint16 _maxCount);
	~DataPool();

	typedef PoolGroup<T> MemberGroup;

	void Create();
	void DestroyAll();

	// 创建
	PoolHandle NextHandle();
	void Release(PoolHandle handle);

	T* GetMember(PoolHandle handle);

	uint32 GroupCount()const { return members.size(); }
private:
	vector<MemberGroup*> members;
	uint16 maxCount;

	PoolGroup<T>* AddGroup();
	PoolGroup<T>* FindOpenGroup(uint16* index);
	PoolGroup<T>* GetGroup(uint16 index);

	const uint16 GetGroupIndex(PoolHandle handle)const;
	const uint16 GetItemIndex(PoolHandle handle)const;
	PoolHandle BuildPoolHandle(uint16 groupIndex, uint16 itemIndex);
};

template<class T>
inline DataPool<T>::DataPool()
{
	maxCount = 0;
}

template<class T>
inline DataPool<T>::DataPool(uint16 _maxCount)
{
	maxCount = _maxCount;
}

template<class T>
inline DataPool<T>::~DataPool<T>()
{

}

template<class T>
inline void  DataPool<T>::Create()
{
	if (maxCount == 0)
		maxCount = 4;
}

template<class T>
inline void DataPool<T>::DestroyAll()
{
	for (uint32 i = 0;i < members.size();i++)
	{
		MemberGroup* group = members[i];
		group->Destroy();
		if (group) delete group;
	}
	members.clear();
}

template<class T>
inline PoolHandle DataPool<T>::NextHandle()
{
	uint16 groupIndex = 0;
	MemberGroup* openGroup = FindOpenGroup(&groupIndex);
	uint16 itemIndex = openGroup->NextMember();

	return BuildPoolHandle(groupIndex, itemIndex);
}

template<class T>
inline void DataPool<T>::Release(PoolHandle handle)
{
	uint16 groupIndex = GetGroupIndex(handle);
	uint16 itemIndex = GetItemIndex(handle);

	MemberGroup* group = GetGroup(groupIndex);
	if (group)
	{
		group->Release(itemIndex);
	}

	if (group->TotalOpenCount() == maxCount)
	{
		group->Destroy();
		members.erase(members.begin() + groupIndex);
	}
}

template<class T>
inline T* DataPool<T>::GetMember(PoolHandle handle)
{
	uint16 groupIndex = GetGroupIndex(handle);
	uint16 itemIndex = GetItemIndex(handle);

	MemberGroup* group = GetGroup(groupIndex);

	return group->GetMemberPtr(itemIndex);
};

template<class T>
inline PoolGroup<T>* DataPool<T>::AddGroup()
{
	MemberGroup* group = new MemberGroup(maxCount);
	members.push_back(group);
	group->Create(uint16(members.size() - 1));

	return group;
}

template<class T>
inline PoolGroup<T>* DataPool<T>::FindOpenGroup(uint16* index)
{
	for (uint32 i = 0;i < members.size();i++)
	{
		MemberGroup* group = members[i];
		if (group->IsOpen())
			return group;
		++(*index);
	}

	return AddGroup();
}

template<class T>
inline PoolGroup<T>* DataPool<T>::GetGroup(uint16 index)
{
	if (index >= members.size())
		return NULL;
	return members[index];
}

template<class T>
inline const uint16 DataPool<T>::GetGroupIndex(PoolHandle handle)const
{
	return handle >> POOL_SHIFT;
}

template<class T>
inline const uint16 DataPool<T>::GetItemIndex(PoolHandle handle)const
{
	return handle & POOL_ITEM_INDEX_MASK;
}

template<class T>
inline PoolHandle DataPool<T>::BuildPoolHandle(uint16 groupIndex, uint16 itemIndex)
{
	return (groupIndex << POOL_SHIFT) | itemIndex;
}


#endif

