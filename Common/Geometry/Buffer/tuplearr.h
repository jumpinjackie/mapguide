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

//
// FILE: tuplearr.h.
//
// PURPOSE: Declaration of the SweepTupleArray class. The SweepTupleArray class
//          implements the sweep tuple array required in the plane sweep
//          algorithm. Sweep tuples are generated for each sweep edge, and then
//          these tuples are sorted as stipulated in Meyer's paper. The sorting
//          method may be specified to be quicksort (the default) or heapsort.
//
//------------------------------------------------------------------------------

#ifndef _TUPLEARR_H_
#define _TUPLEARR_H_

class SweepTupleArray {
public:
    /// constructor/destructor

    SweepTupleArray();
    virtual ~SweepTupleArray();

    /// declaration of the sweep Tuple structure

    struct Tuple {
        SweepEdge *m_edge;
        double m_x;
        int m_seqNo;
        double m_minusy;
        int operator<(const Tuple &) const;
    };

    /// methods to add, sort, and access the sweep tuples

    void AddSweepTuples(SweepEdge *edge);
    void Sort(ProgressCallback &callback, SortMethod method = QUICKSORT);
    Tuple &operator[](int index) const;
    int GetNSweepTuples() const;

private:
    BufferAllocator<Tuple> m_allocator;

    Tuple *GetNextFreeTuple();
    void GetNewBlock();
};


//------------------------------------------------------------------------------
//
// METHOD: SweepTupleArray::Tuple::operator<().
//
// PURPOSE: Overloaded operator to compare two sweep tuples under the less than
//          relation.
//
// PARAMETERS:
//
//     Input:
//
//         tuple - reference to the Tuple to compare with.
//
//     Output:
//
//         None.
//
// RETURNS: Returns 1 if *this < tuple, otherwise returns 0.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int SweepTupleArray::Tuple::operator<(const Tuple &tuple) const
{
    if (m_x < tuple.m_x)
        return 1;
    else if (m_x > tuple.m_x)
        return 0;
    else if (m_seqNo < tuple.m_seqNo)
        return 1;
    else if (m_seqNo > tuple.m_seqNo)
        return 0;
    else if (m_minusy < tuple.m_minusy)
        return 1;
    else
        return 0;

} // end: operator<()


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Operator used to return a reference to the indexed Tuple object.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the Tuple.
//
//     Output:
//
//         None.
//
// RETURNS: A reference to the indexed Tuple.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline SweepTupleArray::Tuple &SweepTupleArray::operator[](int index) const
{
    /// check for valid index in debug mode

    assert(index >= 0);
    assert(index < m_allocator.GetNObjects());

    return *m_allocator[index];

} // end: operator[]()

#endif
