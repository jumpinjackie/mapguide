//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsCSQueryProcessor::CGwsCSQueryProcessor(IGWSCoordinateSystemConverter *pConverter)
{
    m_status = eGwsOk;
    m_converter = pConverter;
    if (pConverter != NULL)
        pConverter->AddRef ();
    m_bAlignPolygonFilter = false;
}


CGwsCSQueryProcessor::~CGwsCSQueryProcessor()
{
}


void CGwsCSQueryProcessor::ProcessSpatialCondition(FdoSpatialCondition& sc)
{
    ProcessGeometry<FdoSpatialCondition> (sc);
}


EGwsStatus CGwsCSQueryProcessor::Status() const
{
    return m_status;
}


void CGwsCSQueryProcessor::ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> lop = filter.GetLeftOperand ();
    lop->Process (this);

    FdoPtr<FdoFilter> rop = filter.GetRightOperand ();
    rop->Process (this);
}


void CGwsCSQueryProcessor::ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> op = filter.GetOperand ();
    op->Process (this);
}


void CGwsCSQueryProcessor::ProcessComparisonCondition(FdoComparisonCondition& filter)
{
}


void CGwsCSQueryProcessor::ProcessInCondition(FdoInCondition& filter)
{
}


void CGwsCSQueryProcessor::ProcessNullCondition(FdoNullCondition& filter)
{
}


void CGwsCSQueryProcessor::ProcessDistanceCondition(FdoDistanceCondition& sc)
{
    ProcessGeometry<FdoDistanceCondition> (sc);
}


void CGwsCSQueryProcessor::Dispose()
{
    delete this;
}
