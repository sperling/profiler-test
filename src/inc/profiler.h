#ifndef __PROFILER_H_
#define __PROFILER_H_

#include <cor.h>
#include <corprof.h>

#ifndef PLATFORM_UNIX
#include <stdio.h>
#endif

extern FILE *flog;

#define LOG_INIT() do { flog = fopen("log.txt", "w"); } while (0)
#define LOG_SHUTDOWN() do { fclose(flog); } while (0)
#define LOG_APPEND(EXPR) do { fprintf(flog, "%s\n", EXPR); fflush(flog); } while(0)
#define LOG_APPEND2(EXPR, COUNT) do { fprintf(flog, "%.*s", COUNT, "                                                                    "); fprintf(flog, EXPR); fprintf(flog, " tid: %d\n", GetCurrentThreadId()); fflush(flog); } while(0)
#define LOG_APPEND2W(EXPR, COUNT) do { fprintf(flog, "%.*s", COUNT, "                                                                    "); fwprintf(flog, EXPR); fprintf(flog, " tid: %d\n", GetCurrentThreadId()); fflush(flog); } while(0)
#define LOG_IFFAILEDRET(HR, EXPR) do { if (FAILED(HR)) { fprintf(flog, "%s , hr = %x\n", EXPR, HR); fflush(flog); return HR; } } while(0)

extern const GUID __declspec(selectany) CLSID_PROFILER = {
	0xc4d6e538, 0x1af1, 0x44d0, { 0x92, 0xc0, 0x55, 0x25, 0xde, 0x10, 0xb7, 0x26 }
};

#define PROFILER_GUID "{C4D6E538-1AF1-44D0-92C0-5525DE10B726}"

// alias' for COM method signatures
#define COM_METHOD(TYPE) TYPE STDMETHODCALLTYPE

template <class MetaInterface>
class COMPtrHolder
{
public:
	COMPtrHolder()
	{
		m_ptr = NULL;
	}

	~COMPtrHolder()
	{
		if (m_ptr != NULL)
		{
			m_ptr->Release();
			m_ptr = NULL;
		}
	}

	MetaInterface* operator->()
	{
		return m_ptr;
	}

	MetaInterface** operator&()
	{
		return &m_ptr;
	}

	operator MetaInterface*()
	{
		return m_ptr;
	}

private:
	MetaInterface* m_ptr;
};

#ifndef W
#define W(STR) 512, L##STR
#endif

class ProfilerCallback : public ICorProfilerCallback6
{
public:

	// instantiate an instance of the callback interface
	static COM_METHOD(HRESULT) CreateObject(REFIID riid, void **ppInterface);

	ProfilerCallback();
	virtual ~ProfilerCallback();

	// IUnknown
	COM_METHOD(ULONG)   AddRef();
	COM_METHOD(ULONG)   Release();
	COM_METHOD(HRESULT) QueryInterface(REFIID riid, void **ppInterface);

	// STARTUP/SHUTDOWN EVENTS
	virtual COM_METHOD(HRESULT) Initialize(IUnknown *pICorProfilerInfoUnk);
	COM_METHOD(HRESULT) Shutdown();

	// APPLICATION DOMAIN EVENTS
	COM_METHOD(HRESULT) AppDomainCreationStarted(AppDomainID appDomainID);
	COM_METHOD(HRESULT) AppDomainCreationFinished(AppDomainID appDomainID, HRESULT hrStatus);
	COM_METHOD(HRESULT) AppDomainShutdownStarted(AppDomainID appDomainID);
	COM_METHOD(HRESULT) AppDomainShutdownFinished(AppDomainID appDomainID, HRESULT hrStatus);

	// ASSEMBLY EVENTS
	COM_METHOD(HRESULT) AssemblyLoadStarted(AssemblyID assemblyID);
	COM_METHOD(HRESULT) AssemblyLoadFinished(AssemblyID assemblyID, HRESULT hrStatus);
	COM_METHOD(HRESULT) AssemblyUnloadStarted(AssemblyID assemblyID);
	COM_METHOD(HRESULT) AssemblyUnloadFinished(AssemblyID assemblyID, HRESULT hrStatus);

	// MODULE EVENTS
	COM_METHOD(HRESULT) ModuleLoadStarted(ModuleID moduleID);
	COM_METHOD(HRESULT) ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus);
	COM_METHOD(HRESULT) ModuleUnloadStarted(ModuleID moduleID);
	COM_METHOD(HRESULT) ModuleUnloadFinished(ModuleID moduleID, HRESULT hrStatus);
	COM_METHOD(HRESULT) ModuleAttachedToAssembly(ModuleID moduleID, AssemblyID assemblyID);

	// CLASS EVENTS
	COM_METHOD(HRESULT) ClassLoadStarted(ClassID classID);
	COM_METHOD(HRESULT) ClassLoadFinished(ClassID classID, HRESULT hrStatus);
	COM_METHOD(HRESULT) ClassUnloadStarted(ClassID classID);
	COM_METHOD(HRESULT) ClassUnloadFinished(ClassID classID, HRESULT hrStatus);
	COM_METHOD(HRESULT) FunctionUnloadStarted(FunctionID functionID);

	// JIT EVENTS
	COM_METHOD(HRESULT) JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock);
	COM_METHOD(HRESULT) JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock);
	COM_METHOD(HRESULT) JITCachedFunctionSearchStarted(FunctionID functionID, BOOL *pbUseCachedFunction);
	COM_METHOD(HRESULT) JITCachedFunctionSearchFinished(FunctionID functionID, COR_PRF_JIT_CACHE result);
	COM_METHOD(HRESULT) JITFunctionPitched(FunctionID functionID);
	COM_METHOD(HRESULT) JITInlining(FunctionID callerID, FunctionID calleeID, BOOL *pfShouldInline);

	// ReJIT EVENTS
	COM_METHOD(HRESULT) ReJITCompilationStarted(FunctionID functionID, ReJITID rejitID, BOOL fIsSafeToBlock);
	COM_METHOD(HRESULT) ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock);
	COM_METHOD(HRESULT) ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus);
	COM_METHOD(HRESULT) GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl);
	COM_METHOD(HRESULT) MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart [], ObjectID newObjectIDRangeStart [], SIZE_T cObjectIDRangeLength []);
	COM_METHOD(HRESULT) SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart [], SIZE_T   cObjectIDRangeLength []);
	COM_METHOD(HRESULT) ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID keyRefIds [], ObjectID valueRefIds [], GCHandleID rootIds []);

	// THREAD EVENTS
	COM_METHOD(HRESULT) ThreadCreated(ThreadID threadID);
	COM_METHOD(HRESULT) ThreadDestroyed(ThreadID threadID);
	COM_METHOD(HRESULT) ThreadAssignedToOSThread(ThreadID managedThreadID, DWORD osThreadID);

	// REMOTING EVENTS
	// > Client-side events
	COM_METHOD(HRESULT) RemotingClientInvocationStarted();
	COM_METHOD(HRESULT) RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync);
	COM_METHOD(HRESULT) RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync);
	COM_METHOD(HRESULT) RemotingClientInvocationFinished();
	// > Server-side events
	COM_METHOD(HRESULT) RemotingServerInvocationStarted();
	COM_METHOD(HRESULT) RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync);
	COM_METHOD(HRESULT) RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync);
	COM_METHOD(HRESULT) RemotingServerInvocationReturned();

	// CONTEXT EVENTS
	COM_METHOD(HRESULT) UnmanagedToManagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason);
	COM_METHOD(HRESULT) ManagedToUnmanagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason);

	// SUSPENSION EVENTS
	COM_METHOD(HRESULT) RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason);
	COM_METHOD(HRESULT) RuntimeSuspendFinished();
	COM_METHOD(HRESULT) RuntimeSuspendAborted();
	COM_METHOD(HRESULT) RuntimeResumeStarted();
	COM_METHOD(HRESULT) RuntimeResumeFinished();
	COM_METHOD(HRESULT) RuntimeThreadSuspended(ThreadID threadid);
	COM_METHOD(HRESULT) RuntimeThreadResumed(ThreadID threadid);

	// GC EVENTS
	COM_METHOD(HRESULT) MovedReferences(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart [], ObjectID newObjectIDRangeStart [], ULONG cObjectIDRangeLength []);
	COM_METHOD(HRESULT) SurvivingReferences(ULONG cmovedObjectIDRanges, ObjectID objectIDRangeStart [], ULONG cObjectIDRangeLength []);
	COM_METHOD(HRESULT) ObjectAllocated(ObjectID objectID, ClassID classID);
	COM_METHOD(HRESULT) ObjectsAllocatedByClass(ULONG classCount, ClassID classIDs [], ULONG objects []);
	COM_METHOD(HRESULT) ObjectReferences(ObjectID objectID, ClassID classID, ULONG cObjectRefs, ObjectID objectRefIDs []);
	COM_METHOD(HRESULT) RootReferences(ULONG cRootRefs, ObjectID rootRefIDs []);
	COM_METHOD(HRESULT) GarbageCollectionStarted(int cGenerations, BOOL generationCollected [], COR_PRF_GC_REASON reason);
	COM_METHOD(HRESULT) GarbageCollectionFinished();
	COM_METHOD(HRESULT) STDMETHODCALLTYPE FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID);
	COM_METHOD(HRESULT) STDMETHODCALLTYPE RootReferences2(ULONG cRootRefs, ObjectID rootRefIds [], COR_PRF_GC_ROOT_KIND rootKinds [], COR_PRF_GC_ROOT_FLAGS rootFlags [], UINT_PTR rootIds []);
	COM_METHOD(HRESULT) STDMETHODCALLTYPE HandleCreated(UINT_PTR handleId, ObjectID initialObjectId);
	COM_METHOD(HRESULT) STDMETHODCALLTYPE HandleDestroyed(UINT_PTR handleId);

	// EXCEPTION EVENTS
	// > Exception creation
	COM_METHOD(HRESULT) ExceptionThrown(ObjectID thrownObjectID);
	// > Search phase
	COM_METHOD(HRESULT) ExceptionSearchFunctionEnter(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionSearchFunctionLeave();
	COM_METHOD(HRESULT) ExceptionSearchFilterEnter(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionSearchFilterLeave();
	COM_METHOD(HRESULT) ExceptionSearchCatcherFound(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionCLRCatcherFound();
	COM_METHOD(HRESULT) ExceptionCLRCatcherExecute();
	COM_METHOD(HRESULT) ExceptionOSHandlerEnter(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionOSHandlerLeave(FunctionID functionID);
	// > Unwind phase
	COM_METHOD(HRESULT) ExceptionUnwindFunctionEnter(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionUnwindFunctionLeave();
	COM_METHOD(HRESULT) ExceptionUnwindFinallyEnter(FunctionID functionID);
	COM_METHOD(HRESULT) ExceptionUnwindFinallyLeave();
	COM_METHOD(HRESULT) ExceptionCatcherEnter(FunctionID functionID, ObjectID objectID);
	COM_METHOD(HRESULT) ExceptionCatcherLeave();

	// COM CLASSIC WRAPPER
	COM_METHOD(HRESULT)  COMClassicVTableCreated(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable, ULONG cSlots);
	COM_METHOD(HRESULT)  COMClassicVTableDestroyed(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable);
	COM_METHOD(HRESULT) STDMETHODCALLTYPE ThreadNameChanged(ThreadID threadId, ULONG cchName, __in_ecount_opt(cchName) WCHAR name []);

	// ATTACH EVENTS
	COM_METHOD(HRESULT) STDMETHODCALLTYPE InitializeForAttach(IUnknown *pICorProfilerInfoUnk, void *pvClientData, UINT cbClientData);
	COM_METHOD(HRESULT) ProfilerAttachComplete();
	COM_METHOD(HRESULT) ProfilerDetachSucceeded();

	// ICorProfilerCallback6
	COM_METHOD(HRESULT) STDMETHODCALLTYPE GetAssemblyReferences(const WCHAR *wszAssemblyPath, ICorProfilerAssemblyReferenceProvider *pAsmRefProvider);

	// used by function hooks, they have to be static
        static void  Enter( FunctionID functionID );
        static void  Leave( FunctionID functionID );
        static void  Tailcall( FunctionID functionID );
private:
	// how many references to this class instance exists.
	// only atomic increment/decrement operations should be performed on this.
	// when value equals 0, then this class instance should be deleted.
	LONG m_refCount;

	ICorProfilerInfo6 *m_pProfilerInfo;
};

#endif
