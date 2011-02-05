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

#ifndef FEATURESOURCE_H_
#define FEATURESOURCE_H_

#include "MdfModel.h"
#include "NameStringPair.h"
#include "Extension.h"
#include "SupplementalSpatialContextInfo.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION:
// Feature Source class is the portal to all connection information
// needed to connect to a feature source.  It also contains extensions that
// contain joins for the feature classes in the feature source.
//-------------------------------------------------------------------------
class MDFMODEL_API FeatureSource : public MdfRootObject
    {
    public:
         // Construction, destruction, initialization
        FeatureSource();
        virtual ~FeatureSource();

        // Operations
        // Property : Parameters
        NameStringPairCollection* GetParameters();

        // Property : Extensions
        ExtensionCollection* GetExtensions();

        // Property : SupplementalSpatialContextInfo
        SupplementalSpatialContextInfoCollection* GetSupplementalSpatialContextInfo();

        // Property : Provider
        const MdfString& GetProvider() const;
        void SetProvider(const MdfString &provider);

        // Property : LongTransaction
        const MdfString& GetLongTransaction() const;
        void SetLongTransaction(const MdfString &configFile);

        // Property : ConfigurationDocument
        const MdfString& GetConfigurationDocument() const;
        void SetConfigurationDocument(const MdfString &configFile);

    private:
        // Hidden copy constructor and assignment operator.
        FeatureSource(const FeatureSource&);
        FeatureSource& operator=(const FeatureSource&);

        // Collection of key/value pairs
        NameStringPairCollection m_collParameters;

        // Collection of extensions
        ExtensionCollection m_collExtensions;

        // Collection of supplemental spatial context info
        SupplementalSpatialContextInfoCollection m_collSupplementalSpatialContextInfo;

        // Provider name
        MdfString m_strProvider;

        // The long transaction to use when connected to the provider
        MdfString m_strLongTransaction;

        // Configuration information needed for certain providers
        MdfString m_strConfigurationDocument;
    };

END_NAMESPACE_MDFMODEL
#endif // FEATURESOURCE_H_
