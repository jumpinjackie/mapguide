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
// The TextFrame class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "TextFrame.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the TextFrame class.
//-------------------------------------------------------------------------
TextFrame::TextFrame()
{
    // default values
    // NOTE: values used in IOTextFrame::Write must match these
//  this->m_sLineColor = L"";    // NOXLATE
//  this->m_sFillColor = L"";    // NOXLATE
    this->m_sOffsetX   = L"0.0"; // NOXLATE
    this->m_sOffsetY   = L"0.0"; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
TextFrame::~TextFrame()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& TextFrame::GetLineColor() const
{
    return this->m_sLineColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void TextFrame::SetLineColor(const MdfString& color)
{
    this->m_sLineColor = color;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& TextFrame::GetFillColor() const
{
    return this->m_sFillColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void TextFrame::SetFillColor(const MdfString& color)
{
    this->m_sFillColor = color;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& TextFrame::GetOffsetX() const
{
    return this->m_sOffsetX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void TextFrame::SetOffsetX(const MdfString& offset)
{
    this->m_sOffsetX = offset;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& TextFrame::GetOffsetY() const
{
    return this->m_sOffsetY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void TextFrame::SetOffsetY(const MdfString& offset)
{
    this->m_sOffsetY = offset;
}
