//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

//------------------------------------------------------------------------------
//
// FILE: progcbck.cpp.
//
// PURPOSE: Implementation of the ProgressCallback object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

const int ProgressCallback::StackSize = 32;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a ProgressCallback object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to complete initialization.
//
//------------------------------------------------------------------------------

ProgressCallback::ProgressCallback() :
    m_canceled(FALSE),
    m_lastCancelationCheck(0),
    m_topOfStack(-1),
    m_intervalInfoStack(NULL)
{
    m_intervalInfoStack = new ProgressIntervalInfo[StackSize];

} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

ProgressCallback::~ProgressCallback()
{
    delete [] m_intervalInfoStack;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: BeginProgressInterval().
//
// PURPOSE: Begin a progress (sub)interval within the current progress interval.
//
// PARAMETERS:
//
//     Input:
//
//         nSubIntervals - passes the number of subintervals the new progress
//                         interval will consist of.
//
//     Output:
//
//         None.
//
// RETURNS:
//
// EXCEPTIONS:
//
//------------------------------------------------------------------------------

void ProgressCallback::BeginProgressInterval(int nSubIntervals)
{
    assert(nSubIntervals > 0);
    assert(m_topOfStack < StackSize - 1);

    // initialize a new progress interval info structure and push it onto the
    // stack

    if (m_topOfStack < StackSize - 1) {
        ProgressIntervalInfo intervalInfo;
        intervalInfo.m_nSubIntervals = nSubIntervals;
        intervalInfo.m_currSubInterval = 0;

        if (nSubIntervals > 0) {
            if (m_topOfStack >= 0) {
                ProgressIntervalInfo &parentInfo = m_intervalInfoStack[m_topOfStack];
                intervalInfo.m_baseProgress = parentInfo.m_baseProgress +
                    parentInfo.m_currSubInterval * parentInfo.m_progressIncrement;
                intervalInfo.m_progressIncrement = parentInfo.m_progressIncrement /
                    nSubIntervals;
            }
            else {
                intervalInfo.m_baseProgress = 0.0;
                intervalInfo.m_progressIncrement = 100.0 / nSubIntervals;
            }
        }
        else {
            intervalInfo.m_baseProgress = 0.0;
            intervalInfo.m_progressIncrement = 0.0;
        }

        m_intervalInfoStack[++m_topOfStack] = intervalInfo;
    }

} // end: BeginProgressInterval()


//------------------------------------------------------------------------------
//
// METHOD: EndProgressInterval().
//
// PURPOSE: End the current progress interval. Progress is advanced to the end
//          of the subinterval if not already done so by prior calls to Advance-
//          OneSubInterval().
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void ProgressCallback::EndProgressInterval()
{
    assert(m_topOfStack >= 0);

    // if the stack is not empty then pop the top element

    if (m_topOfStack >= 0) {
        ProgressIntervalInfo &intervalInfo = m_intervalInfoStack[m_topOfStack];

        if (intervalInfo.m_currSubInterval < intervalInfo.m_nSubIntervals &&
                !m_canceled) {
            while (intervalInfo.m_currSubInterval<intervalInfo.m_nSubIntervals)
                AdvanceOneSubInterval();
        }

        if (--m_topOfStack >= 0)
            m_intervalInfoStack[m_topOfStack].m_currSubInterval++;
    }

} // end: EndProgressInterval()


//------------------------------------------------------------------------------
//
// METHOD: AdvanceOneSubInterval().
//
// PURPOSE: Advance the progress by the amount corresponding to one subinterval
//          of the current progress interval.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void ProgressCallback::AdvanceOneSubInterval()
{
    assert(m_topOfStack >= 0);

    if (m_topOfStack >= 0) {
        ProgressIntervalInfo &intervalInfo = m_intervalInfoStack[m_topOfStack];
        assert(intervalInfo.m_currSubInterval < intervalInfo.m_nSubIntervals);
        intervalInfo.m_currSubInterval++;

        if (!m_canceled && intervalInfo.m_currSubInterval <=
                intervalInfo.m_nSubIntervals) {
            double percentProgress = intervalInfo.m_baseProgress +
                intervalInfo.m_currSubInterval *
                intervalInfo.m_progressIncrement;
            ReportProgress(percentProgress <= 100.0 ? percentProgress : 100.0);
        }
    }

} // end: EndProgressInterval()


//------------------------------------------------------------------------------
//
// METHOD: IsCanceled().
//
// PURPOSE: Check whether a cancelation has been signalled.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if a cancelation has been signalled and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL ProgressCallback::IsCanceled()
{
    // if not already canceled, and more than 1/10 of a second has passed since
    // the last check, then call the virtual GetCanceled() method to check for
    // a cancelation

    if (!m_canceled) {
        long currTime = (long)clock();

        if (currTime - m_lastCancelationCheck >= 100) {
            m_lastCancelationCheck = currTime;
            m_canceled = GetCanceled();
        }
    }

    return m_canceled;

} // end: IsCanceled()


//------------------------------------------------------------------------------
//
// METHOD: SetCanceled().
//
// PURPOSE: Set the cancelation flag to TRUE.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void ProgressCallback::SetCanceled()
{
    m_canceled = TRUE;

} // end: SetCanceled()


//------------------------------------------------------------------------------
//
// METHOD: GetCanceled().
//
// PURPOSE: Virtual method to check for a cancelation. The default implementa-
//          tion simply returns FALSE.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if canceled and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL ProgressCallback::GetCanceled()
{
    return FALSE;

} // end: GetCanceled()


//------------------------------------------------------------------------------
//
// METHOD: ReportProgress().
//
// PURPOSE: Virtual method to report the current percentage progress. The
//          default implementation does nothing.
//
// PARAMETERS:
//
//     Input:
//
//         percentProgress - passes the current percent progress.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void ProgressCallback::ReportProgress(double)
{
} // end: ReportProgress()
