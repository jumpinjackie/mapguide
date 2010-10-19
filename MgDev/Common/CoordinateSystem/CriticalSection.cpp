#ifdef _WIN32
#   include <Windows.h>
#else
#  include <pthread.h>
#  ifndef __bsdi__
#    include <semaphore.h>
#  endif
#endif

#include "CriticalSection.h"

#ifdef _WIN32
    CRITICAL_SECTION CritSect;
#else //LINUX
    pthread_mutex_t CritSect;
    pthread_mutexattr_t CritSectAttributes;
#endif

//declare the "global" instance of our CustomCriticalSection object we're using throughout the whole process
CustomCriticalSection CriticalClass;

CustomCriticalSection::CustomCriticalSection()
{
    m_bInitialized = false;
    Initialize();
}

#ifdef _WIN32

CustomCriticalSection::~CustomCriticalSection()
{
    ::DeleteCriticalSection(&CritSect);
}

void CustomCriticalSection::Initialize()
{
    if(!m_bInitialized)
    {
        m_bInitialized = true;
        ::InitializeCriticalSection(&CritSect);
    }
}

void CustomCriticalSection::Enter()
{
    ::EnterCriticalSection(&CritSect);
}

void CustomCriticalSection::Leave()
{
    ::LeaveCriticalSection(&CritSect);
}

#else //LINUX

CustomCriticalSection::~CustomCriticalSection()
{
    pthread_mutex_destroy(&CritSect);
}

void CustomCriticalSection::Initialize()
{
    if(!m_bInitialized)
    {
        m_bInitialized = true;
        pthread_mutexattr_init(&CritSectAttributes);
        pthread_mutexattr_settype(&CritSectAttributes, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&CritSect, &CritSectAttributes);
        pthread_mutexattr_destroy(&CritSectAttributes);
    }
}

void CustomCriticalSection::Enter()
{
    pthread_mutex_lock(&CritSect);
}

void CustomCriticalSection::Leave()
{
    pthread_mutex_unlock(&CritSect);
}

#endif //_WIN32
