#pragma once
#ifndef __ResourceItem_H__
#define __ResourceItem_H__

#include "DataPool.h"

class IResourceItem
{
public:

	void SetResourceHandle(PoolHandle handle)
	{
		resourceHandle = handle;
	}

	PoolHandle GetPoolHandle()const { return resourceHandle; }

private:
	PoolHandle resourceHandle;
};

#endif

