#pragma once
#define HAVE_STRUCT_TIMESPEC
#include <memory>
#include "pthread.h"
#include <Windows.h>

typedef void* (*PSTART_ROUTINE) (void*);
typedef void (*PFN_THREADTASK)(void *pFunctionParam, HRESULT *phr);

class CThreadPool
{
public:
    CThreadPool();
    ~CThreadPool();

    HRESULT Initialize(int nThreadCount);
    HRESULT Uninitialize();
    int     QueryThreadCount();
    HRESULT AssignTask(PFN_THREADTASK pfnThreadTask, PVOID pThreadFunctionParam, HRESULT *phr, pthread_cond_t *phEventComplete, pthread_mutex_t *phMutexComplete);

private:

    struct _ThreadTaskData{
        BOOL             bTaskAssigned;
        BOOL             bExit;
        PFN_THREADTASK   pfnThreadTask;
        PVOID            pThreadFunctionParam;
        pthread_cond_t   hEventRun;
        pthread_mutex_t  hMutexRun;
        pthread_cond_t  *phEventComplete;
        pthread_mutex_t *phMutexComplete;
        HRESULT         *phr;

        BOOL             bUninitializing;
        /* If some thread is on working, and thread pool get 
        destroy at the same time, there will get crash. 
        So we use a flag to handle it.*/

        _ThreadTaskData()
            : bTaskAssigned(TRUE)
            , bExit(FALSE)
            , pfnThreadTask(NULL)
            , pThreadFunctionParam(NULL)
            , phEventComplete(NULL)
            , phMutexComplete(NULL)
            , bUninitializing(FALSE)
            , phr(NULL)
        {

        }
    };

    static HRESULT              WorkerThread( LPVOID pObject );
    HRESULT                     Reset();

    int32_t                     m_nThreadCount;
    _ThreadTaskData            *m_pThreadTaskData;
    pthread_t                  *m_pThreadHandle;
    int32_t                     m_nCurrentThreadID;
};
