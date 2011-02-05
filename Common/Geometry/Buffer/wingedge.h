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

/*------------------------------------------------------------------------------
//
// FILE: wingedge.h.
//
// PURPOSE: Declaration of the WingedEdge class. The WingedEdge class is an
//          implementation of the modified winged-edge structure as proposed by
//          K. Wieler (SIGGRAPH 80). This is the fundamental  data structure
//          used in the plane sweep algorithm. The basic objective of the sweep
//          algorithm is to connect the winged edges into a graph (processing
//          midpoint intersections by splitting edges, and merging coincident
//          edges). Once the graph has been constructed, the boundaries of the
//          resulting polygons can be extracted by traversing closed loops in
//          the graph. As stipulated in Meyers plane sweep intersection algor-
//          ithm, the following is assumed:
//
//          vert[0]->x < vert[1]->x ||
//          (vert[0]->x == vert[1]->x && vert[0]->y < vert[1]->y)
//
//          Pictorially, the winged-edge stucture can viewed as follows:
//
//
//                       ccwEdge[1]          / cwEdge[0]
//                                          /
//                                         /
//                                        /
//                                       /
//                   vert[1] (Max Vert)  +
//                                       |
//                                       |
//                       MaxVertSide     |     MinVertSide
//                                       |
//                                       |
//                                       + vert[0] (Min Vert)
//                                      /
//                                     /
//                                    /
//                                   /
//                        cwEdge[1] /          ccwEdge[0]
//
//------------------------------------------------------------------------------*/

#ifndef _WINGEDGE_H_
#define _WINGEDGE_H_

class WingedEdge {
public:
    WingedEdge();

    void Initialize(OpsDoublePoint *vertices[2]);

    enum EdgeSide {MinVertSide = 0, MaxVertSide = 1};

    EdgeSide GetSide(const OpsDoublePoint *vert) const;
    EdgeSide GetSide(const WingedEdge *wEdge) const;

    void SetSideIn(EdgeSide side);
    BOOL SideIn(EdgeSide side) const;

    BOOL SideVisited(EdgeSide side) const;
    void SetSideVisited(EdgeSide side);

    void SetEdgeDeleted();
    BOOL EdgeDeleted() const;

    void ResetWings(WingedEdge *oldEdge, WingedEdge *newEdge);
    void Join(WingedEdge *edge, const OpsDoublePoint *endPt);
    BOOL EdgesJoined(const WingedEdge *edge) const;

    OpsDoublePoint *m_vert[2];
    WingedEdge *m_ccwEdge[2];
    WingedEdge *m_cwEdge[2];

    long m_id;

private:
    enum EdgeFlags {MinVertSideIn = 0x01,
                    MaxVertSideIn = 0x02,
                    MinVertSideVisited = 0x04,
                    MaxVertSideVisited = 0x08,
                    EdgeIsDeleted = 0x10};
    short m_flags;
};


//------------------------------------------------------------------------------
//
// METHOD: GetSide().
//
// PURPOSE: Determine which edge-side of a WingedEdge is associated with the
//          specified vertex.
//
// PARAMETERS:
//
//     Input:
//
//         vertex - passes pointer to the vertex.
//
//     Output:
//
//         None.
//
// RETURNS: Returns MinVertSide or MaxVertSide depending on which edge-side
//          the vertex is associated with.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline WingedEdge::EdgeSide WingedEdge::GetSide(const OpsDoublePoint *vertex) const
{
    assert(*m_vert[0] == *vertex || *m_vert[1] == *vertex);

    return *m_vert[0] == *vertex ? MinVertSide : MaxVertSide;

} // end: GetSide()


//------------------------------------------------------------------------------
//
// METHOD: GetSide().
//
// PURPOSE: Determine which edge-side is associated with a specified linked
//          WingedEdge.
//
// PARAMETERS:
//
//     Input:
//
//         wEdge - passes a pointer to the linked WingedEdge.
//
//     Output:
//
//         None.
//
// RETURNS: Returns MinVertSide or MaxVertSide depending on which edge-side
//          the linked edge is associated with.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline WingedEdge::EdgeSide WingedEdge::GetSide(const WingedEdge *wEdge) const
{
    if (m_ccwEdge[0] == wEdge || m_cwEdge[0] == wEdge)
        return MinVertSide;
    else {
        assert(m_ccwEdge[1] == wEdge || m_cwEdge[1] == wEdge);
        return MaxVertSide;
    }

} // end: GetSide()


//------------------------------------------------------------------------------
//
// METHOD: SideVisited().
//
// PURPOSE: Determine if the specified edge-side has been visited in the
//          traversal of the graph of WingedEdges.
//
// PARAMETERS:
//
//     Input:
//
//         side - passes an enumerated value of type EdgeSide specifying
//                which edge-side is to be checked.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the specified edge-side has been marked as visited, and
//          FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL WingedEdge::SideVisited(EdgeSide side) const
{
    if (side == MinVertSide)
        return m_flags & MinVertSideVisited;
    else
        return m_flags & MaxVertSideVisited;

} // end: SideVisited()


//------------------------------------------------------------------------------
//
// METHOD: SetSideVisited().
//
// PURPOSE: Mark the specified edge-side as being interior to the polygon
//          bounded the edge.
//
// PARAMETERS:
//
//     Input:
//
//         side - passes an enumerated value of type EdgeSide specifying
//                which edge-side is to be marked as interior.
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

inline void WingedEdge::SetSideIn(EdgeSide side)
{
    if (side == MinVertSide)
        m_flags |= MinVertSideIn;
    else
        m_flags |= MaxVertSideIn;

} // end: SetSideIn()


//------------------------------------------------------------------------------
//
// METHOD: SideIn().
//
// PURPOSE: Determine if the specified edge-side of the WingedEdge is interior
//          or not.
//
// PARAMETERS:
//
//     Input:
//
//         side - passes an enumerated value of type EdgeSide specifying
//                which edge-side to check.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the specified edge-side is marked as interior, and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL WingedEdge::SideIn(EdgeSide side) const
{
    if (side == MinVertSide)
        return m_flags & MinVertSideIn;
    else
        return m_flags & MaxVertSideIn;

} // end: SideIn()


//------------------------------------------------------------------------------
//
// METHOD: SetSideVisited().
//
// PURPOSE: Mark the specified edge-side of the WingedEdge as having been
//          visited.
//
// PARAMETERS:
//
//     Input:
//
//         side - passes an enumerated value of type EdgeSide specifying
//                which edge-side to mark as visited.
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

inline void WingedEdge::SetSideVisited(EdgeSide side)
{
    if (side == MinVertSide)
        m_flags |= MinVertSideVisited;
    else
        m_flags |= MaxVertSideVisited;

} // end: SetSideVisited()


//------------------------------------------------------------------------------
//
// METHOD: SetEdgeDeleted().
//
// PURPOSE: Mark the edge as deleted from the graph of WingedEdges.
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

inline void WingedEdge::SetEdgeDeleted()
{
    m_flags |= EdgeIsDeleted;

} // end: SetEdgeDeleted()


//------------------------------------------------------------------------------
//
// METHOD: EdgeDeleted().
//
// PURPOSE: Determine if the edge has been marked as deleted.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if the WingedEdge has been marked as deleted and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL WingedEdge::EdgeDeleted() const
{
    return m_flags & EdgeIsDeleted;

} // end: EdgeDeleted()


//------------------------------------------------------------------------------
//
// METHOD: ResetWings().
//
// PURPOSE: Reset all wing pointers that previously pointed to 'oldEdge' to
//          point to 'newEdge'.
//
// PARAMETERS:
//
//     Input:
//
//         oldEdge - passes a pointer to the old WingedEdge.
//         newEdge - passes a pointer to the new WingedEdge.
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

inline void WingedEdge::ResetWings(WingedEdge *oldEdge, WingedEdge *newEdge)
{
    if (m_ccwEdge[0] == oldEdge)
        m_ccwEdge[0] = newEdge;
    if (m_ccwEdge[1] == oldEdge)
        m_ccwEdge[1] = newEdge;

    if (m_cwEdge[0] == oldEdge)
        m_cwEdge[0] = newEdge;
    if (m_cwEdge[1] == oldEdge)
        m_cwEdge[1] = newEdge;

} // end: ResetWings()


//------------------------------------------------------------------------------
//
// METHOD: Join().
//
// PURPOSE: Join this WingedEdge with the specified WingedEdge at the indicated
//          endpoint.
//
// PARAMETERS:
//
//     Input:
//
//         edge  - passes a pointer to the WingedEdge that is to be joined to
//                 this edge.
//         endPt - passes a pointer to the end point (vertex) shared by both
//                 the edges.
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

inline void WingedEdge::Join(WingedEdge *edge, const OpsDoublePoint *endPt)
{
    if (*endPt == *m_vert[0])
        m_ccwEdge[0] = m_cwEdge[1] = edge;
    else
        m_ccwEdge[1] = m_cwEdge[0] = edge;

    if (*endPt == *edge->m_vert[0])
        edge->m_ccwEdge[0] = edge->m_cwEdge[1] = this;
    else
        edge->m_ccwEdge[1] = edge->m_cwEdge[0] = this;

} // end: Join()


//------------------------------------------------------------------------------
//
// METHOD: EdgeJoined().
//
// PURPOSE: Determine if the this WingedEdge and the specified WingedEdge are
//          joined.
//
// PARAMETERS:
//
//     Input:
//         edge - passes a pointer to the edge to test.
//
//     Output:
//
// RETURNS: TRUE if the edges are joined to one another and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL WingedEdge::EdgesJoined(const WingedEdge *edge) const
{
    return m_ccwEdge[0] == edge || m_ccwEdge[1] == edge ||
           m_cwEdge[0] == edge  || m_cwEdge[1] == edge;

} // end: EdgesJoined()

#endif
