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

#ifndef RENDERERSTYLES_H_
#define RENDERERSTYLES_H_

#include <vector>
#include <string>

typedef std::wstring RS_String;


//////////////////////////////////////////////////////////////////////////////
enum RS_HAlignment
{
    RS_HAlignment_Left,
    RS_HAlignment_Center,
    RS_HAlignment_Right,
    RS_HAlignment_Path
};


//////////////////////////////////////////////////////////////////////////////
enum RS_VAlignment
{
    RS_VAlignment_Descent,
    RS_VAlignment_Base,
    RS_VAlignment_Half,
    RS_VAlignment_Cap,
    RS_VAlignment_Ascent
};


//////////////////////////////////////////////////////////////////////////////
enum RS_Justify
{
    RS_Justify_Left,
    RS_Justify_Center,
    RS_Justify_Right,
    RS_Justify_Justify
};


//////////////////////////////////////////////////////////////////////////////
enum RS_FontStyle_Mask
{
    RS_FontStyle_Regular    = 0,
    RS_FontStyle_Bold       = 1,
    RS_FontStyle_Italic     = 2,
    RS_FontStyle_Underline  = 4,
    RS_FontStyle_Overline   = 8
};


//////////////////////////////////////////////////////////////////////////////
enum RS_Units
{
    RS_Units_Device,
    RS_Units_Model
};


//////////////////////////////////////////////////////////////////////////////
enum RS_ImageFormat
{
    RS_ImageFormat_Unknown = 0,
    RS_ImageFormat_ABGR,    // RGBA byte order
    RS_ImageFormat_RGB,     // BGR  byte order
    RS_ImageFormat_JPEG,
    RS_ImageFormat_PNG,
    RS_ImageFormat_ARGB,    // BGRA byte order
    RS_ImageFormat_NATIVE
};


//////////////////////////////////////////////////////////////////////////////
enum RS_TextBackground
{
    // used as a bit mask
    RS_TextBackground_None    = 0,
    RS_TextBackground_Ghosted = 1,  // the text is drawn ghosted
    RS_TextBackground_Framed  = 2,  // the text BB is drawn using a frame
    RS_TextBackground_Opaque  = 4   // the text BB is drawn using a filled rectangle
};


//////////////////////////////////////////////////////////////////////////////
enum RS_OverpostType
{
    RS_OverpostType_All,
    RS_OverpostType_AllFit,
    RS_OverpostType_FirstFit
};


//////////////////////////////////////////////////////////////////////////////
enum RS_ElevationType
{
    RS_ElevationType_RelativeToGround,
    RS_ElevationType_Absolute
};


//////////////////////////////////////////////////////////////////////////////
enum LabelAlgo
{
    laSimple,
    laCurve,
    laSkeleton,
    laPeriodicPolygon,
    laSESymbol
};


//////////////////////////////////////////////////////////////////////////////
enum RS_MarkerType
{
    RS_MarkerType_Marker,
    RS_MarkerType_Block,
    RS_MarkerType_W2D,
    RS_MarkerType_Font,
    RS_MarkerType_Image
};


//////////////////////////////////////////////////////////////////////////////
// Note that these enum values are set to match the WIN32 charset values because it is expected that the
// CharacterSet value will most likely be used on a WIN32 platform to select a font.
enum RS_CharacterSetType
{
    RS_CharSet_Undefined = -1,
    RS_ANSI              = 0,
    RS_Default           = 1,
    RS_Symbol            = 2,
    RS_Mac               = 77,
    RS_ShiftJIS          = 128,
    RS_Hangul            = 129,
    RS_Johab             = 130,
    RS_GB2312            = 134,
    RS_Big5              = 136,
    RS_Greek             = 161,
    RS_Turkish           = 162,
    RS_Vietnamese        = 163,
    RS_Hebrew            = 177,
    RS_Arabic            = 178,
    RS_Baltic            = 186,
    RS_Russian           = 204,
    RS_Thai              = 222,
    RS_EastEurope        = 238,
    RS_OEM               = 255
};


//////////////////////////////////////////////////////////////////////////////
class RS_Color
{
public:
    static const int EMPTY_COLOR_ARGB = 0x00000000;
    static const int EMPTY_COLOR_RGBA = 0x00000000;

    RS_Color() :
        m_red(0),
        m_green(0),
        m_blue(0),
        m_alpha(255)
    {
    }

    RS_Color(int r, int g, int b, int a)
    {
        m_red   = r;
        m_green = g;
        m_blue  = b;
        m_alpha = a;
    }

    RS_Color(int rgba)
    {
        m_red   = (rgba >> 24) & 0xFF;
        m_green = (rgba >> 16) & 0xFF;
        m_blue  = (rgba >> 8)  & 0xFF;
        m_alpha =  rgba        & 0xFF;
    }

    inline int& red()   { return m_red; }
    inline int& green() { return m_green; }
    inline int& blue()  { return m_blue; }
    inline int& alpha() { return m_alpha; }

    inline int argb() const
    {
        return ((m_alpha & 0xFF) << 24) |
               ((m_red   & 0xFF) << 16) |
               ((m_green & 0xFF) <<  8) |
                (m_blue  & 0xFF);
    }

    inline int abgr() const
    {
        return ((m_alpha & 0xFF) << 24) |
               ((m_blue  & 0xFF) << 16) |
               ((m_green & 0xFF) <<  8) |
                (m_red   & 0xFF);
    }

    static RS_Color FromARGB(unsigned int argb)
    {
        RS_Color ret;
        ret.m_alpha = (argb >> 24) & 0xFF;
        ret.m_red   = (argb >> 16) & 0xFF;
        ret.m_green = (argb >> 8)  & 0xFF;
        ret.m_blue  =  argb        & 0xFF;
        return ret;
    }

private:
    int m_red;
    int m_green;
    int m_blue;
    int m_alpha;
};


//////////////////////////////////////////////////////////////////////////////
/// RFC60 list of colors used for color quantization
typedef std::vector<RS_Color> RS_ColorVector;


//////////////////////////////////////////////////////////////////////////////
class RS_LineStroke
{
public:
    RS_LineStroke() :
        m_color(),
        m_width(0.0),
        m_style(L"Solid"),
        m_units(RS_Units_Device)
    {
    }

    RS_LineStroke(RS_Color&        lineColor,
                  double           lineWidth,
                  const RS_String& lineStyle,
                  RS_Units         units)
    {
        m_color = lineColor;
        m_width = lineWidth;
        m_style = lineStyle;
        m_units = units;
    }

    inline RS_Color&  color() { return m_color; }
    inline double&    width() { return m_width; }
    inline RS_String& style() { return m_style; }
    inline RS_Units&  units() { return m_units; }

private:
    RS_Color m_color;
    double m_width;
    RS_String m_style;
    RS_Units m_units;
};


//////////////////////////////////////////////////////////////////////////////
class RS_FillStyle
{
public:
    RS_FillStyle() :
        m_outline(),
        m_color(255, 255, 255, 255),
        m_background(0, 0, 0, 0),
        m_pattern(L"Solid")
    {
    }

    RS_FillStyle(RS_LineStroke&   outlineStroke,
                 RS_Color&        fillColor,
                 RS_Color&        bgColor,
                 const RS_String& fillPattern) :
        m_outline(outlineStroke),
        m_color(fillColor),
        m_background(bgColor),
        m_pattern(fillPattern)
    {
    }

    inline RS_LineStroke& outline()    { return m_outline; }
    inline RS_Color&      color()      { return m_color; }
    inline RS_Color&      background() { return m_background; }
    inline RS_String&     pattern()    { return m_pattern; }

private:
    RS_LineStroke m_outline;
    RS_Color m_color;
    RS_Color m_background;
    RS_String m_pattern;
};


//////////////////////////////////////////////////////////////////////////////
class RS_FontDef
{
public:
    RS_FontDef() :
        m_height(12.0),
        m_name(L"Default"),
        m_style(RS_FontStyle_Regular),
        m_units(RS_Units_Device),
        m_charset(RS_CharSet_Undefined)
    {
    }

    RS_FontDef(RS_String&        name,
               double            height,
               RS_FontStyle_Mask style,
               RS_Units          units) :
        m_name(name),
        m_height(height),
        m_style(style),
        m_units(units),
        m_charset(RS_CharSet_Undefined)
    {
    }

    RS_FontDef(RS_String&          name,
               double              height,
               RS_FontStyle_Mask   style,
               RS_Units            units,
               RS_CharacterSetType charset) :
        m_name(name),
        m_height(height),
        m_style(style),
        m_units(units),
        m_charset(charset)
    {
    }

    inline double&              height() { return m_height; }
    inline RS_String&           name()   { return m_name; }
    inline RS_FontStyle_Mask&   style()  { return m_style; }
    inline RS_Units&            units()  { return m_units; }
    inline RS_CharacterSetType& charset(){ return m_charset; }

private:
    double m_height;
    RS_String m_name;
    RS_FontStyle_Mask m_style;
    RS_Units m_units;
    RS_CharacterSetType m_charset;
};


//////////////////////////////////////////////////////////////////////////////
// Stores text style information.  The rotation is in degrees CCW.
class RS_TextDef
{
public:
    RS_TextDef() :
        m_halign(RS_HAlignment_Center),
        m_valign(RS_VAlignment_Half),
        m_justify(RS_Justify_Left),
        m_textbg(RS_TextBackground_None),
        m_rotation(0.0),
        m_obliqueAngle(0.0),
        m_trackSpacing(1.0),
        m_linespace(1.05),
        m_frameoffsetx(0.0),
        m_frameoffsety(0.0),
        m_markup(L"")
    {
    }

    RS_TextDef(RS_HAlignment halign,
               RS_VAlignment valign) :
        m_halign(halign),
        m_valign(valign),
        m_justify(RS_Justify_Left),
        m_textbg(RS_TextBackground_None),
        m_rotation(0.0),
        m_obliqueAngle(0.0),
        m_trackSpacing(1.0),
        m_linespace(1.05),
        m_frameoffsetx(0.0),
        m_frameoffsety(0.0),
        m_markup(L"")
    {
    }

    inline RS_HAlignment& halign()       { return m_halign; }
    inline RS_VAlignment& valign()       { return m_valign; }
    inline RS_Justify&    justify()      { return m_justify; }
    inline int&           textbg()       { return m_textbg; }
    inline RS_Color&      textcolor()    { return m_textcolor; }
    inline RS_Color&      ghostcolor()   { return m_ghostcolor; }
    inline RS_Color&      framecolor()   { return m_framecolor; }
    inline RS_Color&      opaquecolor()  { return m_opaquecolor; }
    inline RS_FontDef&    font()         { return m_font; }
    inline double&        rotation()     { return m_rotation; }
    inline double&        obliqueAngle() { return m_obliqueAngle; }
    inline double&        trackSpacing() { return m_trackSpacing; }
    inline double&        linespace()    { return m_linespace; }
    inline double&        frameoffsetx() { return m_frameoffsetx; }
    inline double&        frameoffsety() { return m_frameoffsety; }
    inline RS_String&     markup()       { return m_markup; }

private:
    RS_HAlignment m_halign;
    RS_VAlignment m_valign;
    RS_Justify m_justify;
    int m_textbg;
    RS_Color m_textcolor;
    RS_Color m_ghostcolor;
    RS_Color m_framecolor;
    RS_Color m_opaquecolor;
    RS_FontDef m_font;
    double m_rotation;
    double m_obliqueAngle;
    double m_trackSpacing;
    double m_linespace;
    double m_frameoffsetx;
    double m_frameoffsety;
    RS_String m_markup;
};


//////////////////////////////////////////////////////////////////////////////
// Stores marker style information.  The rotation is in degrees CCW.
class RS_MarkerDef
{
public:
    RS_MarkerDef() :
        m_type(RS_MarkerType_Marker),
        m_width(0.01),
        m_height(0.01),
        m_insx(0.5),
        m_insy(0.5),
        m_rotation(0.0),
        m_units(RS_Units_Device),
        m_markernum(0),
        m_fontstyle(RS_FontStyle_Regular)
    {
    }

    RS_MarkerDef(RS_MarkerType       type,
                 double              width,
                 double              height,
                 double              insx,
                 double              insy,
                 double              rotation,
                 RS_Units            units,
                 int                 markernum,
                 const RS_String&    library,
                 const RS_String&    name,
                 const RS_FillStyle& style) :
        m_type(type),
        m_width(width),
        m_height(height),
        m_insx(insx),
        m_insy(insy),
        m_rotation(rotation),
        m_units(units),
        m_markernum(markernum),
        m_library(library),
        m_name(name),
        m_style(style),
        m_fontstyle(RS_FontStyle_Regular)
    {
    }

    inline RS_MarkerType&     type()      { return m_type; }
    inline double&            width()     { return m_width; }
    inline double&            height()    { return m_height; }
    inline double&            insx()      { return m_insx; }
    inline double&            insy()      { return m_insy; }
    inline double&            rotation()  { return m_rotation; }
    inline RS_Units&          units()     { return m_units; }
    inline int&               markernum() { return m_markernum; }
    inline RS_String&         library()   { return m_library; }
    inline RS_String&         name()      { return m_name; }
    inline RS_FillStyle&      style()     { return m_style; }
    inline RS_FontStyle_Mask& fontstyle() { return m_fontstyle; }

private:
    RS_MarkerType m_type;
    double m_width;
    double m_height;
    double m_insx;
    double m_insy;
    double m_rotation;
    RS_Units m_units;
    int m_markernum;                // used with marker symbols
    RS_String m_library;            // used with block and W2D symbols
    RS_String m_name;               // used with block and W2D symbols
    RS_FillStyle m_style;           // used with all symbols
    RS_FontStyle_Mask m_fontstyle;  // used with font symbols
};


//////////////////////////////////////////////////////////////////////////////
class RS_UIGraphic
{
public:
    RS_UIGraphic() :
        m_data(NULL),
        m_length(0),
        m_label(L"")
    {
    }

    RS_UIGraphic(unsigned char* data, int length, const RS_String& label) :
        m_data(data),
        m_length(length),
        m_label(label)
    {
    }

    inline unsigned char*& data()   { return m_data; }
    inline int&            length() { return m_length; }
    inline RS_String&      label()  { return m_label; }

private:
    unsigned char* m_data;
    int            m_length;
    RS_String      m_label;
};


//////////////////////////////////////////////////////////////////////////////
class RS_ElevationSettings
{
public:
    RS_ElevationSettings() :
        m_zOffsetExpression(L""),
        m_zExtrusionExpression(L""),
        m_elevType(RS_ElevationType_RelativeToGround),
        m_metersPerUnit(1.0)
    {
    }

    RS_ElevationSettings(const RS_String& zOffsetExpression,
        const RS_String& zExtrusionExpression,
        double metersPerUnit,
        RS_ElevationType elevType) :
        m_zOffsetExpression(zOffsetExpression),
        m_zExtrusionExpression(zExtrusionExpression),
        m_metersPerUnit(metersPerUnit),
        m_elevType(elevType)
    {
    }

    inline RS_String&        zOffsetExpression()    { return m_zOffsetExpression; }
    inline RS_String&        zExtrusionExpression() { return m_zExtrusionExpression; }
    inline RS_ElevationType& elevType()             { return m_elevType; }
    inline double&           metersPerUnit()        { return m_metersPerUnit; }

private:
    RS_String m_zOffsetExpression;
    RS_String m_zExtrusionExpression;
    RS_ElevationType m_elevType;
    double m_metersPerUnit;
};


//////////////////////////////////////////////////////////////////////////////
class RS_LayerUIInfo
{
public:
    RS_LayerUIInfo() :
        m_name(L""),
        m_layerdef(L""),
        m_guid(L""),
        m_selectable(true),
        m_visible(true),
        m_editable(true),
        m_showInLegend(true),
        m_expandInLegend(true),
        m_groupname(L""),
        m_groupguid(L""),
        m_zorder(0.0),
        m_uiGraphic(NULL, 0, L""),
        m_hastooltips(false),
        m_hashyperlinks(false)
    {
    }

    RS_LayerUIInfo(const RS_String& name,
                   const RS_String& layerdef,
                   const RS_String& guid,
                   bool             selectable,
                   bool             visible,
                   bool             editable,
                   const RS_String& groupname,
                   const RS_String& groupguid,
                   bool             showInLegend,
                   bool             expandInLegend,
                   double           zorder,
                   RS_UIGraphic&    uiGraphic,
                   bool             hastooltips = false,
                   bool             hashyperlinks = false) :
        m_name(name),
        m_layerdef(layerdef),
        m_guid(guid),
        m_selectable(selectable),
        m_visible(visible),
        m_editable(editable),
        m_showInLegend(showInLegend),
        m_expandInLegend(expandInLegend),
        m_groupname(groupname),
        m_groupguid(groupguid),
        m_zorder(zorder),
        m_uiGraphic(uiGraphic),
        m_hastooltips(hastooltips),
        m_hashyperlinks(hashyperlinks)
    {
    }

    inline RS_String&    name()          { return m_name; }
    inline RS_String&    layerdef()      { return m_layerdef; }
    inline RS_String&    guid()          { return m_guid; }
    inline bool&         selectable()    { return m_selectable; }
    inline bool&         visible()       { return m_visible; }
    inline bool&         editable()      { return m_editable; }
    inline bool&         show()          { return m_showInLegend; }
    inline bool&         expand()        { return m_expandInLegend; }
    inline RS_String&    groupname()     { return m_groupname; }
    inline RS_String&    groupguid()     { return m_groupguid; }
    inline double&       zorder()        { return m_zorder; }
    inline RS_UIGraphic& graphic()       { return m_uiGraphic; }
    inline bool&         hastooltips()   { return m_hastooltips; }
    inline bool&         hashyperlinks() { return m_hashyperlinks; }

private:
    RS_String m_name;
    RS_String m_layerdef;
    RS_String m_guid;
    bool m_selectable;
    bool m_visible;
    bool m_editable;
    bool m_showInLegend;
    bool m_expandInLegend;
    RS_String m_groupname;
    RS_String m_groupguid;
    double m_zorder;
    RS_UIGraphic m_uiGraphic;
    bool m_hastooltips;
    bool m_hashyperlinks;
};


//////////////////////////////////////////////////////////////////////////////
class RS_MapUIInfo
{
public:
    RS_MapUIInfo() :
        m_session(L""),
        m_name(L""),
        m_guid(L""),
        m_coordsys(L""),
        m_units(L""),
        m_bgcolor(255, 255, 255, 255),
        m_viewx(0.0),
        m_viewy(0.0),
        m_scale(0.0)
    {
    }

    RS_MapUIInfo(const RS_String& session,
                 const RS_String& name,
                 const RS_String& guid,
                 const RS_String& coordsys,
                 const RS_String& units,
                 const RS_Color&  bgcolor,
                 const double viewx = 0.0,
                 const double viewy = 0.0,
                 const double scale = 0.0) :
        m_session(session),
        m_name(name),
        m_guid(guid),
        m_coordsys(coordsys),
        m_units(units),
        m_bgcolor(bgcolor),
        m_viewx(viewx),
        m_viewy(viewy),
        m_scale(scale)
    {
    }

    inline RS_String& session()  { return m_session; }
    inline RS_String& name()     { return m_name; }
    inline RS_String& guid()     { return m_guid; }
    inline RS_String& coordsys() { return m_coordsys; }
    inline RS_String& units()    { return m_units; }
    inline RS_Color&  bgcolor()  { return m_bgcolor; }
    inline double&    scale()    { return m_scale; }
    inline double&    viewx()    { return m_viewx; }
    inline double&    viewy()    { return m_viewy; }

private:
    RS_String m_session;
    RS_String m_name;
    RS_String m_guid;
    RS_String m_coordsys;
    RS_String m_units;
    RS_Color  m_bgcolor;
    double    m_scale;
    double    m_viewx;
    double    m_viewy;
};


//////////////////////////////////////////////////////////////////////////////
class RS_FeatureClassInfo
{
public:
    RS_FeatureClassInfo() :
        m_name(L""), m_source(L"")
    {
    }

    RS_FeatureClassInfo(const RS_String& name, const RS_String& source) :
        m_name(name), m_source(source)
    {
    }

    //this function is a little out of place here
    //since it does actual work as opposed to everything
    //else in RendererStyles, but for now it will do
    //for now a mapping is represented by two consecutive
    //strings in the internal mapping collection
    void add_mapping(const RS_String& origname, const RS_String& dispname)
    {
        m_propMappings.push_back(origname);
        m_propMappings.push_back(dispname);
    }

    inline RS_String&              name()     { return m_name; }
    inline std::vector<RS_String>& mappings() { return m_propMappings; }
    inline RS_String&              source()   { return m_source; }

private:
    RS_String m_source;
    RS_String m_name;
    std::vector<RS_String> m_propMappings;
};


//////////////////////////////////////////////////////////////////////////////
class RS_LabelInfo
{
public:
    RS_LabelInfo() :
        m_x(0.0),
        m_y(0.0),
        m_dx(0.0),
        m_dy(0.0)
    {
    }

    RS_LabelInfo(double x, double y,
                 double dx, double dy,
                 RS_Units dunits,
                 RS_TextDef& tdef) :
        m_x(x),
        m_y(y),
        m_dx(dx),
        m_dy(dy),
        m_dunits(dunits),
        m_tdef(tdef)
    {
    }

    RS_LabelInfo(double x, double y,
                 RS_TextDef& tdef) :
        m_x(x),
        m_y(y),
        m_dx(0.0),
        m_dy(0.0),
        m_dunits(tdef.font().units()),
        m_tdef(tdef)
        {
        }

    inline RS_TextDef& tdef()     { return m_tdef; }
    inline double&     x()        { return m_x; }
    inline double&     y()        { return m_y; }
    inline double&     dx()       { return m_dx; }
    inline double&     dy()       { return m_dy; }
    inline RS_Units&   dunits()   { return m_dunits; }

private:
    RS_TextDef m_tdef;
    double m_x;
    double m_y;
    double m_dx;
    double m_dy;
    RS_Units m_dunits;
};


//////////////////////////////////////////////////////////////////////////////
struct RS_D_Point
{
    //do not change this -- these are sometimes cast to int arrays
    int x;
    int y;

    RS_D_Point() : x(0), y(0) {}
    RS_D_Point(int d0, int d1) : x(d0), y(d1) {}
};


//////////////////////////////////////////////////////////////////////////////
struct RS_F_Point
{
    //do not change this -- these are sometimes cast to double arrays
    double x;
    double y;

    RS_F_Point() : x(0.0), y(0.0) {}
    RS_F_Point(double d0, double d1) : x(d0), y(d1) {}
};

#endif
