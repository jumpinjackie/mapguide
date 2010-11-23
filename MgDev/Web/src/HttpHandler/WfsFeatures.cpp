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

#include "OgcFramework.h"
#include "WfsFeatures.h"


CPSZ gszElementNameFeatureMember = _("http://www.opengis.net/gml:featureMember");
CPSZ gszElementNameFeatureCollection = _("http://www.opengis.net/wfs:FeatureCollection");

MgWfsFeatures::MgWfsFeatures(CPSZ szInputXml,int iMaxFeatures)
:   m_sFeatureCollection(szInputXml)
,   m_XmlInput(m_sFeatureCollection.c_str())
,   m_iMaxFeatures(iMaxFeatures)
,   m_bOk(true)
,   m_bDone(false)
{
    if(m_iMaxFeatures < 1)
        m_iMaxFeatures = 1000000; // a million features max should be enough.
    m_XmlInput.Next();
}

MgWfsFeatures::~MgWfsFeatures()
{
}

bool MgWfsFeatures::Next()
{
    // Blitz anything that's there.
    m_sCurrentFeature = _("");

    if(m_bOk && m_iMaxFeatures--) {
        while(true) {
            // End of stream?
            if(m_XmlInput.AtEnd()) {
                m_bOk = false;
                break; // while
            }

           // Okay; a good sign... let's start poking around.
            switch(m_XmlInput.Current().Type()) {
            case keBeginElement:
                {
                    MgXmlBeginElement& Begin = (MgXmlBeginElement&)m_XmlInput.Current();
                    m_Namespaces.TrackBeginElement(Begin);

                    // If it's a <featureMember> element...
                    if(m_Namespaces.QualifiedName(Begin) == gszElementNameFeatureMember) {
                        m_sCurrentFeature = Begin.Contents();
                        m_iCurrentInnerContent = m_sCurrentFeature.length();

                        MgXmlSynchronizeOnNamespaceElement SlurpThis(m_XmlInput,
                                                                     gszElementNameFeatureMember,
                                                                     m_Namespaces);
                        // Declare our intent to go into the element.
                        SlurpThis.AtBegin();
                        // Now slurp out the contents.
                        while(!SlurpThis.AtEnd()) {
                            m_sCurrentFeature += m_XmlInput.Current().Contents();
                            m_XmlInput.Next();
                        }
                        // Make a note of how much has been added.
                        m_iCurrentInnerLength = m_sCurrentFeature.length() - m_iCurrentInnerContent;
                        // We're currently parked over the </featureMember> element; tack that on.
                        m_sCurrentFeature += m_XmlInput.Current().Contents();
                        // Track the end element's going.
                        m_Namespaces.TrackEndElement((MgXmlEndElement&)m_XmlInput.Current());
                        // And tell the caller we've got a hit.
                        return m_bOk;
                    }
                    else if(m_Namespaces.QualifiedName(Begin) == gszElementNameFeatureCollection){
                        MgXmlAttribute& attributes = Begin.Attributes();
                        m_sFeatureCollectionNamespaces = attributes.Contents();

                        m_iMaxFeatures++; // Read schema namespaces should not count the feature

                        m_XmlInput.Next();
                        return m_bOk;
                    }
                    else {
                        m_XmlInput.Next();
                    }
                }
                break;

            case keEndElement:
                {
                    // The </featureMember> element is swallowed above,
                    // This processes only non-featureMember elements, as is
                    // the case when querying for two or more feature classes.
                    MgXmlEndElement& End = (MgXmlEndElement&)m_XmlInput.Current();
                    m_Namespaces.TrackEndElement(End);
                    m_XmlInput.Next();
                }
                break;

            default:
                // Unexpected and uninteresting (<?xml?>, white space, comments, etc.); skip it.
                m_XmlInput.Next();
            }
        }
    }
    else
        m_bOk = false;

    return m_bOk;
}

void MgWfsFeatures::GenerateDefinitions(MgUtilDictionary& Dictionary)
{
    if(m_bOk && !m_sCurrentFeature.empty()) {
        Dictionary.AddDefinition(L"Feature.OuterXml",    m_sCurrentFeature);
        Dictionary.AddDefinition(L"Feature.InnerXml",    m_sCurrentFeature.substr(m_iCurrentInnerContent,m_iCurrentInnerLength));
        Dictionary.AddDefinition(L"Feature.EndElement",  m_sCurrentFeature.substr(m_iCurrentInnerContent+m_iCurrentInnerLength));
        Dictionary.AddDefinition(L"Feature.BeginElement",m_sCurrentFeature.substr(0,m_iCurrentInnerContent));
    }
}

bool MgWfsFeatures::GenerateNamespacesDefinition(MgUtilDictionary& Dictionary)
{
    if(m_bOk && !m_sFeatureCollectionNamespaces.empty()) {
        Dictionary.AddDefinition(L"FeatureCollection.Namespaces", m_sFeatureCollectionNamespaces);
        return true;
    }

    return false;
}

