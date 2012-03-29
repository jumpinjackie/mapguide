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

//-------------------------------------------------------------------------
// DESCRIPTION:
// Version Class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Version.h"
#include <sstream>

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Version class.
//-------------------------------------------------------------------------
Version::Version()
{
    this->SetMajor(0);
    this->SetMinor(0);
    this->SetRevision(0);
}

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Version class.
//-------------------------------------------------------------------------
Version::Version(int nMajorNumber, int nMinorNumber, int nRevisionNumber)
{
    this->SetMajor(nMajorNumber);
    this->SetMinor(nMinorNumber);
    this->SetRevision(nRevisionNumber);
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
Version::~Version()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Major number.
// RETURNS: the Major number .
//-------------------------------------------------------------------------
int Version::GetMajor() const
{
    return this->m_nMajorNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Major number.
// PARAMETERS:
//      Input:
//          nMajorNumber - the Major number.
//-------------------------------------------------------------------------
void Version::SetMajor(const int& nMajorNumber)
{
    this->m_nMajorNumber= nMajorNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Minor number.
// RETURNS: the Minor number .
//-------------------------------------------------------------------------
int Version::GetMinor() const
{
    return this->m_nMinorNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Minor number.
// PARAMETERS:
//      Input:
//          nMinorNumber - the Minor number.
//-------------------------------------------------------------------------
void Version::SetMinor(const int& nMinorNumber)
{
    this->m_nMinorNumber= nMinorNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Revision number.
// RETURNS: the Revision number .
//-------------------------------------------------------------------------
int Version::GetRevision() const
{
    return this->m_nRevisionNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Revision number.
// PARAMETERS:
//      Input:
//          nRevisionNumber - the Revision number.
//-------------------------------------------------------------------------
void Version::SetRevision(const int& nRevisionNumber)
{
    this->m_nRevisionNumber= nRevisionNumber;
}

//-------------------------------------------------------------------------
// PURPOSE: Return a string representation of the version.  The format
//                 is Major.Minor.Revision (ie 1.0.2).
// RETURNS: the string representation of of the version.
//-------------------------------------------------------------------------
MdfString Version::ToString()
{
    MdfString strVersion;

    std::wstringstream ss;
    MdfString strMajor;
    MdfString strMinor;
    MdfString strRevision;
    MdfString strDot = L"."; // NOXLATE

    ss << this->m_nMajorNumber;
    ss >> strMajor;

    ss.clear();

    ss << this->m_nMinorNumber;
    ss >> strMinor;

    ss.clear();

    ss << this->m_nRevisionNumber;
    ss >> strRevision;

    strVersion = strMajor + strDot + strMinor + strDot + strRevision;

    return strVersion;
}

// checks if this version equals the supplied version
bool Version::operator==(const Version& version) const
{
    return m_nMajorNumber    == version.m_nMajorNumber &&
           m_nMinorNumber    == version.m_nMinorNumber &&
           m_nRevisionNumber == version.m_nRevisionNumber;
}

// checks if this version does not equal the supplied version
bool Version::operator!=(const Version& version) const
{
    return !(*this == version);
}

// checks if this version is greater than the supplied version
bool Version::operator>(const Version& version) const
{
    if (m_nMajorNumber == version.m_nMajorNumber)
    {
        if (m_nMinorNumber == version.m_nMinorNumber)
            return m_nRevisionNumber > version.m_nRevisionNumber;
        else
            return m_nMinorNumber > version.m_nMinorNumber;
    }
    else
        return m_nMajorNumber > version.m_nMajorNumber;
}

// checks if this version is less than the supplied version
bool Version::operator<(const Version& version) const
{
    return !(*this >= version);
}

// checks if this version is greater than or equal to the supplied version
bool Version::operator>=(const Version& version) const
{
    if (m_nMajorNumber == version.m_nMajorNumber)
    {
        if (m_nMinorNumber == version.m_nMinorNumber)
            return m_nRevisionNumber >= version.m_nRevisionNumber;
        else
            return m_nMinorNumber > version.m_nMinorNumber;
    }
    else
        return m_nMajorNumber > version.m_nMajorNumber;
}

// checks if this version is less than or equal to the supplied version
bool Version::operator<=(const Version& version) const
{
    return !(*this > version);
}
