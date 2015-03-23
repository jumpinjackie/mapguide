//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#ifndef TILESTOREPARAMETERS_H_
#define TILESTOREPARAMETERS_H_

#include "MdfModel.h"
#include "NameStringPair.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    class MDFMODEL_API TileStoreParameters : public MdfRootObject
    {
    public:
        TileStoreParameters();
        virtual ~TileStoreParameters();

        // Operations
        // Property : Parameters
        NameStringPairCollection* GetParameters();

        // Property : TileProvider
        const MdfString& GetTileProvider() const;
        void SetTileProvider(const MdfString &provider);

    private:

        // Hidden copy constructor and assignment operator.
        TileStoreParameters(const TileStoreParameters&);
        TileStoreParameters& operator=(const TileStoreParameters&);

        int m_width;
        int m_height;
        MdfString m_provider;
        MdfString m_format;
        NameStringPairCollection m_parameters;
    };

END_NAMESPACE_MDFMODEL

#endif //TILESTOREPARAMETERS_H_