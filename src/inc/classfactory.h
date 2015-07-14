#ifndef __CLASSFACTORY_H__
#define __CLASSFACTORY_H__

#include "profiler.h"

// create a new instance of an object.
typedef HRESULT(__stdcall * PFN_CREATE_OBJ)(REFIID riid, void **ppInterface);

//************************************************************************************//

//******************         COCLASS_REGISTER Declaration          ******************//

//************************************************************************************//

struct COCLASS_REGISTER
{
	const GUID *pClsid;             // Class ID of the coclass
	const char *szProgID;           // Prog ID of the class
	PFN_CREATE_OBJ pfnCreateObject; // function to create instance
};

// this map contains the list of coclasses which are exported from this module
const COCLASS_REGISTER g_CoClasses [] = {
	{
		&CLSID_PROFILER,
		PROFILER_GUID,
		ProfilerCallback::CreateObject
	},
	{
		NULL,
		NULL,
		NULL
	}
};

//************************************************************************************//

//******************          CClassFactory Declaration            ******************//

//************************************************************************************//

class CClassFactory :
	public IClassFactory
{
private:
	CClassFactory();

public:
	CClassFactory(const COCLASS_REGISTER *pCoClass);
	virtual ~CClassFactory();

public:
	// IUnknown 
	COM_METHOD(ULONG) AddRef();
	COM_METHOD(ULONG) Release();
	COM_METHOD(HRESULT) QueryInterface(REFIID riid, void **ppInterface);

	// IClassFactory 
	COM_METHOD(HRESULT) LockServer(BOOL fLock);
	COM_METHOD(HRESULT) CreateInstance(IUnknown *pUnkOuter,
		REFIID riid,
		void **ppInterface);

private:
	LONG m_refCount;
	const COCLASS_REGISTER *m_pCoClass;

};

#endif
