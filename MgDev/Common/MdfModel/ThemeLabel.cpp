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
// The ThemeLabel class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ThemeLabel.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the ThemeLabel class.
//-------------------------------------------------------------------------
ThemeLabel::ThemeLabel()
{
    // default values
    // NOTE: values used in IOThemeLabel::Write must match these
//  this->m_sDescription    = L""; // NOXLATE
//  this->m_sCategoryFormat = L""; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ThemeLabel::GetDescription() const
{
    return this->m_sDescription;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ThemeLabel::SetDescription(const MdfString& description)
{
    this->m_sDescription = description;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ThemeLabel::GetCategoryFormat() const
{
    return this->m_sCategoryFormat;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ThemeLabel::SetCategoryFormat(const MdfString& categoryFormat)
{
    this->m_sCategoryFormat = categoryFormat;
}
