//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef URLDATA_H_
#define URLDATA_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The URLData class is used by a layer to provide URL information for each 
    // feature.
    //-------------------------------------------------------------------------
    class MDFMODEL_API URLData : public MdfRootObject
    {
        public:
            // Construction, destruction, initialization
            URLData();
            ~URLData();

            // Operations
            // Property : URL content
            const MdfString& GetUrlContent() const;
            void SetUrlContent(const MdfString &urlContent);

            // Property : URL content override
            const MdfString& GetUrlContentOverride() const;
            void SetUrlContentOverride(const MdfString &urlContentOverride);

            //Property : URL description
            const MdfString& GetUrlDescription() const;
            void SetUrlDescription(const MdfString &urlDescription);

            //Property : URL description override
            const MdfString& GetUrlDescriptionOverrride() const;
            void SetUrlDescriptionOverrride(const MdfString &urlDescriptionOverride);

        private:
            // Hidden copy constructor and assignment operator.
            URLData(const URLData&);
            URLData& operator = (const URLData&);

            //URL content
            MdfString m_strUrlContent;

            //URL content override
            MdfString m_strUrlContentOverride;

            //URL description
            MdfString m_strUrLDescription;

            //URL description override
            MdfString m_strUrlDescriptionOverride;
    };

END_NAMESPACE_MDFMODEL
#endif
