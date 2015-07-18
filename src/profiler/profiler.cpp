#include "profiler.h"

EXTERN_C void EnterNaked3(FunctionIDOrClientID functionIDOrClientID);
EXTERN_C void LeaveNaked3(FunctionIDOrClientID functionIDOrClientID);
EXTERN_C void TailcallNaked3(FunctionIDOrClientID functionIDOrClientID);


/***************************************************************************************
 ********************                                               ********************
 ********************   Global Functions Used by Function Hooks     ********************
 ********************                                               ********************
 ***************************************************************************************/

//
// The functions EnterStub, LeaveStub and TailcallStub are wrappers. The use of 
// of the extended attribute "__declspec( naked )" does not allow a direct call
// to a profiler callback (e.g., ProfilerCallback::Enter( functionID )).
//
// The enter/leave function hooks must necessarily use the extended attribute
// "__declspec( naked )". Please read the corprof.idl for more details. 
//

/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
EXTERN_C void __stdcall EnterStub( FunctionID functionID )
{
    ProfilerCallback::Enter( functionID );
    
} // EnterStub


/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
EXTERN_C void __stdcall LeaveStub( FunctionID functionID )
{
    ProfilerCallback::Leave( functionID );
    
} // LeaveStub


/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
EXTERN_C void __stdcall TailcallStub( FunctionID functionID )
{
    ProfilerCallback::Tailcall( functionID );
    
} // TailcallStub

FILE *flog;

#define _TR

#ifdef _TR

static DWORD tlsIndex;

#define TR_START() do { tlsIndex = TlsAlloc(); TlsSetValue(tlsIndex, reinterpret_cast<LPVOID>(0)); } while (0)
#define TR_PUSH() do { TlsSetValue(tlsIndex, reinterpret_cast<LPVOID>(reinterpret_cast<int>(TlsGetValue(tlsIndex)) + 1)); } while (0)
#define TR_POP() do { int vv = reinterpret_cast<int>(TlsGetValue(tlsIndex)) - 1; /* are getting a AppDomainCreationFinished without a AppDomainCreationStarted...*/if (vv < 0) { vv = 0; } TlsSetValue(tlsIndex, reinterpret_cast<LPVOID>(vv)); } while (0)
#define TR(STR) LOG_APPEND2("ProfilerCallback::" STR, reinterpret_cast<int>(TlsGetValue(tlsIndex)))

#else

#define TR_START()
#define TR_PUSH()
#define TR_POP()
#define TR(STR)

#endif

__forceinline void ProfilerCallback::Enter(FunctionID functionID)
{
	//TR("Enter");
}

__forceinline void ProfilerCallback::Leave(FunctionID functionID)
{
	TR("Leave");
}


__forceinline void ProfilerCallback::Tailcall(FunctionID functionID)
{
	TR("Tailcall");
}


// [public] Creates a new ProfilerCallback instance
HRESULT ProfilerCallback::CreateObject(REFIID riid, void **ppInterface)
{
	LOG_APPEND("CreateObject");

	TR_START();

	*ppInterface = NULL;

	if ((riid != IID_IUnknown) &&
		(riid != IID_ICorProfilerCallback5) &&
		(riid != IID_ICorProfilerCallback) &&
		(riid != IID_ICorProfilerCallback2) &&
		(riid != IID_ICorProfilerCallback3) &&
		(riid != IID_ICorProfilerCallback4) &&
		(riid != IID_ICorProfilerCallback5) &&
		(riid != IID_ICorProfilerCallback6))
	{
		return E_NOINTERFACE;
	}

	// This profiler implements the "profile-first" alternative of dealing
	// with multiple in-process side-by-side CLR instances.  First CLR
	// to try to load us into this process wins
	{
		static volatile LONG s_nFirstTime = 1;
		if (s_nFirstTime == 0)
		{
			return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
		}

		// Dirty-read says this is the first load.  Double-check that
		// with a clean-read
		if (InterlockedCompareExchange(&s_nFirstTime, 0, 1) == 0)
		{
			// Someone beat us to it
			return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
		}
	}

	ProfilerCallback * pProfilerCallback = new ProfilerCallback();
	if (pProfilerCallback == NULL)
	{
		return E_OUTOFMEMORY;
	}

	pProfilerCallback->AddRef();	
	*ppInterface = static_cast<ICorProfilerCallback6 *>(pProfilerCallback);

	LOG_APPEND("Profiler succesfully entered.");

	return S_OK;
}

// [public] Creates a new instance of the profiler and zeroes all members
ProfilerCallback::ProfilerCallback() :
	m_refCount(0),
	m_pProfilerInfo(nullptr)
{
}

ProfilerCallback::~ProfilerCallback()
{
	LOG_SHUTDOWN();
}

// [public] IUnknown method, increments refcount to keep track of when to call destructor
ULONG ProfilerCallback::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

// [public] IUnknown method, decrements refcount and deletes if unreferenced
ULONG ProfilerCallback::Release()
{
	LONG refCount = InterlockedDecrement(&m_refCount);
	if (refCount == 0)
		delete this;

	return refCount;
}

// [public] IUnknown method, gets the interface (The profiler only supports ICorProfilerCallback5)
HRESULT ProfilerCallback::QueryInterface(REFIID riid, void **ppInterface)
{
	TR("QueryInterface");

	// Get interface from riid
	if (riid == IID_IUnknown)
		*ppInterface = static_cast<IUnknown *>(this);
	else if (riid == IID_ICorProfilerCallback6)
		*ppInterface = static_cast<ICorProfilerCallback6 *>(this);
	else if (riid == IID_ICorProfilerCallback5)
		*ppInterface = static_cast<ICorProfilerCallback5 *>(this);
	else if (riid == IID_ICorProfilerCallback4)
		*ppInterface = static_cast<ICorProfilerCallback4 *>(this);
	else if (riid == IID_ICorProfilerCallback3)
		*ppInterface = static_cast<ICorProfilerCallback3 *>(this);
	else if (riid == IID_ICorProfilerCallback2)
		*ppInterface = static_cast<ICorProfilerCallback2 *>(this);
	else if (riid == IID_ICorProfilerCallback)
		*ppInterface = static_cast<ICorProfilerCallback *>(this);
	else
	{
		*ppInterface = NULL;		
	}

	// Interface was successfully inferred, increase its reference count.
	reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();

	return S_OK;
}

// [public] Initializes the profiler using the given (hopefully) instance of ICorProfilerCallback6
HRESULT ProfilerCallback::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	TR("Initialize");

	HRESULT hr;

	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo6, (void **) &m_pProfilerInfo);
	LOG_IFFAILEDRET(hr, "QueryInterface for ICorProfilerInfo6 failed in ::Initialize");
	
	hr = m_pProfilerInfo->SetEventMask(
		COR_PRF_MONITOR_MODULE_LOADS |
		COR_PRF_MONITOR_ASSEMBLY_LOADS |
		COR_PRF_MONITOR_APPDOMAIN_LOADS |
		COR_PRF_MONITOR_JIT_COMPILATION |
		COR_PRF_MONITOR_GC |
#ifndef PLATFORM_UNIX
// TODO:	will thrash *obj(System.AppDomain) in JIT_MonReliableEnter_Portable(System.Threading.Monitor.ReliableEnter)
//		    for System.Threading.Monitor.Enter call. because of static, nested, byref or something wrong with EnterNaked3?
//			EnterNaked3 gets called for System.AppDomain.SetupDomain and System.Threading.Monitor.Enter, so looks ok.
        COR_PRF_MONITOR_ENTERLEAVE |
#endif
		//COR_PRF_ENABLE_REJIT |
		//COR_PRF_DISABLE_ALL_NGEN_IMAGES |
		COR_PRF_USE_PROFILE_IMAGES |
		COR_PRF_DISABLE_TRANSPARENCY_CHECKS_UNDER_FULL_TRUST |
		COR_PRF_MONITOR_EXCEPTIONS |
		COR_PRF_ENABLE_STACK_SNAPSHOT);

	LOG_IFFAILEDRET(hr, "SetEventMask failed in ::Initialize");
	
#ifndef PLATFORM_UNIX
	hr = m_pProfilerInfo->SetEnterLeaveFunctionHooks3((FunctionEnter3 *)EnterNaked3,
                                                      NULL,//(FunctionLeave3 *)LeaveNaked3,
                                                      NULL);//(FunctionTailcall3 *)TailcallNaked3 );
    LOG_IFFAILEDRET(hr, "SetEnterLeaveFunctionHooks3 failed in ::Initialize");
#endif
	return S_OK;
}

HRESULT ProfilerCallback::Shutdown()
{
	TR("Shutdown");
	return S_OK;
}

HRESULT ProfilerCallback::AppDomainCreationStarted(AppDomainID appDomainID)
{	
	TR("AppDomainCreationStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AppDomainCreationFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
	TR_POP();
	TR("AppDomainCreationFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AppDomainShutdownStarted(AppDomainID appDomainID)
{
	TR("AppDomainShutdownStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AppDomainShutdownFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
	TR_POP();
	TR("AppDomainShutdownFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AssemblyLoadStarted(AssemblyID assemblyId)
{
	TR("AssemblyLoadStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	TR_POP();
	TR("AssemblyLoadFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AssemblyUnloadStarted(AssemblyID assemblyID)
{
	TR("AssemblyUnloadStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::AssemblyUnloadFinished(AssemblyID assemblyID, HRESULT hrStatus)
{
	TR_POP();
	TR("AssemblyUnloadFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ModuleLoadStarted(ModuleID moduleID)
{
	TR("ModuleLoadStarted");
	TR_PUSH();
	return S_OK;
}

// [public] 
HRESULT ProfilerCallback::ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	TR_POP();
	TR("ModuleLoadFinished");
	return S_OK;
}

// Don't forget--modules can unload!  Remove it from our records when it does.
HRESULT ProfilerCallback::ModuleUnloadStarted(ModuleID moduleID)
{
	TR("ModuleUnloadStarted");
	TR_PUSH();
	return S_OK;
}

HRESULT ProfilerCallback::ModuleUnloadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	TR_POP();
	TR("ModuleUnloadFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ModuleAttachedToAssembly(ModuleID moduleID, AssemblyID assemblyID)
{
	TR("ModuleAttachedToAssembly");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ClassLoadStarted(ClassID classID)
{
	TR("ClassLoadStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ClassLoadFinished(ClassID classID, HRESULT hrStatus)
{
	TR_POP();
	TR("ClassLoadFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ClassUnloadStarted(ClassID classID)
{
	TR("ClassUnloadStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ClassUnloadFinished(ClassID classID, HRESULT hrStatus)
{
	TR_POP();
	TR("ClassUnloadFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::FunctionUnloadStarted(FunctionID functionID)
{
	TR("FunctionUnloadStarted");
	return S_OK;
}

HRESULT ProfilerCallback::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock)
{
	TR("JITCompilationStarted");
	TR_PUSH();
	return S_OK;
}

HRESULT ProfilerCallback::JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	TR_POP();
	TR("JITCompilationFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::JITCachedFunctionSearchStarted(FunctionID functionID, BOOL *pbUseCachedFunction)
{
	TR("JITCachedFunctionSearchStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::JITCachedFunctionSearchFinished(FunctionID functionID, COR_PRF_JIT_CACHE result)
{	
	TR_POP();
	TR("JITCachedFunctionSearchFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::JITFunctionPitched(FunctionID functionID)
{
	TR("JITFunctionPitched");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::JITInlining(FunctionID callerID, FunctionID calleeID, BOOL *pfShouldInline)
{
	//TR("JITInlining");
	return S_OK;
}

// [public] When a ReJIT starts, profilers don't typically need to do much in this
// method.  Here, we just do some light validation and logging.
HRESULT ProfilerCallback::ReJITCompilationStarted(FunctionID functionID, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	TR("ReJITCompilationStarted");
	TR_PUSH();
	return S_OK;
}

HRESULT ProfilerCallback::ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	TR_POP();
	TR("ReJITCompilationFinished");
	return S_OK;
}

// [public] Logs any errors encountered during ReJIT.
HRESULT ProfilerCallback::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	TR("ReJITError");
	return S_OK;
}

// [public] Here's where the real work happens when a method gets ReJITed.  This is
// responsible for getting the new (instrumented) IL to be compiled.
HRESULT ProfilerCallback::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl)
{
	TR("GetReJITParameters");
	return S_OK;
}

HRESULT ProfilerCallback::MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart [], ObjectID newObjectIDRangeStart [], SIZE_T cObjectIDRangeLength [])
{
	TR("MovedReferences2");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart [], SIZE_T   cObjectIDRangeLength [])
{
	TR("SurvivingReferences2");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID keyRefIds [], ObjectID valueRefIds [], GCHandleID rootIds [])
{
	TR("ConditionalWeakTableElementReferences");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ThreadCreated(ThreadID threadID)
{
	TR("ThreadCreated");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ThreadDestroyed(ThreadID threadID)
{
	TR("ThreadDestroyed");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ThreadAssignedToOSThread(ThreadID managedThreadID, DWORD osThreadID)
{
	TR("ThreadAssignedToOSThread");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingClientInvocationStarted()
{
	TR("RemotingClientInvocationStarted");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync)
{
	TR("RemotingClientSendingMessage");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync)
{
	TR("RemotingClientReceivingReply");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingClientInvocationFinished()
{
	TR("RemotingClientInvocationFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingServerInvocationStarted()
{
	TR("RemotingServerInvocationStarted");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync)
{
	TR("RemotingServerReceivingMessage");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync)
{
	TR("RemotingServerSendingReply");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RemotingServerInvocationReturned()
{
	TR("RemotingServerInvocationReturned");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::UnmanagedToManagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
	TR("UnmanagedToManagedTransition");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ManagedToUnmanagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
	TR("ManagedToUnmanagedTransition");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	TR("RuntimeSuspendStarted");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeSuspendFinished()
{
	TR("RuntimeSuspendFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeSuspendAborted()
{
	TR("RuntimeSuspendAborted");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeResumeStarted()
{
	TR("RuntimeResumeStarted");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeResumeFinished()
{
	TR("RuntimeResumeFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeThreadSuspended(ThreadID threadID)
{
	TR("RuntimeThreadSuspended");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RuntimeThreadResumed(ThreadID threadID)
{
	TR("RuntimeThreadResumed");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::MovedReferences(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart [], ObjectID newObjectIDRangeStart [], ULONG cObjectIDRangeLength [])
{
	TR("MovedReferences");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::SurvivingReferences(ULONG cmovedObjectIDRanges, ObjectID objectIDRangeStart [], ULONG cObjectIDRangeLength [])
{
	TR("SurvivingReferences");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ObjectsAllocatedByClass(ULONG classCount, ClassID classIDs [], ULONG objects [])
{
	TR("ObjectsAllocatedByClass");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ObjectAllocated(ObjectID objectID, ClassID classID)
{
	TR("ObjectAllocated");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ObjectReferences(ObjectID objectID, ClassID classID, ULONG objectRefs, ObjectID objectRefIDs [])
{
	TR("ObjectReferences");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RootReferences(ULONG rootRefs, ObjectID rootRefIDs [])
{
	TR("RootReferences");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::GarbageCollectionStarted(int cGenerations, BOOL generationCollected [], COR_PRF_GC_REASON reason)
{
	TR("GarbageCollectionStarted");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::GarbageCollectionFinished()
{
	TR_POP();
	TR("GarbageCollectionFinished");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
	TR("FinalizeableObjectQueued");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds [], COR_PRF_GC_ROOT_KIND rootKinds [], COR_PRF_GC_ROOT_FLAGS rootFlags [], UINT_PTR rootIds [])
{
	TR("RootReferences2");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::HandleCreated(UINT_PTR handleId, ObjectID initialObjectId)
{
	TR("HandleCreated");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::HandleDestroyed(UINT_PTR handleId)
{
	TR("HandleDestroyed");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionThrown(ObjectID thrownObjectID)
{
	TR("ExceptionThrown");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionSearchFunctionEnter(FunctionID functionID)
{
	TR("ExceptionSearchFunctionEnter");
	TR_PUSH();
	return S_OK;		 
}

// Empty method.
HRESULT ProfilerCallback::ExceptionSearchFunctionLeave()
{
	TR_POP();
	TR("ExceptionSearchFunctionLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionSearchFilterEnter(FunctionID functionID)
{
	TR("ExceptionSearchFilterEnter");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionSearchFilterLeave()
{
	TR_POP();
	TR("ExceptionSearchFilterLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionSearchCatcherFound(FunctionID functionID)
{
	TR("ExceptionSearchCatcherFound");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionCLRCatcherFound()
{
	TR("ExceptionCLRCatcherFound");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionCLRCatcherExecute()
{
	TR("ExceptionCLRCatcherExecute");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionOSHandlerEnter(FunctionID functionID)
{
	TR("ExceptionOSHandlerEnter");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionOSHandlerLeave(FunctionID functionID)
{
	TR_POP();
	TR("ExceptionOSHandlerLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionUnwindFunctionEnter(FunctionID functionID)
{
	TR("ExceptionUnwindFunctionEnter");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionUnwindFunctionLeave()
{
	TR_POP();
	TR("ExceptionUnwindFunctionLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionUnwindFinallyEnter(FunctionID functionID)
{
	TR("ExceptionUnwindFinallyEnter");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionUnwindFinallyLeave()
{
	TR_POP();
	TR("ExceptionUnwindFinallyLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionCatcherEnter(FunctionID functionID, ObjectID objectID)
{
	TR("ExceptionCatcherEnter");
	TR_PUSH();
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ExceptionCatcherLeave()
{
	TR_POP();
	TR("ExceptionCatcherLeave");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::COMClassicVTableCreated(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable, ULONG cSlots)
{
	TR("COMClassicVTableCreated");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::COMClassicVTableDestroyed(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable)
{
	TR("COMClassicVTableDestroyed");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ThreadNameChanged(ThreadID threadId, ULONG cchName, __in_ecount_opt(cchName) WCHAR name [])
{
	TR("ThreadNameChanged");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::InitializeForAttach(IUnknown *pICorProfilerInfoUnk, void *pvClientData, UINT cbClientData)
{
	TR("InitializeForAttach");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ProfilerAttachComplete()
{
	TR("ProfilerAttachComplete");
	return S_OK;
}

// Empty method.
HRESULT ProfilerCallback::ProfilerDetachSucceeded()
{
	TR("ProfilerDetachSucceeded");
	return S_OK;
}

HRESULT ProfilerCallback::GetAssemblyReferences(const WCHAR *wszAssemblyPath, ICorProfilerAssemblyReferenceProvider *pAsmRefProvider)
{
	TR("GetAssemblyReferences");
	return S_OK;
}

