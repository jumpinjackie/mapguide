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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: progcbck.h.
//
// PURPOSE: Declaration of the ProgressCallback object. The ProgressCallback
//          object is used in the buffering library to report progress, and
//          to check for cancelation of a buffering operation. Progress is
//          computed using an interval based approach. Intervals may be recurs-
//          ively subdivided into subintervals, with each subinterval occupying
//          an equal increment of the progress range associated with its parent
//          interval. A single interval exists at the uppermost level, and spans
//          the progress range of 0% to 100%. Progress is advanced within the
//          appropriate parent interval on each call to EndProgressInterval(),
//          or AdvanceOneSubInterval(). Clients of the buffering library are
//          expected to provide derived versions of this class that implement
//          the protected GetCanceled() and ReportProgress() methods to handle
//          the actual mechanics of checking for cancelation and reporting
//          progress.
//
//------------------------------------------------------------------------------

#ifndef _PROGCBCK_H_
#define _PROGCBCK_H_

class ProgressCallback
{
public:
    /// constructor/destructor

    ProgressCallback();
    virtual ~ProgressCallback();

    /// methods to begin/end progress intervals, and to advance one subinterval

    void BeginProgressInterval(int nSubIntervals = 1);
    void EndProgressInterval();
    void AdvanceOneSubInterval();

    /// method to check for cancelation, and to set the canceled flag

    virtual BOOL IsCanceled();
    void SetCanceled();

protected:
    /// virtual methods overridden in derived versions to handle the mechanics of
    /// checking for cancelation, and to report the current amount of progress

    virtual BOOL GetCanceled();
    virtual void ReportProgress(double percentProgress);

private:
    struct ProgressIntervalInfo {
        int m_nSubIntervals;
        int m_currSubInterval;
        double m_baseProgress;
        double m_progressIncrement;
    };

    BOOL m_canceled;
    long m_lastCancelationCheck;
    int m_topOfStack;
    ProgressIntervalInfo *m_intervalInfoStack;
    static const int StackSize;
};

#endif
