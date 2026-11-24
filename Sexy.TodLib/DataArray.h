#ifndef __DATAARRAY_H__
#define __DATAARRAY_H__

#include <new.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include "TodDebug.h"
#include "TodCommon.h"

enum 
{
	DATA_ARRAY_INDEX_MASK = 65535,
	DATA_ARRAY_KEY_MASK = -65536,
	DATA_ARRAY_KEY_SHIFT = 16,
	DATA_ARRAY_MAX_SIZE = 65536,
	DATA_ARRAY_KEY_FIRST = 1
};

template <typename T> class DataArray
{
public:
	class DataArrayItem
	{
	public:
		T					mItem;
		unsigned int		mID;
	};
	
public:
	DataArrayItem*			mBlock;
	unsigned int			mMaxUsedCount;
	unsigned int			mMaxSize;
	unsigned int			mFreeListHead;
	unsigned int			mSize;
	unsigned int			mNextKey;
	const char*				mName;

public:
	DataArray()
	{
		mBlock = nullptr;
		mMaxUsedCount = 0U;
		mMaxSize = 0U;
		mFreeListHead = 0U;
		mSize = 0U;
		mNextKey = 1U;
		mName = nullptr;
	}

	~DataArray()
	{
		DataArrayDispose();
	}

	void DataArrayInitialize(unsigned int theMaxSize, const char* theName)
	{
		TOD_ASSERT(mBlock == nullptr);
		mBlock = (DataArrayItem*)operator new(sizeof(DataArrayItem) * theMaxSize);
		mMaxSize = theMaxSize;
		mNextKey = 1001U;
		mName = theName;
	}

	void DataArrayDispose()
	{
		if (mBlock != nullptr)
		{
			DataArrayFreeAll();
			operator delete(mBlock);
			mBlock = nullptr;
			mMaxUsedCount = 0U;
			mMaxSize = 0U;
			mFreeListHead = 0U;
			mSize = 0U;
			mName = nullptr;
		}
	}

	void DataArrayFree(T* theItem)
	{
		DataArrayItem* aItem = (DataArrayItem*)theItem;
		TOD_ASSERT(DataArrayGet(aItem->mID) == theItem, "Failed: DataArrayFree(0x%x) in %s", theItem, mName);
		theItem->~T();
		unsigned int anId = aItem->mID & DATA_ARRAY_INDEX_MASK;
		aItem->mID = mFreeListHead;
		mFreeListHead = anId;
		mSize--;
	}

	void DataArrayFreeAll()
	{
		T* aItem = nullptr;
		while (IterateNext(aItem))
			DataArrayFree(aItem);

		mFreeListHead = 0U;
		mMaxUsedCount = 0U;
	}

	inline unsigned int DataArrayGetID(T* theItem)
	{
		DataArrayItem* aItem = (DataArrayItem*)theItem;
		TOD_ASSERT(DataArrayGet(aItem->mID) == theItem, "Failed: DataArrayGetID(0x%x) for %s", theItem, mName);
		return aItem->mID;
	}

	bool IterateNext(T*& theItem)
	{
		if (mBlock == nullptr) return false;
		DataArray<T>::DataArrayItem* aItem = (DataArray<T>::DataArrayItem*)theItem;
		if (aItem == nullptr)
			aItem = &mBlock[0];
		else
			aItem++;

		DataArray<T>::DataArrayItem* aLast = &mBlock[mMaxUsedCount];
		while (aItem < aLast)
		{
			if (aItem->mID & DATA_ARRAY_KEY_MASK)
			{
				theItem = (T*)aItem;
				return true;
			}
			aItem++;
		}
		return false;
	}

	T* DataArrayAlloc()
	{
		if (mBlock == nullptr || mSize >= mMaxSize)
			throw std::runtime_error(std::string("DataArray capacity exceeded: ") + (mName ? mName : "unnamed"));
		if (mFreeListHead > mMaxUsedCount || mMaxUsedCount > mMaxSize)
			throw std::runtime_error(std::string("DataArray free list is invalid: ") + (mName ? mName : "unnamed"));
		TOD_ASSERT(mSize < mMaxSize, "Data array full: %s", mName);
		TOD_ASSERT(mFreeListHead <= mMaxUsedCount, "DataArrayAlloc error in %s", mName);
		unsigned int aNext = mMaxUsedCount;
		if (mFreeListHead == mMaxUsedCount)
			mFreeListHead = ++mMaxUsedCount;
		else
		{
			aNext = mFreeListHead;
			mFreeListHead = mBlock[mFreeListHead].mID;
		}

		DataArray<T>::DataArrayItem* aNewItem = &mBlock[aNext];
		memset(aNewItem, 0, sizeof(DataArrayItem));
		aNewItem->mID = (mNextKey++ << DATA_ARRAY_KEY_SHIFT) | aNext;
		if (mNextKey == DATA_ARRAY_MAX_SIZE) mNextKey = 1;
		mSize++;

		new (aNewItem)T();
		return (T*)aNewItem;
	}

	T* DataArrayTryToGet(unsigned int theId)
	{
		if (!theId || (theId & DATA_ARRAY_INDEX_MASK) >= mMaxSize)
			return nullptr;

		DataArrayItem* aBlock = &mBlock[theId & DATA_ARRAY_INDEX_MASK];
		return aBlock->mID == theId ? &aBlock->mItem : nullptr;
	}

	T* DataArrayGet(unsigned int theId)
	{
		T* item = DataArrayTryToGet(theId);
		if (!item)
			throw std::runtime_error(std::string("Invalid DataArray ID ") + std::to_string(theId) +
				" in " + (mName ? mName : "unnamed"));
		return item;
	}
};

#endif
