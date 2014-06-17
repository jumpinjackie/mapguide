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

#include "ServerRenderingDllExport.h"
#include "ServerMappingDllExport.h"
#include "MapGuideCommon.h"
#include "FeaturePropRenderer.h"
#include "RS_FeatureReader.h"
#include "SE_SymbolDefProxies.h"

#define SPECIAL_PROP_LAYER_NAME     L"_MgLayerName"
#define SPECIAL_PROP_BOUNDING_BOX   L"_MgFeatureBoundingBox"

FeaturePropRenderer::FeaturePropRenderer(MgSelection* selection, int maxFeatures, double mapScale, bool bIncludeFeatureBBOX)
: FeatureInfoRenderer(selection, maxFeatures, mapScale)
{
    m_featprops = new MgBatchPropertyCollection();
    m_currentFeature = NULL;
    m_bIncludeFeatureBBOX = bIncludeFeatureBBOX;
}


FeaturePropRenderer::~FeaturePropRenderer()
{
    SAFE_RELEASE(m_featprops);
    SAFE_RELEASE(m_currentFeature);
}


void FeaturePropRenderer::StartFeature(RS_FeatureReader* feature,
                                       bool initialPass,
                                       const RS_String* tooltip,
                                       const RS_String* url,
                                       const RS_String* theme,
                                       double /*zOffset*/,
                                       double /*zExtrusion*/,
                                       RS_ElevationType /*zOffsetType*/)
{
    if (!initialPass)
        return;

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

    SAFE_RELEASE(m_currentFeature);
    Ptr<MgPropertyCollection> featureProps = new MgPropertyCollection(true, true);
    m_currentFeature = SAFE_ADDREF(featureProps.p);

    //Add the layer name as a property with a special ID
    Ptr<MgStringProperty> layerNameProperty = new MgStringProperty(SPECIAL_PROP_LAYER_NAME, m_layerInfo->name());
    featureProps->Add(layerNameProperty);

    //Add bounding box if we're instructed to
    if (m_bIncludeFeatureBBOX)
    {
        Ptr<MgStringProperty> boundingBoxProperty = new MgStringProperty(SPECIAL_PROP_BOUNDING_BOX, L"");
        featureProps->Add(boundingBoxProperty);
    }

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

void FeaturePropRenderer::GetGeometryBounds(const LineBuffer* lb, RS_Bounds& bounds)
{
    const RS_Bounds& featBounds = lb->bounds();
    
    bounds.minx = featBounds.minx;
    bounds.miny = featBounds.miny;
    bounds.maxx = featBounds.maxx;
    bounds.maxy = featBounds.maxy;

    //printf("Current bounds: [%f, %f, %f, %f]\n", bounds.minx, bounds.miny, bounds.maxx, bounds.maxy);

    if (m_currentTransform)
    {
        double minx = bounds.minx;
        double miny = bounds.miny;
        double maxx = bounds.maxx;
        double maxy = bounds.maxy;

        m_currentTransform->TransformExtent(minx, miny, maxx, maxy);
        
        bounds.minx = minx;
        bounds.miny = miny;
        bounds.maxx = maxx;
        bounds.maxy = maxy;

        //printf("Transformed bounds: [%f, %f, %f, %f]\n", bounds.minx, bounds.miny, bounds.maxx, bounds.maxy);
    }
}

void FeaturePropRenderer::SetBBOXProperty(const RS_Bounds& bounds, MgStringProperty* bbox)
{
    STRING val;
    STRING buf;
    MgUtil::DoubleToString(bounds.minx, buf);
    val += buf;
    MgUtil::DoubleToString(bounds.miny, buf);
    val += L" ";
    val += buf;
    MgUtil::DoubleToString(bounds.maxx, buf);
    val += L" ";
    val += buf;
    MgUtil::DoubleToString(bounds.maxy, buf);
    val += L" ";
    val += buf;
    bbox->SetValue(val);
}

void FeaturePropRenderer::ProcessPolygon(LineBuffer*   lb,
                                         RS_FillStyle& fill)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(lb, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}

void FeaturePropRenderer::ProcessPolyline(LineBuffer*    lb,
                                          RS_LineStroke& lsym)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(lb, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}

void FeaturePropRenderer::ProcessMarker(LineBuffer*   lb,
                                        RS_MarkerDef& mdef,
                                        bool          allowOverpost,
                                        RS_Bounds*    bounds)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    //Should we inflate this a bit to represent an actual box?
    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(lb, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}

void FeaturePropRenderer::ProcessPoint(SE_ApplyContext* ctx,
                                       SE_RenderPointStyle* style,
                                       RS_Bounds* bounds)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    //Should we inflate this a bit to represent an actual box?
    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(ctx->geometry, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}

void FeaturePropRenderer::ProcessLine(SE_ApplyContext* ctx,
                                      SE_RenderLineStyle* style)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    //Should we inflate this a bit to represent an actual box?
    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(ctx->geometry, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}

void FeaturePropRenderer::ProcessArea(SE_ApplyContext* ctx,
                                      SE_RenderAreaStyle* style)
{
    if (!m_bIncludeFeatureBBOX)
        return;

    //Should we inflate this a bit to represent an actual box?
    RS_Bounds fbounds(0.0, 0.0, 0.0, 0.0);
    GetGeometryBounds(ctx->geometry, fbounds);
    Ptr<MgStringProperty> bbox = dynamic_cast<MgStringProperty*>(m_currentFeature->GetItem(SPECIAL_PROP_BOUNDING_BOX));
    SetBBOXProperty(fbounds, bbox);
}
