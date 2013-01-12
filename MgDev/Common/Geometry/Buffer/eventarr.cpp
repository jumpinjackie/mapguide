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
// FILE: eventarr.cpp.
//
// PURPOSE: Implementation of the EventIntervalArray object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

static const int EventIntervalBlockSize = 256;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an EventIntervalArray object using the
//          sweep tuples contained in the specified SweepTupleArray object.
//
// PARAMETERS:
//
//     Input:
//
//         tupleArray - passes a reference to the SweepTupleArray object to
//                      create the event intervals from.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the event intervals populating the array.
//
//------------------------------------------------------------------------------

EventIntervalArray::EventIntervalArray(const SweepTupleArray &tupleArray) :
    m_allocator(EventIntervalBlockSize),
    m_startAbscissa(0.0),
    m_delta(1.0),
    m_minVector(NULL)
{
    // scan the SweepTupleArray, and derive the event intervals

    MG_TRY()

    int nTuples = tupleArray.GetNSweepTuples();
    int nEventIntervals = 0;
    int i, j;

    for (i = 0; i < nTuples; ) {
        EventInterval *evInterval = m_allocator.GetNextObject();
        nEventIntervals++;
        SweepTupleArray::Tuple *tuple = &tupleArray[i];

        evInterval->m_tupleArray = &tupleArray;
        evInterval->m_abscissa = tuple->m_x;
        double abscissa = evInterval->m_abscissa;

        evInterval->m_beginTupleIndex = i;
        for (j = 0; tuple->m_seqNo == 0 && tuple->m_x == abscissa; j++)
            tuple = &tupleArray[++i];
        evInterval->m_beginLength = j;

        evInterval->m_vertTupleIndex = i;
        for (j = 0; tuple->m_seqNo == 1 && tuple->m_x == abscissa; j++)
            tuple = &tupleArray[++i];
        evInterval->m_vertLength = j;

        evInterval->m_endTupleIndex = i;
        for (j = 0; tuple->m_seqNo == 2 && tuple->m_x == abscissa; j++) {
            if (++i >= nTuples)
                break;
            tuple = &tupleArray[i];
        }
        evInterval->m_endLength = j;
    }

    // perform a sweep over the event intervals to derive the MIN vector
    // proposed in Meyer's paper - the purpose of the MIN vector is to
    // provide O(1) expected time searches for the interval containing a
    // specified abscissa

    if (nEventIntervals > 0) {
        m_startAbscissa = (*this)[0].Abscissa();
        m_delta = ((*this)[nEventIntervals-1].Abscissa() -
            m_startAbscissa) / static_cast<double>(nEventIntervals);
    }

    m_minVector = new MinVector(nEventIntervals);

    for (i = 0, j = 0; i < nEventIntervals; i++) {
        int hashVal = Hash((*this)[i].Abscissa());
        assert(hashVal <= nEventIntervals);

        for (; j <= hashVal; j++)
            (*m_minVector)[j] = i;
    }

    assert(j >= nEventIntervals);
    (*m_minVector)[nEventIntervals] = nEventIntervals - 1;
    (*m_minVector)[nEventIntervals+1] = nEventIntervals - 1;

    MG_CATCH(L"EventIntervalArray.EventIntervalArray")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        Cleanup();
    }

    MG_THROW()
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

EventIntervalArray::~EventIntervalArray()
{
    Cleanup();

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Get a reference to the specified EventInterval object.
//
// PARAMETERS:
//
//     Input:
//
//         evIntervalIndex - passes the index of the event interval.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a reference to the specified event interval object.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

const EventInterval &EventIntervalArray::operator[](int evIntervalIndex) const
{
    // check for valid index in debug mode

    assert(evIntervalIndex >= 0);
    assert(evIntervalIndex < m_allocator.GetNObjects());

    return *m_allocator[evIntervalIndex];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: GetNEventIntervals().
//
// PURPOSE: Get the number of event intervals stored in the EventIntervalArray
//          object.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of event interval object stored in the array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int EventIntervalArray::GetNEventIntervals() const
{
    return m_allocator.GetNObjects();

} // end: GetNEventIntervals()


//------------------------------------------------------------------------------
//
// METHOD: GetContainingInterval().
//
// PURPOSE: Get the index of the interval containing the specified x coordinate.
//
// PARAMETERS:
//
//     Input:
//
//         x - passes the x-value the containing interval is to be determined
//             for.
//
//     Output:
//
//         None.
//
// RETURNS: The index of the interval containing the specified x-value.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int EventIntervalArray::GetContainingInterval(double x) const
{
    int hashVal = Hash(x);
    int low = (*m_minVector)[hashVal];
    int high = (*m_minVector)[hashVal+1];

    while (low < high) {
        int middle = (low + high) / 2;

        if (x <= (*this)[middle].Abscissa())
            high = middle;
        else
            low = middle + 1;
    }

    assert(low == 0 || x > (*this)[low-1].Abscissa());
    assert(x <= (*this)[low].Abscissa());

    return low > 0 ? low - 1 : low;

} // end: GetContainingInterval()


//------------------------------------------------------------------------------
//
// METHOD: Cleanup().
//
// PURPOSE: Internal method to release all dynamically allocated resources.
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

void EventIntervalArray::Cleanup()
{
    delete m_minVector;

} // end: Cleanup()


//------------------------------------------------------------------------------
//
// Implementatation of EventIntervalArray::MinVector.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a EventIntervalArray::MinVector object.
//
// PARAMETERS:
//
//     Input:
//
//         nEventIntervals - passes the number of event intervals. The MIN
//                           vector is sized to nEventIntervals + 2.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

EventIntervalArray::MinVector::MinVector(int nEventIntervals) :
    m_vectorSize(nEventIntervals + 2),
    m_vector(new int[m_vectorSize])
{
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

EventIntervalArray::MinVector::~MinVector()
{
    delete [] m_vector;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Overloaded subscripting operator to access an element in the MIN
//          vector.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index into the MIN vector.
//
//     Output:
//
//         None.
//
// RETURNS: A reference to the index integer element of the MIN vector.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int &EventIntervalArray::MinVector::operator[](int index) const
{
    // check for valid index in debug mode

    assert(index >= 0);
    assert(index < m_vectorSize);

    return m_vector[index];

} // end: operator[]()
