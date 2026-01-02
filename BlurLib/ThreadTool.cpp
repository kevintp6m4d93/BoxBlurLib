#include "ThreadTool.h"

CThreadPool::CThreadPool()
    : m_nThreadCount(0)
    , m_pThreadTaskData(NULL)
    , m_pThreadHandle(NULL)
    , m_nCurrentThreadID(0)
{
}

CThreadPool::~CThreadPool()
{
    Uninitialize();
}

HRESULT CThreadPool::Uninitialize()
{
    if(m_nThreadCount > 0)
    {
        for(int32_t index = 0 ; index < m_nThreadCount ; ++index)
        {
            pthread_mutex_lock(&(m_pThreadTaskData[index].hMutexRun));
            m_pThreadTaskData[index].bExit = TRUE;
            m_pThreadTaskData[index].bUninitializing = TRUE;
            pthread_cond_signal( &(m_pThreadTaskData[index].hEventRun) );
            pthread_mutex_unlock(&(m_pThreadTaskData[index].hMutexRun));
        }

        for(int32_t index = 0 ; index < m_nThreadCount ; ++index)
        {
            if( pthread_join( m_pThreadHandle[index], NULL ) != 0 )
            {
                //DebugMsg("[CThreadPool] thread destroy fail!!!");
            }

            pthread_cond_destroy( &(m_pThreadTaskData[index].hEventRun) );
            pthread_mutex_destroy( &(m_pThreadTaskData[index].hMutexRun) );
            m_pThreadTaskData[index].phEventComplete = NULL;
            m_pThreadTaskData[index].phMutexComplete = NULL;
            m_pThreadTaskData[index].pfnThreadTask = NULL;
            m_pThreadTaskData[index].pThreadFunctionParam = NULL;
            m_pThreadTaskData[index].phr = NULL;
        }

        delete [] m_pThreadTaskData;
        delete [] m_pThreadHandle;

        m_nThreadCount = 0;
        m_pThreadTaskData = NULL;
    }

    m_nCurrentThreadID = 0;

    return S_OK;
}

HRESULT CThreadPool::Initialize(int32_t nThreadCount)
{
    Uninitialize();

    m_nThreadCount = nThreadCount;

    m_pThreadTaskData = new _ThreadTaskData[m_nThreadCount];
    for (int i = 0 ; i < m_nThreadCount ; ++i)
    {
        pthread_cond_init( &(m_pThreadTaskData[i].hEventRun), NULL );
        pthread_mutex_init( &(m_pThreadTaskData[i].hMutexRun), NULL);
    }

    m_pThreadHandle = new pthread_t [m_nThreadCount];

    DWORD dwThreadID;
    for(int32_t index = 0 ; index < m_nThreadCount ; ++index)
    {
        dwThreadID = pthread_create(
            &(m_pThreadHandle[index]),
            NULL,
            (PSTART_ROUTINE)WorkerThread,
            (PVOID)&(m_pThreadTaskData[index]) );
    }

    return S_OK;
}

int CThreadPool::QueryThreadCount()
{
    return m_nThreadCount;
}

HRESULT CThreadPool::WorkerThread(LPVOID pObject)
{
    if ( pObject == NULL ) return E_POINTER;
    //_ChkPtrRtn_(pObject);
    _ThreadTaskData *pTaskData = (_ThreadTaskData*)pObject;

    while(TRUE)
    {
        pthread_mutex_lock(&(pTaskData->hMutexRun));

        pTaskData->bTaskAssigned = FALSE;

        if(!pTaskData->bUninitializing)
            pthread_cond_wait( &(pTaskData->hEventRun), &(pTaskData->hMutexRun) );
        
        pthread_mutex_unlock(&(pTaskData->hMutexRun));

        if(pTaskData->bExit == TRUE)
            break;

        if(pTaskData->bTaskAssigned == TRUE)
        {
            if(pTaskData->pfnThreadTask)
            {
                pTaskData->pfnThreadTask(pTaskData->pThreadFunctionParam, pTaskData->phr);
            }

            pthread_mutex_lock(pTaskData->phMutexComplete);
            pthread_cond_signal( pTaskData->phEventComplete );
            pthread_mutex_unlock(pTaskData->phMutexComplete);
        }
    }

    return S_OK;
}

HRESULT CThreadPool::AssignTask(PFN_THREADTASK pfnThreadTask, PVOID pThreadFunctionParam, HRESULT *phr, pthread_cond_t *phEventComplete, pthread_mutex_t *phMutexComplete)
{
    if((m_nThreadCount == 0) || (pfnThreadTask == NULL))
        return E_INVALIDARG;

    while(true)
    {
        BOOL bFound = FALSE;
        for ( int i = 0 ; i < m_nThreadCount; ++i )
        {
            m_nCurrentThreadID = (m_nCurrentThreadID + 1) % m_nThreadCount;
            if(m_pThreadTaskData[m_nCurrentThreadID].bTaskAssigned == FALSE)
            {
                bFound = TRUE;
                break;
            }
        }

        if(bFound)
            break;

        //Sleep(1);
    }

    m_pThreadTaskData[m_nCurrentThreadID].bTaskAssigned = TRUE;
    m_pThreadTaskData[m_nCurrentThreadID].pfnThreadTask = pfnThreadTask;
    m_pThreadTaskData[m_nCurrentThreadID].pThreadFunctionParam = pThreadFunctionParam;
    m_pThreadTaskData[m_nCurrentThreadID].phEventComplete = phEventComplete;
    m_pThreadTaskData[m_nCurrentThreadID].phMutexComplete = phMutexComplete;
    m_pThreadTaskData[m_nCurrentThreadID].phr = phr;

    pthread_mutex_lock(&(m_pThreadTaskData[m_nCurrentThreadID].hMutexRun));
    pthread_cond_signal(&(m_pThreadTaskData[m_nCurrentThreadID].hEventRun));
    pthread_mutex_unlock(&(m_pThreadTaskData[m_nCurrentThreadID].hMutexRun));
    return S_OK;
}

