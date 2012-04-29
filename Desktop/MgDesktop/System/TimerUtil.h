#ifndef DESKTOP_TIMER_UTIL_H
#define DESKTOP_TIMER_UTIL_H

#ifdef WIN32   
#include <windows.h>
#else          
#include <sys/time.h>
#endif

/// \cond INTERNAL
class MG_DESKTOP_API MgTimerUtil
{
    DECLARE_CLASSNAME(MgTimerUtil)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object.
    ///
    ///

    MgTimerUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///
    ///

    virtual ~MgTimerUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieves the current time in milliseconds
    ///
    /// \return
    /// current time in milliseconds
    ///
    ///

    static double GetTime();

private:

    /// Unimplemented Methods

    MgTimerUtil(const MgTimerUtil& util);
    MgTimerUtil& operator=(const MgTimerUtil& util);

    /// Helper Methods

#ifdef WIN32
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Invoke the QueryPerformanceFrequency to initialize frequency.
    /// \param pFrequency
    /// Pointer which idicates the frequncy.
    ///

    static void GetFrequency(LARGE_INTEGER* pFrequency);
#endif

private:

#ifdef WIN32
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER count;
#else
    static timeval count;
#endif
};
/// \endcond

#endif