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

#ifndef VERSION_H_
#define VERSION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Represents a version object.
    // The format of the version is Major.Minor.Revision.
    // For example version 1.0.2,
    // the Major number is 1, Minor number is 0, Revision number is 2.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Version : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        Version();
        Version(int nMajorNumber, int nMinorNumber, int nRevisionNumber);
        virtual ~Version();

        // Operations
        // Property : Major
        int GetMajor() const;
        void SetMajor(const int& nMajorNumber);

        // Property : Minor
        int GetMinor() const;
        void SetMinor(const int& nMinorNumber);

        // Property : Revision
        int GetRevision() const;
        void SetRevision(const int& nRevisionNumber);

        bool operator==(const Version& version) const;
        bool operator!=(const Version& version) const;
        bool operator>(const Version& version) const;
        bool operator<(const Version& version) const;
        bool operator>=(const Version& version) const;
        bool operator<=(const Version& version) const;

        MdfString ToString();

    private:
        // Data members
        // The major number in the version
        int m_nMajorNumber;

        // The minor number in the version
        int m_nMinorNumber;

        // The revision number in the version
        int m_nRevisionNumber;
    };

END_NAMESPACE_MDFMODEL
#endif //VERSION_H_
