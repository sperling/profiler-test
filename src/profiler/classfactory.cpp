#include "classfactory.h"

// <EMPTY> [private] CCLassFactory construction. Does nothing.
CClassFactory::CClassFactory()
{
}

// [public] CClassFactory construction, registering one item as pCoClass.
CClassFactory::CClassFactory(const COCLASS_REGISTER *pCoClass) :
m_refCount(1),
m_pCoClass(pCoClass)
{
}

// <EMPTY> [public] CClassFactory destruction. Does nothing (nothing to release anymore!).
CClassFactory::~CClassFactory()
{
}

// [public] Adds a reference to this ClassFactory.
ULONG CClassFactory::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

// [public] Removes a reference to this ClassFactory and, if no longer referenced, deletes it.
ULONG CClassFactory::Release()
{
	LONG refCount = InterlockedDecrement(&m_refCount);
	if (refCount == 0)
		delete this;

	return refCount;
}

// [public] Quesries the type of interface of this object.
HRESULT CClassFactory::QueryInterface(REFIID riid, void **ppInterface)
{
	LOG_APPEND("--- CClassFactory::QueryInterface ---");


	if (riid == IID_IUnknown)
	{
		LOG_APPEND("--- IID_Unknown ---");
		*ppInterface = static_cast<IUnknown *>(this);
	}
	else if (riid == IID_IClassFactory)
	{
		LOG_APPEND("--- IID_IClassFactory ---");
		*ppInterface = static_cast<IClassFactory *>(this);                		
	}
	else
	{
		*ppInterface = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();

	return S_OK;
}

// [public] Cretes an instance of the ppInstance.
HRESULT CClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppInstance)
{
	// aggregation is not supported by these objects
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	// ask the object to create an instance of itself, and check the iid.
	return (*m_pCoClass->pfnCreateObject)(riid, ppInstance);
}

// <EMPTY> [public] We are not required to hook any logic since this is always an in-process server.
HRESULT CClassFactory::LockServer(BOOL fLock)
{
	return S_OK;
}
