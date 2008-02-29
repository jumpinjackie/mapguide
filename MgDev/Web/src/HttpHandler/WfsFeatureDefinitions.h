//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
    MgWfsFeatureDefinitions(MgResourceService* pResourceService,MgFeatureService* pFeatureService,MgStringCollection* pFeatureTypes);
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

    // Convenience methods to map Feature Source <-> QName prefix.
    // * Given a Feature Source ("Library://...FeatureSource") string,
    //   creates the recommended prefix name.  Generally succeeds.
    bool FeatureSourceToPrefix(CREFSTRING sFeatureSource,REFSTRING sPrefix);
    // * Given a prefix, attempts to identify the corresponding feature source
    //   string; will fail if an invalid prefix is provided.  Should be used
    //   only if definitive feature source is not already known (via namespace)
    //   declaration (as might be the case for HTTP GET variants of GetFeature
    //   and DescribeFeatureType.)
    bool PrefixToFeatureSource(STRING sPrefix,REFSTRING sFeatureSource);

private:
    bool   SkipElement(MgXmlParser& Input,CPSZ pszElementName);
    bool   GetElementContents(MgXmlParser& Input,CPSZ pszElementName,STRING& sValue);
    bool   GetMetadataDefinitions(MgXmlParser& Input,CStream& oStream,bool& isPublished);

    void   AddDefinition(CStream& oStream,CPSZ pszPropertyName,CPSZ pszPropertyValue);

    bool   AddSubset(CPSZ pszTypeName);
    bool   IsWantedSubset(CPSZ pszTypeName);

    void   Initialize();

    MgXmlParser*       m_pXmlInput;
    MgResourceService* m_pResourceService;
    MgFeatureService*  m_pFeatureService;
    MgStringCollection* m_pFeatureTypes;

    STRING m_sSourcesAndClasses;
    STRING m_sSubsetOfTypes;
    bool m_bOk;


    // Used to generate a statistically "unique" integer
    // (shorter than a GUID) of the given string.
    // Used as a second line of defense for finding
    // a feature when the explicit namespace isn't provided.
    static unsigned StringHasher(CPSZ s);

};

#endif//_MgWfsFeatureDefinitions_h
