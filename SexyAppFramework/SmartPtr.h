#ifndef __SEXY_SMARTPTR_H__
#define __SEXY_SMARTPTR_H__
#include "Common.h"

#pragma pack(push,8)                                                  

namespace Sexy
{

                                                                               
                                                                               

class RefCount
{
private:
	mutable long mRefCount;

protected: 
	                                                                                         
	                                                                                               
	                                                                                            
	                                 
	virtual ~RefCount() {}

public:
	RefCount() : mRefCount(0) {}

	const RefCount* CreateRef() const
	{ 
		InterlockedIncrement(&mRefCount);
		return this;
	}
	
	void Release()
	{
		if(InterlockedDecrement(&mRefCount)<=0)
			delete this;
	}

	                                                                                  
	                                                                                 
	                        
	RefCount(const RefCount&) : mRefCount(0) {}
	RefCount& operator=(const RefCount&) { return *this; }

	unsigned long GetRefCount() { return mRefCount; }	
};

                                                                               
                                                                               

                                                                                   
                                                                                            
template <class T> class ConstSmartPtr
{
protected:
	T *p;

public:
	ConstSmartPtr() : p(NULL) {}

	                                                                    
	ConstSmartPtr(const T* theObject) : p((T*)(theObject?theObject->CreateRef():NULL)) {} 
	ConstSmartPtr(const ConstSmartPtr& theCopy) : p((T*)(theCopy?theCopy.p->CreateRef():NULL)){}

	                                                      
	~ConstSmartPtr() { if(p!=NULL) p->Release(); }

	                                                                         
	const T* operator->() const { return p; }

	                                                                  
	operator const T*() const { return p; }

	                                                                                                 
	const T* operator=(const T* thePtr) 
	{ 
		if(p!=thePtr)                           
		{
			if(p!=NULL) p->Release(); 
			p = (T*)(thePtr?thePtr->CreateRef():NULL); 
		}

		return thePtr; 
	}	

	const ConstSmartPtr& operator=(const ConstSmartPtr& theCopy)
	{
		operator=(theCopy.get());
		return *this;
	}

	                                             
	bool operator==(const T* thePtr) const { return p==thePtr; }
	bool operator!=(const T* thePtr) const { return p!=thePtr; }
	bool operator<(const T *thePtr) const { return p < thePtr; } 

	                            
	const T* get() const { return p; }

	struct Comp { bool operator()(const ConstSmartPtr &a, const ConstSmartPtr &b) const { return a.get()<b.get(); } };
};


                                                                             
                                                                                                
template <class T> class SmartPtr : public ConstSmartPtr<T>
{
public:
	SmartPtr() {}
	SmartPtr(T* theObject) : ConstSmartPtr<T>(theObject){}
	SmartPtr(const SmartPtr& theCopy) : ConstSmartPtr<T>(theCopy.p) {}

	const SmartPtr& operator=(const SmartPtr& theCopy)
	{
		ConstSmartPtr<T>::operator=(theCopy.get());
		return *this;
	}

	T* operator=(T* thePtr)
	{
		ConstSmartPtr<T>::operator =(thePtr);
		return thePtr;
	}


	T* operator->() const { return p; }
	operator T*() const { return p; }

	T* get() const { return p; }
};

                                                                               
                                                                               

typedef SmartPtr<RefCount> RefCountPtr;

}                  

#pragma pack(pop)

                                                                                                       
#define SEXY_PTR_FORWARD(X) class X; typedef SmartPtr<X> X##Ptr
#define SEXY_PTR_FORWARDNS(X) class X; typedef Sexy::SmartPtr<X> X##Ptr

#endif
