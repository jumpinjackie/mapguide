//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
// DESCRIPTION:
// The SimpleSymbolDefinition class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "SimpleSymbolDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the SimpleSymbolDefinition class.
//-------------------------------------------------------------------------
SimpleSymbolDefinition::SimpleSymbolDefinition()
{
    this->m_pResizeBox = NULL;
    this->m_pPointUsage = NULL;
    this->m_pLineUsage = NULL;
    this->m_pAreaUsage = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
SimpleSymbolDefinition::~SimpleSymbolDefinition()
{
    delete this->m_pResizeBox;
    delete this->m_pPointUsage;
    delete this->m_pLineUsage;
    delete this->m_pAreaUsage;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
GraphicElementCollection* SimpleSymbolDefinition::GetGraphics()
{
    return &this->m_collGraphics;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
ResizeBox* SimpleSymbolDefinition::GetResizeBox()
{
    return this->m_pResizeBox;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbolDefinition::AdoptResizeBox(ResizeBox* resizeBox)
{
    if (this->m_pResizeBox != resizeBox)
    {
        delete this->m_pResizeBox;
        this->m_pResizeBox = resizeBox;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
ResizeBox* SimpleSymbolDefinition::OrphanResizeBox()
{
    ResizeBox* pRet = this->m_pResizeBox;
    this->m_pResizeBox = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
PointUsage* SimpleSymbolDefinition::GetPointUsage()
{
    return this->m_pPointUsage;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbolDefinition::AdoptPointUsage(PointUsage* pointUsage)
{
    if (this->m_pPointUsage != pointUsage)
    {
        delete this->m_pPointUsage;
        this->m_pPointUsage = pointUsage;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
PointUsage* SimpleSymbolDefinition::OrphanPointUsage()
{
    PointUsage* pRet = this->m_pPointUsage;
    this->m_pPointUsage = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
LineUsage* SimpleSymbolDefinition::GetLineUsage()
{
    return this->m_pLineUsage;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbolDefinition::AdoptLineUsage(LineUsage* lineUsage)
{
    if (this->m_pLineUsage != lineUsage)
    {
        delete this->m_pLineUsage;
        this->m_pLineUsage = lineUsage;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
LineUsage* SimpleSymbolDefinition::OrphanLineUsage()
{
    LineUsage* pRet = this->m_pLineUsage;
    this->m_pLineUsage = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
AreaUsage* SimpleSymbolDefinition::GetAreaUsage()
{
    return this->m_pAreaUsage;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbolDefinition::AdoptAreaUsage(AreaUsage* areaUsage)
{
    if (this->m_pAreaUsage != areaUsage)
    {
        delete this->m_pAreaUsage;
        this->m_pAreaUsage = areaUsage;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
AreaUsage* SimpleSymbolDefinition::OrphanAreaUsage()
{
    AreaUsage* pRet = this->m_pAreaUsage;
    this->m_pAreaUsage = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
ParameterCollection* SimpleSymbolDefinition::GetParameterDefinition()
{
    return &this->m_collParameters;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbolDefinition::AcceptVisitor(ISymbolDefinitionVisitor& isdVisitor)
{
    isdVisitor.VisitSimpleSymbolDefinition(*this);
}
