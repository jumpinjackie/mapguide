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
// FILE: ppgunion.h.
//
// PURPOSE: Declaration of the OrientedPolyPolygonUnion class. The OrientedPoly-
//          PolygonUnion class is used to derive the union of a set of Orient-
//          PolyPolygon's, producing the union as another OrientedPolyPolygon.
//
//------------------------------------------------------------------------------

#ifndef _PPGUNION_H_
#define _PPGUNION_H_

class OrientedPolyPolygonUnion : private PlaneSweep {
public:
    /// constructor/destructor

    OrientedPolyPolygonUnion(FloatTransform *transform);
    virtual ~OrientedPolyPolygonUnion();

    /// method to create the union of a list of oriented poly-polygons

    /// TODO: VKG
    void CreateUnion(const std::vector<OrientedPolyPolygon*> &inputPolyPolygons,
        ProgressCallback &callback, OrientedPolyPolygon &unionPolyPolygon);

protected:
    /// override of method to determine whether or not to accept a polygon
    /// boundary as part of the output

    virtual BOOL AcceptBoundary(const OpsFloatExtent &boundaryExt,
        Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
        const OpsDoublePoint &interiorPt) const;

private:
    /// derived version of an OpsRTree used to speed up boundary containment
    /// tests

    class OrientedPolyPolygonRTree : public OpsRTree {
    public:
        BOOL BoundaryContained(const OpsFloatExtent &boundaryExt,
            const OpsDoublePoint &interiorPt, BOOL strictContainment);
    private:
    /// search predicate object used by BoundaryContained(), to test whether
    /// an item extent contains a boundary extent

        class ExtContainsExt : public OpsRTree::SearchPredicate {
        public:
            void Initialize(const OpsFloatExtent &boundaryExt,
                BOOL strictContainment);
            virtual BOOL Predicate(const OpsFloatExtent &itemExtent) const;

        private:
            OpsFloatExtent m_boundaryExt;
            BOOL m_strictContainment;
        };

        ExtContainsExt m_extContainsExt;
    };

    OrientedPolyPolygonRTree *m_polyPolygonRTree;
    BOOL m_unionAlreadyGenerated;

    void AddPolyPolygonEdges(const OrientedPolyPolygon *polyPolygon);
};

#endif
