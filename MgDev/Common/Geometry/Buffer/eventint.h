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

//-----------------------------------------------------------------------------
//
// FILE: eventint.h.
//
// PURPOSE: Declaration of the EventInterval class. The EventInterval class
//          is used to store the requisite information about each event
//          interval.
//
//-----------------------------------------------------------------------------

#ifndef _EVENTINT_H_
#define _EVENTINT_H_

class SweepTupleArray;

class EventInterval {
public:
    double Abscissa() const;
    int BeginLength() const;
    SweepEdge *Begin(int i) const;
    int VertLength() const;
    SweepEdge *Vert(int i) const;
    int EndLength() const;
    SweepEdge *End(int i) const;

private:
    const SweepTupleArray *m_tupleArray;
    int m_beginLength;
    int m_beginTupleIndex;
    int m_vertLength;
    int m_vertTupleIndex;
    int m_endLength;
    int m_endTupleIndex;
    double m_abscissa;

    friend class EventIntervalArray;
};


//------------------------------------------------------------------------------
//
// METHOD: Abscissa().
//
// PURPOSE: Get the start abscissa of the interval.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The starting abscissa (x-value) of the event interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline double EventInterval::Abscissa() const
{
    return m_abscissa;

} // end: Abscissa()


//------------------------------------------------------------------------------
//
// METHOD: BeginLength().
//
// PURPOSE: Get the number of SweepEdges that begin in the event interval.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of edges that begin in the event interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int EventInterval::BeginLength() const
{
    return m_beginLength;

} // end: BeginLength()


//------------------------------------------------------------------------------
//
// METHOD: Begin().
//
// PURPOSE: Get a pointer to the i-th SweepEdge that begins in the event
//          interval.
//
// PARAMETERS:
//
//     Input:
//
//         i - passes the index of the SweepEdge to get.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the i-th SweepEdge that begins in the interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline SweepEdge *EventInterval::Begin(int i) const
{
    assert(i < m_beginLength);

    return (*m_tupleArray)[m_beginTupleIndex+i].m_edge;

} // end: Begin()


//------------------------------------------------------------------------------
//
// METHOD: VertLength().
//
// PURPOSE: Get the number of vertical SweepEdges in the event interval.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of vertical edges in the event interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int EventInterval::VertLength() const
{
    return m_vertLength;

} // end: VertLength()


//------------------------------------------------------------------------------
//
// METHOD: Vert().
//
// PURPOSE: Get a pointer to the i-th vertical SweepEdge in the event interval.
//
// PARAMETERS:
//
//     Input:
//
//         i - passes the index of the SweepEdge to get.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the i-th vertical SweepEdge in the interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline SweepEdge *EventInterval::Vert(int i) const
{
    assert(i < m_vertLength);

    return (*m_tupleArray)[m_vertTupleIndex+i].m_edge;

} // end: Vert()


//------------------------------------------------------------------------------
//
// METHOD: EndLength().
//
// PURPOSE: Get the number of SweepEdges that end in the event interval.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of edges that end in the event interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int EventInterval::EndLength() const
{
    return m_endLength;

} // end: EndLength()


//------------------------------------------------------------------------------
//
// METHOD: End().
//
// PURPOSE: Get a pointer to the i-th SweepEdge that ends in the event interval.
//
// PARAMETERS:
//
//     Input:
//
//         i - passes the index of the SweepEdge to get.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the i-th SweepEdge that ends in the interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline SweepEdge *EventInterval::End(int i) const
{
    assert(i < m_endLength);

    return (*m_tupleArray)[m_endTupleIndex+i].m_edge;

} // end: End()

#endif
