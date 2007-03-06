//
//  Copyright (C) 2007 by Autodesk, Inc.
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
// The Image class implementation.
//-------------------------------------------------------------------------

#include "Image.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Image class.
//-------------------------------------------------------------------------
Image::Image()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Image::~Image()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetContent() const
{
    return this->m_sContent;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetContent(const MdfString& content)
{
    this->m_sContent = content;

    // Content and Reference are mutually exclusive.  Setting one
    // clears the other.
    this->m_sReference.clear();
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetReference() const
{
    return this->m_sReference;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetReference(const MdfString& reference)
{
    this->m_sReference = reference;

    // Content and Reference are mutually exclusive.  Setting one
    // clears the other.
    this->m_sContent.clear();
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetSizeX() const
{
    return this->m_sSizeX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetSizeX(const MdfString& sizeX)
{
    this->m_sSizeX = sizeX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetSizeY() const
{
    return this->m_sSizeY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetSizeY(const MdfString& sizeY)
{
    this->m_sSizeY = sizeY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetPositionX() const
{
    return this->m_sPositionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetPositionX(const MdfString& positionX)
{
    this->m_sPositionX = positionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetPositionY() const
{
    return this->m_sPositionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetPositionY(const MdfString& positionY)
{
    this->m_sPositionY = positionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Image::GetAngle() const
{
    return this->m_sAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::SetAngle(const MdfString& angle)
{
    this->m_sAngle = angle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Image::AcceptVisitor(IGraphicElementVisitor& igeVisitor)
{
    igeVisitor.VisitImage(*this);
}
