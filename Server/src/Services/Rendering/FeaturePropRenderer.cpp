//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "MapGuideCommon.h"
#include "FeaturePropRenderer.h"
#include "RS_FeatureReader.h"


FeaturePropRenderer::FeaturePropRenderer(MgSelection* selection, int maxFeatures, double mapScale)
: FeatureInfoRenderer(selection, maxFeatures, mapScale)
{
    m_featprops = new MgBatchPropertyCollection();
}


FeaturePropRenderer::~FeaturePropRenderer()
{
    SAFE_RELEASE(m_featprops);
}


void FeaturePropRenderer::StartFeature(RS_FeatureReader* feature,
                                       bool /*initialPass*/,
                                       const RS_String* tooltip,
                                       const RS_String* url,
                                       const RS_String* theme,
                                       double /*zOffset*/,
                                       double /*zExtrusion*/,
                                       RS_ElevationType /*zOffsetType*/)
{
    /*
    //add feature ID to the selection set
    if (m_selection)
    {
        //generate base 64 id
        const char* base64 = m_keyEncode->EncodeKey(feature);
        size_t len = base64? strlen(base64) : 0;

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
    */

    Ptr<MgPropertyCollection> featureProps = new MgPropertyCollection(true, true);

    //Add the layer name as a property with a special ID
    Ptr<MgStringProperty> layerNameProperty = new MgStringProperty(L"_MgLayerName", m_layerInfo->name());
    featureProps->Add(layerNameProperty);

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

        featureProps->Add(sp);
    }

    //add to the output collection of features
    m_featprops->Add(featureProps);

    m_numFeatures++;
}


bool FeaturePropRenderer::SupportsTooltips()
{
    // set to false to disable processing of tooltips
    return false;
}


bool FeaturePropRenderer::SupportsHyperlinks()
{
    // set to false to disable processing of hyperlinks
    return false;
}
