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

#include "ServerMappingServiceDefs.h"
#include "QueryRenderer.h"
#include "RSBufferOutputStream.h"

QueryRenderer::QueryRenderer(int width, int height, MgFeatureService* svcFeature)
: m_width(width),
  m_height(height),
  m_extents(0,0,1,1),
  m_dstResId(NULL),
  m_fclass(NULL),
  m_numFeatures(0),
  m_feature(NULL)
{
    m_svcFeature = SAFE_ADDREF(svcFeature);
}

QueryRenderer::~QueryRenderer()
{
    SAFE_RELEASE(m_fclass);
    SAFE_RELEASE(m_svcFeature);
    SAFE_RELEASE(m_dstResId);
}


void QueryRenderer::InitializeFeatureContext(MgFeatureReader* reader, MgResourceIdentifier* dstFeatures, RS_Units dstUnits)
{
    //create a feature source for the destination data,
    //based on the feature class of the source data
    Ptr<MgClassDefinition> fcsrc = reader->GetClassDefinition();

    if (m_fclass)
        SAFE_RELEASE(m_fclass);

    m_fclass = new MgClassDefinition();

    if (m_dstResId)
        SAFE_RELEASE(m_dstResId);

    m_dstResId = SAFE_ADDREF(dstFeatures);

    m_units = dstUnits;

    m_fclass->SetName(fcsrc->GetName());
    m_fclass->SetDescription(fcsrc->GetDescription() + L" Feature Query");
    m_fclass->SetDefaultGeometryPropertyName(fcsrc->GetDefaultGeometryPropertyName());

    Ptr<MgPropertyDefinitionCollection> srcProps = fcsrc->GetProperties();
    Ptr<MgPropertyDefinitionCollection> dstProps = m_fclass->GetProperties();

    //TODO: is it safe to share property objects like that?
    //I hope so, otherwise we will need to clone property by property
    for (int i=0; i<srcProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> prop = srcProps->GetItem(i);
        dstProps->Add(prop);
    }

    Ptr<MgPropertyDefinitionCollection> srcIdProps = fcsrc->GetIdentityProperties();
    Ptr<MgPropertyDefinitionCollection> dstIdProps = m_fclass->GetIdentityProperties();

    //TODO: is it safe to share property objects like that?
    //I hope so, otherwise we will need to clone property by property
    for (int i=0; i<srcIdProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> prop = srcIdProps->GetItem(i);
        dstIdProps->Add(prop);
    }

    Ptr<MgFeatureSchema> schema = new MgFeatureSchema(L"BufferSchema", L"Temporary buffer schema");
    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
    classes->Add(m_fclass);


    Ptr<MgCreateSdfParams> params = (MgCreateSdfParams*)NULL;

    if (m_units == RS_Units_Device)
        params = new MgCreateSdfParams(L"", L"", schema); //coord sys is in pixels
    else
        params = new MgCreateSdfParams(L"", m_coordSys, schema); //use coord sys of map

    //create the SDF file
    m_svcFeature->CreateFeatureSource(dstFeatures, params);
}


void QueryRenderer::StartMap      (RS_MapUIInfo* mapInfo,
                    RS_Bounds&    extents,
                    double        mapScale,
                    double        dpi,
                    double        metersPerUnit,
                    CSysTransformer* xformToLL)
{
    m_extents = extents;
    m_mapScale = mapScale;
    m_dpi = dpi;
    m_metersPerUnit = metersPerUnit;
    m_coordSys = mapInfo->coordsys();


    //find scale used to convert to pixel coordinates
    //need to take aspect ratios into account

    double arDisplay = (double)m_width / (double)m_height;
    double arMap = m_extents.width() / m_extents.height();

    if (arDisplay > arMap)
    {
        m_scale = (double)m_height / m_extents.height();

    //    //compute margin required on the left and right
    //    double pxwidth = arMap * (double)m_height;
    //    double margin = 0.5 * (m_width - pxwidth);
    }
    else
    {
        m_scale = (double)(m_width) / m_extents.width();

    //    //compute margin required on top and bottom
    //    double pxheight = arMap / (double)m_width;
    //    double margin = 0.5 * (m_height - pxheight);
    }

    m_offsetX = m_extents.minx;
    m_offsetY = m_extents.miny;


    double metersPerPixel = 0.0254 / m_dpi;

    //compute drawing scale
    //drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;
}



void QueryRenderer::EndMap           ()
{
}

void QueryRenderer::StartLayer     (RS_LayerUIInfo*      legendInfo,
                                 RS_FeatureClassInfo* classInfo
                                )
{
}

void QueryRenderer::EndLayer       ()
{
}

void QueryRenderer::StartFeature (RS_FeatureReader* feature, const RS_String*  tooltip,
                                  const RS_String*  url)
{
    m_feature = feature;
}



void QueryRenderer::ProcessPolygon (LineBuffer*       lb,
                                    RS_FillStyle&     fill)
{
    //convert geometry
    Ptr<MgByteReader> geom = GetGeometry(lb);

    //transfer attributes
    TransferFeature(m_feature, geom);
}

void QueryRenderer::ProcessPolyline(LineBuffer* lb,
                                 RS_LineStroke& lsym)
{
    //convert geometry
    Ptr<MgByteReader> geom = GetGeometry(lb);

    //transfer attributes
    TransferFeature(m_feature, geom);

}

void QueryRenderer::ProcessRaster  (unsigned char* data,
                                    int length,
                                    RS_ImageFormat format,
                                    int width, int height,
                                    RS_Bounds extents)
{
    //create a bounding polygon for rasters

    LineBuffer* lb = new LineBuffer(8);
    lb->SetGeometryType(GisGeometryType_Polygon);

    lb->MoveTo(extents.minx, extents.miny);
    lb->LineTo(extents.maxx, extents.miny);
    lb->LineTo(extents.maxx, extents.maxy);
    lb->LineTo(extents.minx, extents.maxy);
    lb->Close();


    //convert geometry
    Ptr<MgByteReader> geom = GetGeometry(lb);

    //transfer attributes
    //TransferFeature(feature, geom);

}

void QueryRenderer::ProcessMarker( LineBuffer*       lb,
                                   RS_MarkerDef&     mdef,
                                   bool              allowOverpost)
{

    //
    //TODO define a rectangle describing the symbol
    //Since sending back a point makes no sense for image map purposes
    //
    Ptr<MgByteReader> geom = GetGeometry(lb);

    //transfer attributes
    TransferFeature(m_feature, geom);
}


void QueryRenderer::ProcessLabel(double x, double y,
                                 const RS_String& text, RS_TextDef& tdef)
{
}

void QueryRenderer::ProcessLabelGroup(  RS_LabelInfo*    labels,
                                        int              nlabels,
                                        const RS_String& text,
                                        RS_OverpostType  type,
                                        bool             exclude,
                                        LineBuffer*      path)
{
}

void QueryRenderer::AddDWFContent(RS_InputStream*  in,
                                  CSysTransformer* xformer,
                                  const RS_String& section,
                                  const RS_String& passwd
                               )
{
}

void QueryRenderer::SetSymbolManager(RS_SymbolManager* manager)
{
}


double QueryRenderer::GetMapScale()
{
    return m_mapScale;
}

RS_Bounds& QueryRenderer::GetBounds()
{
    return m_extents;
}

double QueryRenderer::GetDpi()
{
    return m_dpi;
}

double QueryRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}

bool QueryRenderer::RequiresClipping()
{
    return true; //doesn't matter in this case
}

bool QueryRenderer::UseLocalOverposting()
{
    return false; //not applicable
}

void QueryRenderer::TransferFeature(RS_FeatureReader* src, MgByteReader* geom)
{
    Ptr<MgPropertyCollection> props = new MgPropertyCollection();

    Ptr<MgGeometryProperty> geomp = new MgGeometryProperty(m_fclass->GetDefaultGeometryPropertyName(), geom);
    props->Add(geomp);

    Ptr<MgPropertyDefinitionCollection> propdefs = m_fclass->GetProperties();

    for (int i=0; i<propdefs->GetCount(); i++)
    {
        Ptr<MgProperty> p = propdefs->GetItem(i);
        MgPropertyDefinition* pd = dynamic_cast<MgPropertyDefinition*>(p.p);

        if (pd)
        {
            INT16 pt = pd->GetPropertyType();

            if (pt == MgFeaturePropertyType::DataProperty)
            {
                MgDataPropertyDefinition* dpd = (MgDataPropertyDefinition*)(pd);

                if (dpd->IsAutoGenerated()) continue;

                INT16 dt = dpd->GetDataType();

                switch (dt)
                {
                case MgPropertyType::Blob:
                    break;
                case MgPropertyType::Boolean:
                    {
                        Ptr<MgBooleanProperty> bp = new MgBooleanProperty(pd->GetName(), src->GetBoolean(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Byte:
                    {
                        Ptr<MgByteProperty> bp = new MgByteProperty(pd->GetName(), src->GetByte(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Clob:
                    break;
                case MgPropertyType::DateTime:
                    break;
                case MgPropertyType::Double:
                    {
                        Ptr<MgDoubleProperty> bp = new MgDoubleProperty(pd->GetName(), src->GetDouble(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Feature:
                    break;
                case MgPropertyType::Geometry:
                    break;
                case MgPropertyType::Int16:
                    {
                        Ptr<MgInt16Property> bp = new MgInt16Property(pd->GetName(), src->GetInt16(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Int32:
                    {
                        Ptr<MgInt32Property> bp = new MgInt32Property(pd->GetName(), src->GetInt32(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Int64:
                    {
                        Ptr<MgInt64Property> bp = new MgInt64Property(pd->GetName(), src->GetInt64(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::Raster:
                    break;
                case MgPropertyType::Single:
                    {
                        Ptr<MgSingleProperty> bp = new MgSingleProperty(pd->GetName(), src->GetSingle(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                case MgPropertyType::String:
                    {
                        Ptr<MgStringProperty> bp = new MgStringProperty(pd->GetName(), src->GetString(pd->GetName().c_str()));
                        props->Add(bp);
                    }
                    break;
                default : break;
                }
            }
        }

    }

    //do the insert command into the destination feature source

    Ptr<MgInsertFeatures> insert = new MgInsertFeatures(m_fclass->GetName(), props);

    Ptr<MgFeatureCommandCollection> cmdColl = new MgFeatureCommandCollection();
    cmdColl->Add(insert);

    m_svcFeature->UpdateFeatures(m_dstResId, cmdColl, false);

    m_numFeatures++;
}


MgByteReader* QueryRenderer::GetGeometry(LineBuffer* srclb)
{
    //do we need device space?
    if (m_units == RS_Units_Device)
        _TransformPointsInPlace(srclb);

    RSBufferOutputStream os(8);
    srclb->ToAgf(&os);
    Ptr<MgByteSource> src = new MgByteSource(os.data(), (INT32)os.length());
    return src->GetReader(); //caller must release
}

//////////////////////////////////////////////////////////////////////
//
// Map to Screen transform
//

//transforms an x coordinate from mapping to screen space
inline double QueryRenderer::_TX(double x)
{
    if (x > m_extents.maxx)
        x = m_extents.maxx;
    else if (x < m_extents.minx)
        x = m_extents.minx;

    return (x - m_offsetX) * m_scale;
}

//transforms a y coordinate from mapping to screen space
inline double QueryRenderer::_TY(double y)
{
    if (y > m_extents.maxy)
        y = m_extents.maxy;
    else if (y < m_extents.miny)
        y = m_extents.miny;

    return (y - m_offsetY) * m_scale;
}


//transform a LineBuffer's points from mapping space to
//device space in place
//THIS MODIFIES THE LineBuffer DESTRUCTIVELY!
void QueryRenderer::_TransformPointsInPlace(LineBuffer* lb)
{
    double * pts = lb->points();

    for (int i=0; i<lb->point_count(); i++)
    {
        *pts = (int)_TX(*pts); //cast to pixel coordinate, then back to double
        pts++;
        *pts = (int)_TY(*pts); //cast to pixel coordinate, then back to double
        pts++;
    }
}


int QueryRenderer::GetNumFeaturesProcessed()
{
    return m_numFeatures;
}
