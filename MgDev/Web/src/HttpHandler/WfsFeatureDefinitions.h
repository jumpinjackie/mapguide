//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef _MgWfsFeatureDefinitions_h
#define _MgWfsFeatureDefinitions_h

#include "XmlParser.h"
#include "Dictionary.h"

class MgWfsFeatureDefinitions: public IOgcResourceEnumerator
{
public:
    MgWfsFeatureDefinitions(MgResourceService* pResourceService,MgFeatureService* pFeatureService);
    ~MgWfsFeatureDefinitions();

    bool Next();

    // Used by the enumerator to create dictionary entries
    // for the features in question.
    void GenerateDefinitions(MgUtilDictionary& Dictionary);

    // Does the resource contain the feature named?
    bool HasFeature(CPSZ pszFeatureName);

    // Request that the otherwise complete enumeration be restricted to this subset.
    // Returns true if successful; false indicates a mismatch (like asking for a
    // feature that isn't part of the complete set.)
    // String is a comma-separated list of feature names.
    bool SubsetFeatureList(CPSZ pszFeatureNames);

private:
    bool   SkipElement(MgXmlParser& Input,CPSZ pszElementName);
    bool   GetElementContents(MgXmlParser& Input,CPSZ pszElementName,STRING& sValue);
    bool   GetMetadataDefinitions(MgXmlParser& Input,CStream& oStream);

    void   AddDefinition(CStream& oStream,CPSZ pszPropertyName,CPSZ pszPropertyValue);

    bool   AddSubset(CPSZ pszTypeName);
    bool   IsWantedSubset(CPSZ pszTypeName);

    void   Initialize();

    MgXmlParser*       m_pXmlInput;
    MgResourceService* m_pResourceService;
    MgFeatureService*  m_pFeatureService;

    STRING m_sSourcesAndClasses;
    STRING m_sSubsetOfTypes;
    bool m_bOk;
};

#endif//_MgWfsFeatureDefinitions_h
