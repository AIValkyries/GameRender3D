#pragma once
#ifndef __IndexBuffer_H__
#define __IndexBuffer_H__

#include "DataType.h"
#include <vector>
#include "Types.h"


class IndexBuffer
{
public:
	IndexBuffer(INDEX_TYPE type);

	void SetType(INDEX_TYPE IndexType)
	{
		indexType = IndexType;
	}

	void Clear()
	{
		indices16.clear();
		indices32.clear();
	}

	INDEX_TYPE GetType()const 
	{ 
		return (INDEX_TYPE)indexType;
	}
	uint32 Stride()const 
	{ 
		if (indexType == INDEX_16BIT)
		{
			return 2;
		}
		else
		{
			return 4;
		}
	}
	uint32 Size()const 
	{
		if (indexType == INDEX_16BIT)
		{
			return indices16.size();
		}
		else
		{
			return indices32.size();
		}
	}
	uint32 GetPrimitiveCount()const 
	{
		if (indexType == INDEX_16BIT)
		{
			return indices16.size() / 3;
		}
		else
		{
			return indices32.size() / 3;
		}
	}

	void Remove(uint32& element)
	{
		if (indexType == INDEX_16BIT)
		{
			vector<uint16>::iterator itr;

			for (itr = indices16.begin();itr != indices16.end();)
			{
				if (*itr== element)
				{
					itr = indices16.erase(itr);
				}
				else
				{
					itr++;
				}
			}
		}
		else
		{
			vector<uint32>::iterator itr;

			for (itr = indices32.begin();itr != indices32.end();)
			{
				if (*itr == element)
				{
					itr = indices32.erase(itr);
				}
				else
				{
					itr++;
				}
			}
		}

	}
	void PushBack(const uint32& element) 
	{ 
		if (indexType == INDEX_16BIT)
		{
			indices16.push_back((uint16)element);
		}
		else
		{
			indices32.push_back(element);
		}
	}
	int32 operator [](uint32 index) 
	{ 
		if (indexType == INDEX_16BIT)
		{
			return indices16[index];
		}
		else
		{
			return indices32[index];
		}
	}

	uint32 GetValue(uint32 index)const 
	{
		if (indexType == INDEX_16BIT)
		{
			return indices16[index];
		}
		else
		{
			return indices32[index];
		}
	}
	void SetValue(uint32 index, uint32 value)
	{
		if (indexType == INDEX_16BIT)
		{
			indices16[index] = (uint16)value;
		}
		else
		{
			indices32[index] = value;
		}
	}

private:
	vector<uint16> indices16;
	vector<uint32> indices32;

	int32 indexType;

};

inline IndexBuffer::IndexBuffer(INDEX_TYPE type)
{
	SetType(type);
}

#endif

