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

#ifndef SUPPLEMENTALSPATIALCONTEXTINFO_H_
#define SUPPLEMENTALSPATIALCONTEXTINFO_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //  Class SupplementalSpatialContextInfo represents additional information
    //  for a spatial context when the coordinate system is missing from the
    //  feature source.
    // ------------------------------------------------------------------------
class MDFMODEL_API SupplementalSpatialContextInfo : public MdfRootObject
    {
    public:
        //Constructors,
        //default copy constructor, destructor and assignment operator.
        SupplementalSpatialContextInfo(const MdfString& strName , const MdfString& strCoordinateSystem);
        SupplementalSpatialContextInfo();

        // Operations
        // Property : Name
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        // Property : CoordinateSystem
        const MdfString& GetCoordinateSystem() const;
        void SetCoordinateSystem(const MdfString& strCoordinateSystem);

    private:
        // The Name data member
        MdfString m_strName;

        // The CoordinateSystem associated with the name
        MdfString m_strCoordinateSystem;
    };

    typedef MdfOwnerCollection<SupplementalSpatialContextInfo> SupplementalSpatialContextInfoCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<SupplementalSpatialContextInfo>;

END_NAMESPACE_MDFMODEL
#endif//SUPPLEMENTALSPATIALCONTEXTINFO_H_
