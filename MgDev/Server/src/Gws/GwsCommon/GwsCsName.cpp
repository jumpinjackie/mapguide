//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"

/////////////////////////////////////////////////////////////////////
//
// class GWSCoordinateSystem
//
/////////////////////////////////////////////////////////////////////
GWSCoordinateSystem::GWSCoordinateSystem ()
{
    m_cstype = eGwsCSWkt;
    m_csname = L"";
}
GWSCoordinateSystem::GWSCoordinateSystem (FdoString * wktcsname)
{
    m_cstype = eGwsCSWkt;
    m_csname = wktcsname;
}
GWSCoordinateSystem::GWSCoordinateSystem (
    FdoString * csname,
    EGwsCSType  cstype
)
{
    m_cstype = cstype;
    m_csname = csname;
}
GWSCoordinateSystem::GWSCoordinateSystem (const GWSCoordinateSystem & other)
{
    * this = other;
}
GWSCoordinateSystem::~GWSCoordinateSystem ()
{
}

FdoString* GWSCoordinateSystem::Name () const
{
    return m_csname;
}
EGwsCSType GWSCoordinateSystem::Type () const
{
    return m_cstype;
}

const GWSCoordinateSystem & GWSCoordinateSystem::operator= (const GWSCoordinateSystem & other)
{
    m_cstype = other.m_cstype;
    m_csname = other.m_csname;
    return * this;
}

bool GWSCoordinateSystem::operator== (const GWSCoordinateSystem & other) const
{
    return m_cstype == other.m_cstype &&
           m_csname == other.m_csname;
}

GWSCoordinateSystem::operator FdoString * () const
{
    return Name ();
}

bool GWSCoordinateSystem::IsEmpty () const
{
    return m_csname.GetLength () == 0;
}
