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

#include "ServerRenderingDllExport.h"
#include "ServerMappingDllExport.h"
#include "Common.h"
#include "FeatureInfoRenderer.h"
#include "RSBufferOutputStream.h"
#include "KeyEncode.h"
#include "RSMgFeatureReader.h"


FeatureInfoRenderer::FeatureInfoRenderer(MgSelection* selection, int maxFeatures, double mapScale)
: m_extents(0,0,1,1),
  m_numFeatures(0),
  m_url(L""),
  m_tooltip(L""),
  m_props(NULL),
  m_layerId(L""),
  m_nMaxFeatures(maxFeatures),
  m_mapScale(mapScale),
  m_fcName(L""),
  m_fcInfo(NULL)
{
    m_selection = SAFE_ADDREF(selection);
    m_keyEncode = new KeyEncode();
}


FeatureInfoRenderer::~FeatureInfoRenderer()
{
    SAFE_RELEASE(m_props);
    SAFE_RELEASE(m_selection);

    delete m_keyEncode;
}


void FeatureInfoRenderer::StartFeature (RS_FeatureReader* feature, const RS_String* tooltip, const RS_String* url)
{
    //add feature ID to the selection set
    if (m_selection)
    {
        //generate base 64 id
        const unsigned char* base64 = m_keyEncode->EncodeKey(feature);
        size_t len = strlen((const char*)base64);

        //check how big those things can actually get
        assert(len < 65536);

        //only select if it has a key
        if (len > 0)
        {
            wchar_t* touni = (wchar_t*)alloca(sizeof(wchar_t) * (len+1));

            for (size_t k=0; k<len+1; k++)
                touni[k] = (wchar_t)base64[k];

            //add feature to the selection set
            m_selection->Add(m_layerId, m_fcName, (const wchar_t*)touni);
        }
    }

    //if it is the first/only feature, process the properties
    if (!m_props && m_fcInfo)
    {
        if (url)
            m_url = *url;

        if (tooltip)
            m_tooltip = *tooltip;

        m_props = new MgPropertyCollection();

        //for each property in the property mapping, add to the
        //return property collection

        //there are two strings in the list, corresponding to each property
        //the first is the actual name of the property and the second is
        //the display name of the property. Use the actual name to retrieve the
        //value and the display name for the name of the DWF property we store
        for (unsigned i=0; i<m_fcInfo->mappings().size()/2; i++)
        {
            Ptr<MgStringProperty> sp = new MgStringProperty(m_fcInfo->mappings()[2*i+1].c_str()
                , feature->GetAsString(m_fcInfo->mappings()[2*i].c_str()));

            m_props->Add(sp);
        }
    }

    m_numFeatures++;
}


void FeatureInfoRenderer::StartLayer(RS_LayerUIInfo*      legendInfo,
                                     RS_FeatureClassInfo* classInfo)
{
    m_fcInfo = classInfo;
    m_layerId = legendInfo->guid();
    m_fcName = classInfo->name();
}
