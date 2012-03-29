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

#ifndef _MgWfsFeatures_h
#define _MgWfsFeatures_h

#include "XmlParser.h"
#include "Dictionary.h"

class MgWfsFeatures: public IOgcResourceEnumerator, public MgDisposable
{
public:
    MgWfsFeatures(CPSZ inputXml,int iMaxFeatures);
    //Default constructor to keep Ptr<> happy
    MgWfsFeatures();
    virtual ~MgWfsFeatures();

    bool Next();
    void GenerateDefinitions(MgUtilDictionary& Dictionary);
    bool GenerateNamespacesDefinition(MgUtilDictionary& Dictionary);
    virtual void Dispose()
    {
        delete this;
    }

private:
    STRING m_sFeatureCollection;  // the entire blob of features
    MgXmlNamespaceManager m_Namespaces;
    MgXmlParser m_XmlInput;
    STRING m_sCurrentFeature;                 // just the current feature.
    STRING m_sFeatureCollectionNamespaces;    // the namespaces in <wfs:FeatureCollection>
    STRING::size_type m_iCurrentInnerContent; // index to start of content after <gml:featureMember>
    STRING::size_type m_iCurrentInnerLength;  // length of content between <gml:featureMember> and </gml:featureMember>
    bool m_bOk;
    bool m_bDone;
    int m_iMaxFeatures;
};

#endif//_MgWfsFeatures_h

