#include "classfactory.h"

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID FAR *ppv)
{
	LOG_INIT();
	LOG_APPEND("------ DllGetClassObject -----");
	CClassFactory *pClassFactory;
	const COCLASS_REGISTER *pCoClass;
	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

	// scan for the right one
	for (pCoClass = g_CoClasses; pCoClass->pClsid != NULL; pCoClass++)
	{
		if (*pCoClass->pClsid == rclsid)
		{
			pClassFactory = new CClassFactory(pCoClass);
			if (pClassFactory != NULL)
			{
				hr = pClassFactory->QueryInterface(riid, ppv);

				pClassFactory->Release();
				break;
			}
			else
			{
				hr = E_OUTOFMEMORY;
				break;
			}
		}
	}

	return hr;
}
