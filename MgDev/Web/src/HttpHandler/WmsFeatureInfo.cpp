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


MgWmsFeatureInfo::MgWmsFeatureInfo(MgXmlParser& input)
:   m_xmlInput(input)
,   m_bOk(false)

{
    // Set parsing options
    input.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    // Move to the first element, if one exists
    while(input.Next() && input.Current().Type() == keBeginElement)
    {
        // Is it a FeatureInformation element?
        MgXmlBeginElement& begin = (MgXmlBeginElement&)input.Current();
        if(begin.Name() == _("FeatureInformation"))
        {
            // Move to the first entry in the feature information
            m_bOk = input.Next();
            break;
        }

        // If it isn't a FeatureInformation element, skip it.
        SkipElement(begin.Name().c_str());
    }
}

bool MgWmsFeatureInfo::Next()
{
    if(m_bOk)
    {
        // Set to false until we find a property element
        m_bOk = false;
        while(!m_xmlInput.AtEnd() && m_xmlInput.Current().Type() == keBeginElement)
        {
            MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlInput.Current();
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
        if(m_xmlInput.Current().Type() == keBeginElement)
        {
            MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlInput.Current();
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

    if(pszElementName == NULL && m_xmlInput.Current().Type() == keBeginElement)
    {
        MgXmlBeginElement& begin = (MgXmlBeginElement&)m_xmlInput.Current();
        sName = begin.Name();
        pszElementName = sName.c_str();
    }

    MgXmlSynchronizeOnElement whatever(m_xmlInput,pszElementName);

    return whatever.AtBegin();
}
