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

//-------------------------------------------------------------------------
//
// FILE: tuplearr.cpp.
//
// PURPOSE: Implementation of the SweepTupleArray object.
//
//-------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

static const int TupleBlockSize = 256;
static const int TupleStepsPerProgressSubInterval = 128;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an empty SweepTupleArray object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepTupleArray::SweepTupleArray() :
    m_allocator(TupleBlockSize)
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

SweepTupleArray::~SweepTupleArray()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: AddSweepTuples().
//
// PURPOSE: Generate the necessary Tuples corresponding to the specified Sweep-
//          Edge, and add them to the array of tuples.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the SweepEdge the tuples are to be
//                generated for.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             to add the generate and add the sweep tuples.
//
//------------------------------------------------------------------------------

void SweepTupleArray::AddSweepTuples(SweepEdge *edge)
{
    Tuple *tuple = m_allocator.GetNextObject();

    // if edge is vertical generate a single tuple

    if (edge->Vertical()) {
        tuple->m_edge = edge;
        tuple->m_x = edge->MinVert()->x;
        tuple->m_seqNo = 1;
        tuple->m_minusy = -edge->MinVert()->y;
    }

    // else generate two tuples

    else {
        tuple->m_edge = edge;
        tuple->m_x = edge->MinVert()->x;
        tuple->m_seqNo = 0;
        tuple->m_minusy = -edge->MinVert()->y;

        tuple = m_allocator.GetNextObject();
        tuple->m_edge = edge;
        tuple->m_x = edge->MaxVert()->x;
        tuple->m_seqNo = 2;
        tuple->m_minusy = -edge->MinVert()->y;
    }

} // end: AddSweepTuples()


//------------------------------------------------------------------------------
//
// METHOD: Sort().
//
// PURPOSE: Sort the tuples stored in the array on lexicographic order of x,
//          seqNo, and minusy. Either heapsort or quicksort can be chosen as
//          the sort method.
//
// PARAMETERS:
//
//     Input:
//
//         callback - passes a reference to a ProgressCallback object. The
//                    referenced callback object is periodically queried to
//                    determine whether or not to cancel processing, and to
//                    report progress.
//         method   - passes an enumerated value of type SortMethod specifying
//                    which algorithm to use to sort the tuples.
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

void SweepTupleArray::Sort(ProgressCallback &callback, SortMethod method)
{
    BufferSort<Tuple> sortTuples;
    sortTuples.Sort(method, m_allocator, TupleStepsPerProgressSubInterval, callback);

} // end: Sort()


//------------------------------------------------------------------------------
//
// METHOD: GetNSweepTuples().
//
// PURPOSE: Get the total number of Tuple objects stored in the sweep tuple
//          array.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of sweep tuples in the array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int SweepTupleArray::GetNSweepTuples() const
{
    return m_allocator.GetNObjects();

} // end: GetNSweepTuples()
