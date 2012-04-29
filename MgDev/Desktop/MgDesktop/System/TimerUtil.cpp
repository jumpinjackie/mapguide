#include "MgDesktop.h"
#include "TimerUtil.h"

#ifdef WIN32
LARGE_INTEGER MgTimerUtil::frequency = {0};
LARGE_INTEGER MgTimerUtil::count = {0};
#else
timeval MgTimerUtil::count = {0};
#endif

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTimerUtil::MgTimerUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTimerUtil::~MgTimerUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves the current time in milliseconds.
/// </summary>
///----------------------------------------------------------------------------

double MgTimerUtil::GetTime()
{
#ifdef WIN32
    if(0 == frequency.QuadPart)
    {
        GetFrequency(&frequency);
    }
    QueryPerformanceCounter(&count);

    return count.QuadPart* (1000.0 / frequency.QuadPart);
#else
    gettimeofday(&count, NULL);
    return (count.tv_sec * 1000.0) + (count.tv_usec / 1000.0);
#endif
}

#ifdef WIN32
///----------------------------------------------------------------------------
/// <summary>
/// Invoke the QueryPerformanceFrequency to initialize class member frequency.
/// </summary>
///----------------------------------------------------------------------------

void MgTimerUtil::GetFrequency(LARGE_INTEGER* pFrequency)
{
    if(!QueryPerformanceFrequency(pFrequency))
        throw new MgUnclassifiedException(L"MgTimerUtil.GetFrequency", __LINE__, __WFILE__, NULL, L"", NULL);
}
#endif
