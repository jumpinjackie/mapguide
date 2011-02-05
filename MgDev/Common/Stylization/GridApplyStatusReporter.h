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

#ifndef GRIDAPPLYSTATUSREPORTER_H_
#define GRIDAPPLYSTATUSREPORTER_H_

#include "GridStatusReporter.h"

class GridStylizer;

///<summary>
/// An implementation of GridStatusReporter.
/// This class will delegate all the status change events to
/// the stylizer's reactors.
///</summary>
class GridApplyStatusReporter : public GridStatusReporter
{
public:
    GridApplyStatusReporter();

    ///<summary>
    /// Initializes this class by the three arguments.
    ///</summary>
    ///<param name="stylizer">The the stylizer which is applying style.</param>
    ///<param name="pixelCount">The count of the total pixels of the grid.</param>
    void            Init(GridStylizer *stylizer, double pixelCount);

    ///<summary>
    /// Indicates the transaction has been started.
    /// This class will notify the stylizer's reactors that
    /// the stylizer begins to apply the styles.
    ///</summary>
    virtual void     Begin();

    ///<summary>
    /// Indicates the transaction has been finished successfully.
    /// This class will notify the stylizer's reactors that
    /// the stylizer has finished applying styles.
    ///</summary>
    virtual void     Commit();

    ///<summary>
    /// Indicates the transaction encountered some error and has been terminated.
    /// This class will notify the stylizer's reactors that
    /// the stylizer failed to apply styles for some unknown reasons.
    ///</summary>
    virtual void     Rollback();

    ///<summary>
    /// Call this function when finishing a step.
    /// And check the returned value.
    /// If the returned value is true, that means the Step() function has been handled successfully.
    /// Otherwise, false means encountering some exceptions.
    ///</summary>
    virtual bool     Step(int stepSize = 1);

    ///<summary>
    /// If isTerminate is TRUE, then Step() will return FALSE to let the stylizer stop applying.
    /// It just sets a flag. And the flag will show its influence when
    /// calling Step() next time.
    ///</summary>
    void             SetTerminate(bool isTerminate);

    ///<summary>
    /// Returns m_isTerminate.
    ///</summary>
    bool             IsTerminate() const;

private:
    GridStylizer    *m_stylizer;

    // This is the total pixel count of a band. It is also the loop's count.
    double           m_totalPixelCount;

    // This is the count that stylizr has applied in one loop.
    double           m_currentPixelCount;

    // This class doesn't report its status at each Step() function.
    // It will increase the m_currentTime first. If m_currentTime is equal to
    // m_times, then it will call stylizer's Fire_OnStepApply() function to notify
    // its status.
    int              m_currentTime;
    int              m_times;

    // This is the range and the value of each step for UI to show the progress.
    int              m_beginPos;
    int              m_endPos;
    int              m_stepVal;

    // This indicates the current progress.
    int              m_currentStep;

    // If it is true, this class's Step() always returns false to let the stylizer
    // stop applying.
    bool             m_isTerminate;
};

#endif
