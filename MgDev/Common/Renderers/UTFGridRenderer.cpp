#include "UTFGridRenderer.h"
#include "UTFGridContent.h"
#include "SLDSymbols.h"
#include "SymbolTrans.h"
#include "agg_utfgrid_context.h"

// =========================== UTFGridRenderer overview =============================== //
//
// At a high-level, you can think of UTFGridRenderer as a hybrid of the AGGRenderer and KmlRenderer
//
// * Like AGGRenderer, it writes to an AGG backend using mostly the same marker/polyline/polygon rendering
//   logic in the AGGRenderer
//   * Unlike the AGGRenderer, we don't have to concern ourselves with:
//     * Labeling and exclusion region management
//     * Rendering of rasters, symbols and text
//     * Style settings (basic or advanced) when rendering points/lines/polygons
//     * Font Management
//     * Size. Our back buffer is fixed at 256x256
//     * We don't render to an agg_context, we instead render to an agg_utfgrid_context, which does not have:
//       * Polyclip mask rendering
//       * Alpha masking
//       * Font engine state caching/tracking
//       * The underlying primitives is agg::int32u instead of agg:int8u
//     * Pixel format is re-used from mapserver (see MapUTFGrid.h)
//   * For the most part, we solely focus on the actual LineBuffer geometries given to us by the Stylizer
//     and rendering that directly to the agg_utfgrid_context
// * Like KmlRenderer, the renderered "pixels" are encoded into a UTFGrid tile, that is the responsibility
//   of the UTFGridContent class (just like KmlContent is for KmlRenderer). Initial rendering of the
//   "pixels" is handled by UTFGridRenderer. Encoding is handed off to UTFGridContent.
//
// =================================================================================== //

// ======================== Rendering Process ======================================== //
//
// StartMap
//    StartLayer
//        StartFeature
//           - Reset renderme flag to true
//           - Allocate a "color" (an incrementing unsigned int)
//           - Encode current feature attributes to UTFGridContent and associate it with the color
//           - Set renderme flag to true if feature encoding was successful
//           Service ProcessXXX calls from the Stylizer
//              - Ignore call if renderme == false
//              - Render each geometry into the agg_utfgrid_context
//    EndLayer
// EndMap
//    - Transfer back buffer to UTFGridContent for encoding
//    - UTFGridContent will use the color of each pixel to marry up with the encoded feature attributes when assembling the UTFGrid tile
//
// =================================================================================== //

UTFGridRenderer::UTFGridRenderer(UTFGridContent* utfGrid, unsigned int resolution)
  : m_currentColor(0),
    m_content(utfGrid),
    m_dpi(0),
    m_drawingScale(0.0),
    m_metersPerUnit(0.0),
    m_mapScale(0.0),
    m_mapInfo(NULL),
    m_layerInfo(NULL),
    m_fcInfo(NULL),
    m_renderThisFeature(true)
{
    // get the agg context going
    m_context = new agg_utfgrid_context(UTF_GRID_WIDTH, UTF_GRID_HEIGHT, resolution);
}

UTFGridRenderer::~UTFGridRenderer()
{
    delete m_context;
}

void UTFGridRenderer::StartMap(RS_MapUIInfo * mapInfo, RS_Bounds & extents, double mapScale, double dpi, double metersPerUnit, CSysTransformer * /*xformToLL*/)
{
    m_mapScale = mapScale;
    m_dpi = dpi;
    m_metersPerUnit = metersPerUnit;
    m_extents = extents;

    // find scale used to convert to pixel coordinates
    // need to take aspect ratios into account
    double arDisplay = 1.0;
    double arMap = m_extents.width() / m_extents.height();

    double scale;
    if (arDisplay > arMap)
        scale = (double)m_context->rendering_buffer.height() / m_extents.height();
    else
        scale = (double)m_context->rendering_buffer.width() / m_extents.width();

    m_xform.x0 = scale;
    m_xform.x1 = 0.0;
    m_xform.x2 = -scale * m_extents.minx;
    m_xform.y0 = 0.0;
    m_xform.y1 = scale;
    m_xform.y2 = -scale * m_extents.miny;

    m_ixform.x0 = 1.0 / scale;
    m_ixform.x1 = 0.0;
    m_ixform.x2 = m_extents.minx;
    m_ixform.y0 = 0.0;
    m_ixform.y1 = m_ixform.x0;
    m_ixform.y2 = m_extents.miny;

    double metersPerPixel = METERS_PER_INCH / m_dpi;

    // compute drawing scale
    // drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    // remember the map info
    m_mapInfo = mapInfo;
}

void UTFGridRenderer::EndMap()
{
    m_mapInfo = NULL;

    //First pass: Track the rendered pixels
    m_content->BeginTracking();

    unsigned int bufHeight = m_context->rendering_buffer.height();
    unsigned int bufWidth = m_context->rendering_buffer.width();

    for (int y = bufHeight - 1; y >= 0; y--)
    {
        utfgrid_band_type* row_ptr = m_context->rendering_buffer.row_ptr(y);
        for (int x = 0; x < bufWidth; x++)
        {
            m_content->TrackPixel(row_ptr[x]);
        }
    }

    //Second pass: Render the actual grid content. The tracked pixels in the previous pass
    //will assist in any re-mapping (as character space is at a premium)
    m_content->StartGrid();
    bool bFirst = true;
    //Y-axis is flipped, so reverse iterate the y-axis
    for (int y = bufHeight - 1; y >= 0; y--)
    {
        m_content->StartGridRow(bFirst);
        utfgrid_band_type* row_ptr = m_context->rendering_buffer.row_ptr(y);
        for (int x = 0; x < bufWidth; x++)
        {
            m_content->AppendRowPixel(row_ptr[x]);
        }
        m_content->EndGridRow();
        bFirst = false;
    }
    m_content->EndGrid();
    m_content->WriteKeys();
    m_content->WriteData();
}

void UTFGridRenderer::StartLayer(RS_LayerUIInfo * layerInfo, RS_FeatureClassInfo * classInfo)
{
    // remember the layer/feature info
    m_layerInfo = layerInfo;
    m_fcInfo = classInfo;
}

void UTFGridRenderer::EndLayer()
{
    // clear the layer/feature info
    m_layerInfo = NULL;
    m_fcInfo = NULL;
}

void UTFGridRenderer::StartFeature(RS_FeatureReader * feature, bool /* initialPass */, const RS_String * tooltip, const RS_String * url, const RS_String * /* theme */, double /* zOffset */, double /* zExtrusion */, RS_ElevationType /* zOffsetType */)
{
    m_currentColor++;
    m_renderThisFeature = m_content->AddFeature(m_layerInfo, m_currentColor, feature, tooltip, url);
}

void UTFGridRenderer::ProcessPolygon(LineBuffer * lb, RS_FillStyle & /*fill*/)
{
    if (!m_renderThisFeature)
        return;

    DrawScreenPolygon(lb, &m_xform, m_currentColor);
}

void UTFGridRenderer::ProcessPolyline(LineBuffer * lb, RS_LineStroke & /*lsym*/)
{
    if (!m_renderThisFeature)
        return;

    m_lineStroke.color = m_currentColor;
    DrawScreenPolyline(lb, &m_xform, m_lineStroke);
}

void UTFGridRenderer::ProcessRaster(unsigned char * /*data*/, int /*length*/, RS_ImageFormat /*format*/, int /*width*/, int /*height*/, RS_Bounds & /*extents*/, TransformMesh * /*xformMesh*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::ProcessMarker(LineBuffer * srclb, RS_MarkerDef & mdef, bool allowOverpost, RS_Bounds * /*bounds*/)
{
    if (!m_renderThisFeature)
        return;

    for (int i = 0; i<srclb->point_count(); ++i)
    {
        // if marker is processed from here it should be added to the
        // feature W2D, not the labeling W2D - need the API to reflect that.
        ProcessOneMarker(srclb->x_coord(i), srclb->y_coord(i), mdef, allowOverpost);
    }
}

void UTFGridRenderer::ProcessLabelGroup(RS_LabelInfo * /*labels*/, int /*nlabels*/, const RS_String & /*text*/, RS_OverpostType /*type*/, bool /*exclude*/, LineBuffer * /*path*/, double /*scaleLimit*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::AddDWFContent(RS_InputStream * /*in*/, CSysTransformer * /*xformer*/, const RS_String & /*section*/, const RS_String & /*passwd*/, const RS_String & /*w2dfilter*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::SetSymbolManager(RS_SymbolManager * /*manager*/)
{
    //Not applicable for UTFGrid
}

RS_MapUIInfo * UTFGridRenderer::GetMapInfo()
{
    return m_mapInfo;
}

RS_LayerUIInfo * UTFGridRenderer::GetLayerInfo()
{
    return m_layerInfo;
}

RS_FeatureClassInfo * UTFGridRenderer::GetFeatureClassInfo()
{
    return m_fcInfo;
}

double UTFGridRenderer::GetMapScale()
{
    return m_mapScale;
}

double UTFGridRenderer::GetDrawingScale()
{
    return m_drawingScale;
}

double UTFGridRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}

double UTFGridRenderer::GetDpi()
{
    return m_dpi;
}

RS_Bounds & UTFGridRenderer::GetBounds()
{
    return m_extents;
}

bool UTFGridRenderer::RequiresClipping()
{
    return false;
}

bool UTFGridRenderer::RequiresLabelClipping()
{
    return false;
}

bool UTFGridRenderer::SupportsZ()
{
    return false;
}

void UTFGridRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool /*allowOverpost*/)
{
    // symbol in mapping space units
    double iw = _MeterToMapSize(mdef.units(), mdef.width());
    double ih = _MeterToMapSize(mdef.units(), mdef.height());

    // unrotated symbol bounding box (mapping space)
    RS_Bounds dst(x, y, x + iw, y + ih);

    // convert to pixel size
    iw *= m_xform.x0;
    ih *= m_xform.y1;

    // if it's too big, make it smaller
    // it cannot be too big since we allocate 4*iw*ih bytes of
    // memory to cache the symbol image - this code simply truncates
    // the size, ignoring changes in aspect ratio
    const double MAX_SIZE = 4096.0;
    if (iw > MAX_SIZE)
    {
        iw = MAX_SIZE;
        dst.maxx = dst.minx + iw * m_ixform.x0;
    }
    if (ih > MAX_SIZE)
    {
        ih = MAX_SIZE;
        dst.maxy = dst.miny + ih * m_ixform.y1;
    }

    // get insertion point
    double refX = mdef.insx();
    double refY = mdef.insy();

    // rotation angle
    double angleRad = mdef.rotation() * M_PI180;

    // Unlike the AGGRenderer, we will just render an SLD_CIRCLE for this marker. We're not rendering
    // actual pixels with this renderer, just characters, so we can afford some leeway to ensure we can
    // get an adequate "hit area" for point features
    RS_F_Point* poly = (RS_F_Point*)SLD_CIRCLE;
    int npts = sizeof(SLD_CIRCLE) / (2 * sizeof(double));

    // construct transformer
    RS_Bounds src(0.0, 0.0, 1.0, 1.0);
    SymbolTrans trans(src, dst, refX, refY, angleRad);

    // transform to coordinates of temporary image where we
    // draw symbol before transfering to the map
    LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pPool, 8);
    std::unique_ptr<LineBuffer> spLB(lb);

    double tempx, tempy;
    for (int i = 0; i<npts; ++i)
    {
        tempx = poly[i].x;
        tempy = poly[i].y;

        // unit square to world
        trans.TransformPoint(tempx, tempy);

        WorldToScreenPoint(tempx, tempy, tempx, tempy);

        if (i)
            lb->LineTo(tempx, tempy);
        else
            lb->MoveTo(tempx, tempy);
    }

    // unknown symbol
    m_lineStroke.color = m_currentColor;
    m_lineStroke.weight = 1.0;
    DrawScreenPolyline(m_context, lb, NULL, m_lineStroke);

    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}

double UTFGridRenderer::_MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;

    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_mapScale / m_metersPerUnit;
    else
        scale_factor = 1.0 / m_metersPerUnit;

    return number * scale_factor;
}

void UTFGridRenderer::_TransferPoints(agg_utfgrid_context* c, LineBuffer* srcLB, const SE_Matrix* xform, unsigned int* pathids)
{
    c->ps.remove_all();

    int offset = 0;
    int* cntrs = srcLB->cntrs();

    if (pathids)
        *pathids = 0;

    for (int h = 0; h<srcLB->geom_count(); ++h)
    {
        if (h && pathids)
            pathids[h] = c->ps.start_new_path();

        int cur_cntr_count = srcLB->geom_size(h);

        for (int i = 0; i<cur_cntr_count; ++i)
        {
            int ci = offset;

            // add the initial point
            double sx = srcLB->x_coord(ci);
            double sy = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(sx, sy, sx, sy);

            c->ps.move_to(sx, sy);

            int ptlast = cntrs[i] - 1;
            if (!ptlast)
                continue;

            // add the interior points
            double tx, ty;
            for (int j = 1; j<ptlast; ++j)
            {
                tx = srcLB->x_coord(ci);
                ty = srcLB->y_coord(ci);
                ++ci;

                if (xform)
                    xform->transform(tx, ty, tx, ty);

                c->ps.line_to(tx, ty);
            }

            // add the final point
            tx = srcLB->x_coord(ci);
            ty = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(tx, ty, tx, ty);

            // detect a close segment (in a Linebuffer this is caused by first and
            // last point of the contour being equal and size of contour > 2 points)
            if (tx == sx && ty == sy && ptlast > 1)
                c->ps.close_polygon();
            else if (ptlast == 1)
            {
                // only two points in the buffer - check if this corresponds to a dot
                double dx = tx - sx;
                double dy = ty - sy;
                double len2 = dx*dx + dy*dy;

                // Although dots are replaced by lines with length LINE_SEGMENT_DOT_SIZE,
                // it's possible that the lines end up somewhat larger in the wrapping
                // case due to warping.  We therefore perform the check against a 10 times
                // larger length.  In the future it might be worth relaxing this to simply
                // check for any segment less than one pixel in length.
                if (len2 < 100.0*LINE_SEGMENT_DOT_SIZE*LINE_SEGMENT_DOT_SIZE)
                {
                    // found a dot - draw it as a 1-pixel long line
                    if (len2 > 0.0)
                    {
                        double len = sqrt(len2);
                        dx /= len;
                        dy /= len;
                        c->ps.move_to(tx - 0.5*dx, ty - 0.5*dy);
                        c->ps.line_to(tx + 0.5*dx, ty + 0.5*dy);
                    }
                    else
                    {
                        // length of line can be 0 when DOTS_HATCH is rendered from DWF
                        // so use LINE_SEGMENT_DOT_SIZE to render that dots
                        c->ps.move_to(tx - LINE_SEGMENT_DOT_SIZE, ty - LINE_SEGMENT_DOT_SIZE);
                        c->ps.line_to(tx + LINE_SEGMENT_DOT_SIZE, ty + LINE_SEGMENT_DOT_SIZE);
                    }
                }
                else
                {
                    // a normal line segment
                    c->ps.line_to(tx, ty);
                }
            }
            else
                c->ps.line_to(tx, ty);

            offset += cntrs[i];
        }

        cntrs += cur_cntr_count;
    }
}

void UTFGridRenderer::DrawScreenPolyline(LineBuffer * srclb, const SE_Matrix * xform, const SE_LineStroke & lineStroke)
{
    DrawScreenPolyline(m_context, srclb, xform, lineStroke);
}

void UTFGridRenderer::DrawScreenPolyline(agg_utfgrid_context* c, LineBuffer* srclb, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    // if you have no geoms, why do you call us at all?
    if (srclb->geom_count() == 0)
        return;

    double weightpx = rs_max(1.0, lineStroke.weight) / c->m_resolution;

    // add to the agg path storage - here it doesn't matter
    // how many geometries there are in the line buffer,
    // so we just pass NULL for the path offsets array
    _TransferPoints(c, srclb, xform, NULL);

    // stroke the line as a polygon
    agg::conv_stroke<agg::path_storage> stroke(c->ps);
    stroke.width(weightpx);

    // set up cap / join style
    switch (lineStroke.cap)
    {
    case SE_LineCap_None:
        stroke.line_cap(agg::butt_cap);
        break;
    case SE_LineCap_Triangle:
        stroke.line_cap(agg::triangle_cap);
        break;
    case SE_LineCap_Square:
        stroke.line_cap(agg::square_cap);
        break;
    case SE_LineCap_Round:
    default:
        stroke.line_cap(agg::round_cap);
        break;
    }

    switch (lineStroke.join)
    {
    case SE_LineJoin_None:
        stroke.line_join(agg::bevel_join);  // AGG doesn't support None
        break;
    case SE_LineJoin_Bevel:
        stroke.line_join(agg::bevel_join);
        break;
    case SE_LineJoin_Miter:
        stroke.line_join(agg::miter_join);
        stroke.miter_limit(2.0*lineStroke.miterLimit);  // miter limit is relative to the half-width
        break;
    case SE_LineJoin_Round:
    default:
        stroke.line_join(agg::round_join);
        break;
    }

    c->rasterizer.reset();
    c->rasterizer.filling_rule(agg::fill_even_odd);
    c->rasterizer.add_path(stroke);

    c->renderer_scanline.color(utfpix32(UTFGridContent::EncodeChar(lineStroke.color)));
    agg::render_scanlines(c->rasterizer, c->scanline_utf, c->renderer_scanline);
}

void UTFGridRenderer::DrawScreenPolygon(LineBuffer * polygon, const SE_Matrix * xform, unsigned int fill)
{
    // if you have no geoms, why do you call us at all?
    if (polygon->geom_count() == 0)
        return;

    // transform to screen coords and transfer to AGG storage
    unsigned int* pathids = (unsigned int*)alloca(polygon->geom_count() * sizeof(unsigned int));
    _TransferPoints(m_context, polygon, xform, pathids);

    for (int i = 0; i<polygon->geom_count(); ++i)
    {
        m_context->rasterizer.reset();
        m_context->rasterizer.add_path(m_context->ps, pathids[i]);

        m_context->renderer_scanline.color(utfpix32(UTFGridContent::EncodeChar(fill)));
        agg::render_scanlines(m_context->rasterizer, m_context->scanline_utf, m_context->renderer_scanline);
    }
}

void UTFGridRenderer::DrawScreenRaster(unsigned char * /*data*/, int /*length*/, RS_ImageFormat /*format*/, int /*native_width*/, int /*native_height*/, double /*x*/, double /*y*/, double /*w*/, double /*h*/, double /*angleDeg*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::DrawScreenRaster(unsigned char * /*data*/, int /*length*/, RS_ImageFormat /*format*/, int /*native_width*/, int /*native_height*/, double /*x*/, double /*y*/, double /*w*/, double /*h*/, double /*angleDeg*/, double /*alpha*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::DrawScreenText(const RS_TextMetrics & /*tm*/, RS_TextDef & /*tdef*/, double /*insx*/, double /*insy*/, RS_F_Point * /*path*/, int /*npts*/, double /*param_position*/)
{
    //Not applicable for UTFGrid
}

bool UTFGridRenderer::YPointsUp()
{
    return false;
}

void UTFGridRenderer::GetWorldToScreenTransform(SE_Matrix & xform)
{
    xform = m_xform;
}

void UTFGridRenderer::WorldToScreenPoint(double & inx, double & iny, double & ox, double & oy)
{
    m_xform.transform(inx, iny, ox, oy);
}

void UTFGridRenderer::ScreenToWorldPoint(double & inx, double & iny, double & ox, double & oy)
{
    m_ixform.transform(inx, iny, ox, oy);
}

double UTFGridRenderer::GetScreenUnitsPerMillimeterDevice()
{
    return m_dpi / MILLIMETERS_PER_INCH;
}

double UTFGridRenderer::GetScreenUnitsPerMillimeterWorld()
{
    return m_dpi / MILLIMETERS_PER_INCH / m_mapScale;
}

double UTFGridRenderer::GetScreenUnitsPerPixel()
{
    return 1.0;
}

RS_FontEngine * UTFGridRenderer::GetRSFontEngine()
{
    return NULL;
}

void UTFGridRenderer::ProcessSELabelGroup(SE_LabelInfo * /*labels*/, int /*nlabels*/, RS_OverpostType /*type*/, bool /*exclude*/, LineBuffer * /*path*/)
{
    //Not applicable for UTFGrid
}

void UTFGridRenderer::AddExclusionRegion(RS_F_Point * /*fpts*/, int /*npts*/)
{
    //Not applicable for UTFGrid
}
