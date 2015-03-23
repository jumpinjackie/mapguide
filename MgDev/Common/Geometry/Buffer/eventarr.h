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
// FILE: eventarr.h.
//
// PURPOSE: Declaration of the EventIntervalArray. The EventIntervalArray
//          implements the array of EventIntervals required in Meyer's plane sweep
//          algorithm.
//
//------------------------------------------------------------------------------

#ifndef _EVENTARR_H_
#define _EVENTARR_H_

class EventIntervalArray {
public:
    /// constructor/destructor

    EventIntervalArray(const SweepTupleArray &tupleArray);
    virtual ~EventIntervalArray();

    /// methods to access event intervals

    const EventInterval &operator[](int evIntervalIndex) const;
    int GetNEventIntervals() const;
    int GetContainingInterval(double x) const;

private:
    class MinVector {
    public:
        MinVector(int nEventIntervals);
        ~MinVector();
        int &operator[](int index) const;

    private:
        int m_vectorSize;
        int *m_vector;
    };

    BufferAllocator<EventInterval> m_allocator;
    double m_startAbscissa;
    double m_delta;
    MinVector *m_minVector;

    void Cleanup();
    int Hash(double abscissa) const;
};


//------------------------------------------------------------------------------
//
// METHOD: Hash().
//
// PURPOSE: Implements the Hash() method as proposed in Meyer's paper; the
//          Hash() method computes an index into the MIN vector.
//
// PARAMETERS:
//
//     Input:
//
//         abscissa - passes the abscissa (x-value) the hash value is to be
//                    computed for.
//
//     Output:
//
//         None.
//
// RETURNS: An integer index into the MIN vector.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int EventIntervalArray::Hash(double abscissa) const
{
    return static_cast<int>(::floor((abscissa - m_startAbscissa) / m_delta));

} // end: Hash()

#endif
