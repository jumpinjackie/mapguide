//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "MgCSTrans.h"


MgCSTrans::MgCSTrans(MgCoordinateSystem* srcCs, MgCoordinateSystem* dstCs)
{
    assert(srcCs);
    assert(dstCs);
    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
    m_trans = csFactory->GetTransform(srcCs, dstCs);
    m_trans->IgnoreDatumShiftWarning(true);
    m_trans->IgnoreOutsideDomainWarning(true);

    m_dLinearScale = srcCs->ConvertCoordinateSystemUnitsToMeters(1.0) / dstCs->ConvertCoordinateSystemUnitsToMeters(1.0);
}


MgCSTrans::~MgCSTrans()
{
}


void MgCSTrans::TransformPoint(double& x, double& y)
{
    m_trans->Transform(&x, &y);
}


void MgCSTrans::TransformPoints(int numPts, double* x, double* y)
{
    m_trans->Transform(x, y, numPts);
}


void MgCSTrans::TransformExtent(double& minX, double& minY, double& maxX, double& maxY)
{
    MgEnvelope extent(minX, minY, maxX, maxY);
    Ptr<MgEnvelope> transExtent = m_trans->Transform(&extent);
    Ptr<MgCoordinate> ll = transExtent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = transExtent->GetUpperRightCoordinate();

    minX = ll->GetX();
    minY = ll->GetY();
    maxX = ur->GetX();
    maxY = ur->GetY();
}


double MgCSTrans::GetLinearScale()
{
    return m_dLinearScale;
}
