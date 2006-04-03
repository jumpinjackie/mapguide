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

#include "OgcFramework.h"
#include "WmsFeatureInfo.h"


MgWmsFeatureInfo::MgWmsFeatureInfo(CPSZ inputXML)
:   m_bOk(false)

{
    // Create a new XML parser
    m_xmlParser = new MgXmlParser(inputXML);
    
    // Set parsing options
    m_xmlParser->SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    // Move to the first element, if one exists
    while(m_xmlParser->Next() && m_xmlParser->Current().Type() == keBeginElement)
    {
        // Is it a FeatureInformation element?
        MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlParser->Current();
        if(begin.Name() == _("FeatureInformation"))
        {
            // Move to the first entry in the feature information
            m_bOk = m_xmlParser->Next();
            break;
        }

        // If it isn't a FeatureInformation element, skip it.
        SkipElement(begin.Name().c_str());
    }
}

MgWmsFeatureInfo::~MgWmsFeatureInfo()
{
    if(m_xmlParser != NULL)
    {
        delete m_xmlParser;
        m_xmlParser = NULL;
    }
}

bool MgWmsFeatureInfo::Next()
{
    if(m_bOk)
    {
        // Set to false until we find a property element
        m_bOk = false;
        while(!m_xmlParser->AtEnd() && m_xmlParser->Current().Type() == keBeginElement)
        {
            MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlParser->Current();
            if(begin.Name() == _("Property"))
            {
                m_bOk = true;
                break;
            }

            // If it isn't a property element, skip it
            SkipElement(begin.Name().c_str());
        }
    }

    return m_bOk;
}

void MgWmsFeatureInfo::GenerateDefinitions(MgUtilDictionary& dictionary)
{
    if(m_bOk)
    {
        if(m_xmlParser->Current().Type() == keBeginElement)
        {
            MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlParser->Current();
            if(begin.HasAttributes())
            {
                STRING name;
                STRING value;
                m_bOk = begin.GetAttribute(_("name"), name);
                if(m_bOk)
                {
                    m_bOk = begin.GetAttribute(_("value"), value);
                    if(m_bOk)
                    {
                        m_bOk = dictionary.AddDefinition(_("FeatureProperty.Name"), name.c_str());
                        if(m_bOk)
                        {
                            m_bOk = dictionary.AddDefinition(_("FeatureProperty.Value"), value.c_str());
                        }
                    }
                }
            }
            SkipElement(begin.Name().c_str());
        }
    }
}

bool MgWmsFeatureInfo::SkipElement(CPSZ pszElementName)
{
    STRING sName;

    if(pszElementName == NULL && m_xmlParser->Current().Type() == keBeginElement)
    {
        MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlParser->Current();
        sName = begin.Name();
        pszElementName = sName.c_str();
    }

    MgXmlSynchronizeOnElement whatever(*m_xmlParser,pszElementName);

    return whatever.AtBegin();
}
