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
// FILE: sweepedg.h.
//
// PURPOSE: Declaration of the SweepEdge class. The SweepEdge class implements
//          the sweep edge data structure required in Meyer's plane sweep
//          algorithm. SweepEdges are maintained distinct from WingedEdges
//          since the former requires extra information such as the slope of
//          the edge.
//
//------------------------------------------------------------------------------

#ifndef _SWEEPEDG_H_
#define _SWEEPEDG_H_

class SweepEdge {
public:
    SweepEdge();

    void Initialize(WingedEdge *edge);

    double Ordinate(double x) const;
    int XOrder(SweepEdge *edge, double x) const;

    OpsDoublePoint *MinVert() const;
    OpsDoublePoint *MaxVert() const;

    WorkListArray::Node *GetWorkListNode() const;
    void SetWorkListNode(WorkListArray::Node *node);

    XOrderedTree::Node *GetXotNode() const;
    void SetXotNode(XOrderedTree::Node *node);

    WingedEdge *GetWingedEdge() const;

    BOOL Vertical() const;
    double Slope() const;

private:
    double m_slope;
    WingedEdge *m_wingedEdge;
    WorkListArray::Node *m_workListNode;
    XOrderedTree::Node *m_xotNode;
};


//------------------------------------------------------------------------------
//
// METHOD: Ordinate().
//
// PURPOSE: Compute the ordinate of the sweep edge at the specified abscissa.
//
// PARAMETERS:
//
//     Input:
//
//         abscissa - passes the x-value the ordinate is to be computed at.
//
//     Output:
//
//         None.
//
// RETURNS: The oridinate for the sweep edge at the specified x value.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline double SweepEdge::Ordinate(double x) const
{
    if (m_slope == HUGE_VAL || x == MinVert()->x)
        return MinVert()->y;
    else if (x == MaxVert()->x)
        return MaxVert()->y;
    else
        return m_slope * (x - MinVert()->x) + MinVert()->y;

} // end: Ordinate()


//------------------------------------------------------------------------------
//
// METHOD: XOrder().
//
// PURPOSE: Compute the X-ordering of two SweepEdges. The X-Order relation is as
//          defined in Meyer's plane sweep algorithm.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the SweepEdge the X-Order is to be
//                computed with respect to.
//         x    - passes the abscissa where the ordinates of the sweep edges
//                are to be computed.
//
//     Output:
//
//         None.
//
// RETURNS: Returns -1, 0, or 1 depending on whether this edge is less than,
//          equal to, or greater than edge according to the X-order.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int SweepEdge::XOrder(SweepEdge *edge, double x) const
{
    double ordinate1 = Ordinate(x);
    double ordinate2 = edge->Ordinate(x);

    if (ordinate1 < ordinate2)
        return -1;
    else if (ordinate1 > ordinate2)
        return 1;
    else if (m_slope < edge->m_slope)
        return -1;
    else if (m_slope > edge->m_slope)
        return 1;
    else
        return 0;

} // end: XOrder()


//------------------------------------------------------------------------------
//
// METHOD: GetWingedEdge().
//
// PURPOSE: Get the WingedEdge object associated with the SweepEdge.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the WingedEdge.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline WingedEdge *SweepEdge::GetWingedEdge() const
{
    return m_wingedEdge;

} // end: GetWingedEdge()


//------------------------------------------------------------------------------
//
// METHOD: MinVert().
//
// PURPOSE: Get the minimum vertex for the sweep edge.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the minimum vertex.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsDoublePoint *SweepEdge::MinVert() const
{
    return m_wingedEdge->m_vert[0];

} // end: MinVert()


//------------------------------------------------------------------------------
//
// METHOD: MaxVert().
//
// PURPOSE: Get the maximum vertex for the sweep edge.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the maximum vertex.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsDoublePoint *SweepEdge::MaxVert() const
{
     return m_wingedEdge->m_vert[1];

} // end: MaxVert()


//------------------------------------------------------------------------------
//
// METHOD: GetWorkListNode().
//
// PURPOSE: Get the WorkListArray::Node associated with the SweepEdge (if any).
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the WorkListArray::Node or NULL if there is none.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline WorkListArray::Node *SweepEdge::GetWorkListNode() const
{
    return m_workListNode;

} // end: GetWorkListNode()


//------------------------------------------------------------------------------
//
// METHOD: SetWorkListNode().
//
// PURPOSE: Set the WorkListArray::Node for the SweepEdge.
//
// PARAMETERS:
//
//     Input:
//
//         node - passes a pointer to the WorkListArray::Node.
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

inline void SweepEdge::SetWorkListNode(WorkListArray::Node *node)
{
    m_workListNode = node;

} // end: SetWorkListNode()


//------------------------------------------------------------------------------
//
// METHOD: GetXotNode().
//
// PURPOSE: Get the XOrderedTree::Node associated with the SweepEdge (if any).
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the XOrderedTree::Node or NULL if there is none.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline XOrderedTree::Node *SweepEdge::GetXotNode() const
{
    return m_xotNode;

} // end: GetXotNode()


//------------------------------------------------------------------------------
//
// METHOD: SetXotNode().
//
// PURPOSE: Set the XOrderedTree::Node for the SweepEdge.
//
// PARAMETERS:
//
//     Input:
//
//         node - passes a pointer to the XOrderedTree::Node.
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

inline void SweepEdge::SetXotNode(XOrderedTree::Node *node)
{
    m_xotNode = node;

} // end: SetXotNode()


//------------------------------------------------------------------------------
//
// METHOD: Vertical().
//
// PURPOSE: Determine if the SweepEdge is vertical.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if the SweepEdge is vertical and FALSE otherwise.
//
// EXCEPTIONS:
//
//------------------------------------------------------------------------------

inline BOOL SweepEdge::Vertical() const
{
    return MinVert()->x == m_wingedEdge->m_vert[1]->x;

} // end: Vertical()


//------------------------------------------------------------------------------
//
// METHOD: Slope().
//
// PURPOSE: Determine the slope of the SweepEdge.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The slope of the SweepEdge. Note that HUGE_VAL is returned if the
//          edge is vertical.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline double SweepEdge::Slope() const
{
    return m_slope;

} // end: Slope()

#endif
