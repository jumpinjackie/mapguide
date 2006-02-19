//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
}

CGwsCSQueryProcessor::~CGwsCSQueryProcessor()
{
}

void CGwsCSQueryProcessor::ProcessSpatialCondition(FdoSpatialCondition& sc)
{
    if (IGWSException::IsError(m_status) ||
        m_converter == NULL ||
        m_converter->SourceCS ().IsEmpty () ||
        m_converter->DestinationCS ().IsEmpty ())
    {
        //we stop processing if error happened earlier or converter is not set
        return;
    }

    //at this point either tesselation or cs conversion has to happen (not NULL)
    //so we have to extract the geometry
    GisPtr<FdoExpression> pGeometry = sc.GetGeometry();
    GisPtr<GisByteArray> pBa = static_cast<FdoGeometryValue*>((FdoExpression*)pGeometry)->GetGeometry();

    //first we have to apply tesselation if it is necessary
    if (! pBa) {
        return;
    }

    m_status=m_converter->ConvertBackward (pBa);

    //at the end either the error happen or the cs conversion/tesselation
    //actually happened, so we have to set the geometry back
    if (!IGWSException::IsError (m_status))
    {
        static_cast<FdoGeometryValue*>((FdoExpression*)pGeometry)->SetGeometry(pBa);
    }
}

EGwsStatus CGwsCSQueryProcessor::Status() const
{
    return m_status;
}

void CGwsCSQueryProcessor::ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::ProcessComparisonCondition(FdoComparisonCondition& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::ProcessInCondition(FdoInCondition& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::ProcessNullCondition(FdoNullCondition& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::ProcessDistanceCondition(FdoDistanceCondition& filter)
{
    filter; // For "unreferenced formal parameter" warning
}

void CGwsCSQueryProcessor::Dispose()
{
    delete this;
}
