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

#include "Foundation.h"
#include "ParseAwkt.h"
#include "ParseAwktUtil.h"

// Note, if you enable this define here or LexAwkt.h you'll see output
// of the temporary data structures used to save the double values and
// context entries for each component of the geometry
// #define DEBUG_TRACEACTIONS   1

MgParseAwkt::MgParseAwkt()
{
    m_dim = kXY;
    m_geometry = (MgGeometry*)NULL;
    m_values = GisDoubleArray::Create();
    m_types = GisIntArray::Create();
    m_dims = GisIntArray::Create();
    m_starts = GisIntArray::Create();
    m_type = kNone;
    m_break = false;

    // ... no lexical analyzer
    m_lex = NULL;

    gis_awkt_yydebug = 0;
    gis_awkt_yynerrs = 0;
    gis_awkt_yyerrflag = 0;
    gis_awkt_yychar = -1;
    gis_awkt_yyssp = NULL;
    gis_awkt_yyvsp = NULL;
#ifndef _WIN32
        gis_awkt_yyss = NULL;
        gis_awkt_yysslim = NULL;
        gis_awkt_yyvs = NULL;
        gis_awkt_yystacksize = 0;
#endif
    gis_awkt_yylineno = 0;
}

MgParseAwkt::~MgParseAwkt()
{
    if (m_lex != NULL)
        delete m_lex;
    SAFE_RELEASE(m_starts);
    SAFE_RELEASE(m_dims);
    SAFE_RELEASE(m_types);
    SAFE_RELEASE(m_values);
#ifndef _WIN32
    if (gis_awkt_yyss != NULL)
    {
        free(gis_awkt_yyss);
        gis_awkt_yyss = NULL;
    }
    if (gis_awkt_yyvs != NULL)
    {
        free(gis_awkt_yyvs);
        gis_awkt_yyvs = NULL;
    }
#endif
}

MgGeometry* MgParseAwkt::ParseAwkt(wchar_t* pwzAwkt)
{
    assert(NULL != pwzAwkt);

    CHECKARGUMENTNULL(pwzAwkt, L"MgParseAwkt.ParseAwkt");

    m_lex = new GisLexAwkt(pwzAwkt);
    if (m_lex == NULL)
    {
        return NULL;
    }

    gis_awkt_yyparse(this);
    if (m_geometry == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(pwzAwkt);

        throw new MgInvalidArgumentException(L"MgParseAwkt.ParseAwkt",
            __LINE__, __WFILE__, &arguments, L"MgInvalidAgfText", NULL);
    }

    return SAFE_ADDREF(m_geometry.p);
}

// count how many of the same type, starting at iContext and counting it
// but don't move iContext forward (call by value)
INT32 MgParseAwkt::CountSame(INT32 iContext, _types type)
{
    INT32 iResult = 0;
    INT32 countContext = m_types->GetCount();
    while (iContext < countContext && (*m_types)[iContext] == type)
    {
        iResult++;
        iContext++;
    }
    return iResult;
}

MgPoint* MgParseAwkt::DoPoint(INT32& iContext, double* doubles)
{
    _dims dim = (_dims) (*m_dims)[iContext];

    INT32 index = 0;
    Ptr<MgPoint> geom = MgParseAwktUtil::CreatePoint(DimToDimensionality(dim), &doubles[(*m_starts)[iContext]], index);
    iContext++;

    return geom.Detach();
}

MgLineString* MgParseAwkt::DoLineString(INT32& iContext, double* doubles, _types type)
{
    // line strings can have multiple context entries and corresponding n-values per point
    MgGeometryFactory factory;

    _dims dim = (_dims) (*m_dims)[iContext];

    INT32 countSame = CountSame(iContext, type);
    INT32 countValues = countSame*DimToCount(dim);

    INT32 index = 0;
    Ptr<MgLineString> geom = MgParseAwktUtil::CreateLineString(DimToDimensionality(dim),
        countSame, &doubles[(*m_starts)[iContext]], index);
    iContext += countSame;

    return geom.Detach();
}

MgPolygon* MgParseAwkt::DoPolygon(INT32& iContext, double* doubles, _types type)
{
    // polygons have 1 exterior ring and 0 or more interior rings
    MgGeometryFactory factory;

    _dims dim = (_dims) (*m_dims)[iContext];

    // exterior ring
    INT32 countSame = CountSame(iContext, type);
    INT32 countValues = countSame*DimToCount(dim);
    INT32 index = 0;

    Ptr<MgLinearRing> extRing = MgParseAwktUtil::CreateLinearRing(DimToDimensionality(dim),
        countSame, &doubles[(*m_starts)[iContext]], index);

    iContext += countSame;

    // interior ring
    Ptr<MgLinearRingCollection> intRings = new MgLinearRingCollection();
    while (iContext < m_types->GetCount() && (*m_types)[iContext] == -type)
    {
        (*m_types)[iContext] = type;
        countSame = CountSame(iContext, type);
        countValues = countSame*DimToCount(dim);

        INT32 index = 0;
        Ptr<MgLinearRing> intRing = MgParseAwktUtil::CreateLinearRing(DimToDimensionality(dim),
            countSame, &doubles[(*m_starts)[iContext]], index);

        if (intRing != NULL)
            intRings->Add(intRing);

        iContext += countSame;
    }

    Ptr<MgPolygon> geom = factory.CreatePolygon(extRing, intRings);

    return geom.Detach();
}

MgMultiPoint* MgParseAwkt::DoMultiPoint(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgPointCollection> pnts = new MgPointCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    // there can be n-context entries for each point
    INT32 countContext = CountSame(iContext, kMultiPoint);

    for (int i=0; i < countContext; i++)
    {
        // add each point to the collection
        dim = (_dims) (*m_dims)[i];

        INT32 index = 0;
        Ptr<MgPoint> pnt = MgParseAwktUtil::CreatePoint(DimToDimensionality(dim), &doubles[(*m_starts)[iContext]], index);
        if (pnt != NULL)
            pnts->Add(pnt);
        iContext++;
    }

    Ptr<MgMultiPoint> geom = factory.CreateMultiPoint(pnts);

    return geom.Detach();
}

MgMultiLineString* MgParseAwkt::DoMultiLineString(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgLineStringCollection> lineStrings = new MgLineStringCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    Ptr<MgLineString> lineString = (MgLineString*)DoLineString(iContext, doubles, kMultiLineString);
    if (lineString != NULL)
        lineStrings->Add(lineString);

    while (iContext < m_types->GetCount() && (*m_types)[iContext] == -kMultiLineString)
    {
        (*m_types)[iContext] = kMultiLineString;
        Ptr<MgLineString> lineString1 = (MgLineString*)DoLineString(iContext, doubles, kMultiLineString);
        if (lineString1 != NULL)
        {
            lineStrings->Add(lineString1);
        }
    }

    Ptr<MgMultiLineString> geom = factory.CreateMultiLineString(lineStrings);

    return geom.Detach();
}

MgMultiPolygon* MgParseAwkt::DoMultiPolygon(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgPolygonCollection> polygons = new MgPolygonCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    Ptr<MgPolygon> polygon = DoPolygon(iContext, doubles, kMultiPolygon);
    if (polygon != NULL)
        polygons->Add(polygon);

    while (iContext < m_types->GetCount() && (*m_types)[iContext] == kRing)
    {
        // skip polygons separator (ring)
        iContext++;
        Ptr<MgPolygon> polygon1 = DoPolygon(iContext, doubles, kMultiPolygon);
        if (polygon1 != NULL)
            polygons->Add(polygon1);
    }

    Ptr<MgMultiPolygon> geom = factory.CreateMultiPolygon(polygons);

    return geom.Detach();
}

MgCurveSegmentCollection* MgParseAwkt::DoCurveSegmentCollection(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgCurveSegmentCollection> segs = new MgCurveSegmentCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    // skip the first point (look back at the previous point for each segment)
    iContext++;
    INT32 index = 0;

    // while kCircularArcSegment or kLineStringSegment create curve segments
    _types typeSegment;
    while (iContext < m_types->GetCount())
    {
        typeSegment = (_types)(*m_types)[iContext];

        if (typeSegment == kCircularArcSegment)
        {
            // this trick (-1,+0,+1) works because the previous point is either the first point
            // or the last point from the previous segment
            index = 0;
            Ptr<MgCoordinate> start = MgParseAwktUtil::CreateCoordinate(DimToDimensionality(dim),
                &doubles[(*m_starts)[iContext-1]],index);

            index = 0;
            Ptr<MgCoordinate> mid = MgParseAwktUtil::CreateCoordinate(DimToDimensionality(dim),
                &doubles[(*m_starts)[iContext]],index);

            index = 0;
            Ptr<MgCoordinate> end = MgParseAwktUtil::CreateCoordinate(DimToDimensionality(dim),
                &doubles[(*m_starts)[iContext+1]],index);

            Ptr<MgArcSegment> seg = factory.CreateArcSegment(start, end, mid);

            segs->Add(seg);

            iContext += 2;
        }
        else if (typeSegment == kLineStringSegment)
        {
            INT32 countSame = CountSame(iContext, kLineStringSegment);
            INT32 countValues = countSame*DimToCount(dim);

            //Ptr<MgLinearSegment> linearSeg = m_gf->CreateLineStringSegment(DimToDimensionality(dim),
                // countValues+DimToCount(dim), &doubles[(*m_starts)[iContext-1]]);
            INT32 index = 0;
            Ptr<MgLinearSegment> linearSeg = MgParseAwktUtil::CreateLinearSegment(DimToDimensionality(dim),
                 countSame+1, &doubles[(*m_starts)[iContext-1]], index);

            segs->Add(linearSeg);

            iContext += countSame;
        }
        else
            break;
    }

    return segs.Detach();
}

MgCurveString* MgParseAwkt::DoCurveString(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;

    Ptr<MgCurveSegmentCollection> segs = DoCurveSegmentCollection(iContext, doubles);
    Ptr<MgCurveString> geom = factory.CreateCurveString(segs);

    return geom.Detach();
}

MgCurvePolygon* MgParseAwkt::DoCurvePolygon(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgCurveRingCollection> intRings = new MgCurveRingCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    // exterior ring
    Ptr<MgCurveSegmentCollection> extSegs = DoCurveSegmentCollection(iContext, doubles);
    Ptr<MgCurveRing> extRing = factory.CreateCurveRing(extSegs);

    // interior rings
    _types typeSegment;
    while (iContext < m_types->GetCount())
    {
        typeSegment = (_types)(*m_types)[iContext];

        if ((typeSegment != -kCircularArcSegment) && (typeSegment != -kLineStringSegment))
            break;

        (*m_types)[iContext] = -typeSegment;

        Ptr<MgCurveSegmentCollection> intSegs = DoCurveSegmentCollection(iContext, doubles);
        if (intSegs != NULL)
        {
            Ptr<MgCurveRing> intRing = factory.CreateCurveRing(intSegs);
            if (intRing != NULL)
                intRings->Add(intRing);
        }
    }

    Ptr<MgCurvePolygon> geom = factory.CreateCurvePolygon(extRing, intRings);

    return geom.Detach();
}

MgMultiCurveString* MgParseAwkt::DoMultiCurveString(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;

    Ptr<MgCurveStringCollection> curveStrings = new MgCurveStringCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    Ptr<MgCurveString> curveString = (MgCurveString*)DoCurveString(iContext, doubles);
    if (curveString != NULL)
        curveStrings->Add(curveString);

    _types typeSegment;
    while (iContext < m_types->GetCount())
    {
        typeSegment = (_types)(*m_types)[iContext];
        if ((typeSegment != -kCircularArcSegment) && (typeSegment != -kLineStringSegment))
            break;

        (*m_types)[iContext] = -typeSegment;
        Ptr<MgCurveString> curveString1 = DoCurveString(iContext, doubles);
        if (curveString1 != NULL)
            curveStrings->Add(curveString1);
    }

    Ptr<MgMultiCurveString> geom = factory.CreateMultiCurveString(curveStrings);

    return geom.Detach();
}


MgMultiCurvePolygon* MgParseAwkt::DoMultiCurvePolygon(INT32& iContext, double* doubles)
{
    MgGeometryFactory factory;
    Ptr<MgCurvePolygonCollection> polygons = new MgCurvePolygonCollection();

    _dims dim = (_dims) (*m_dims)[iContext];

    Ptr<MgCurvePolygon> polygon = DoCurvePolygon(iContext, doubles);
    if (polygon != NULL)
        polygons->Add(polygon);

    _types typeSegment;
    while (iContext < m_types->GetCount())
    {
        typeSegment = (_types)(*m_types)[iContext];
        if (typeSegment != kRing)
            break;

        // skip ring separator
        iContext++;
        Ptr<MgCurvePolygon> polygon1 = DoCurvePolygon(iContext, doubles);
        if (polygon1 != NULL)
            polygons->Add(polygon1);
    }

    Ptr<MgMultiCurvePolygon> geom = factory.CreateMultiCurvePolygon(polygons);

    return geom.Detach();
}

MgGeometry* MgParseAwkt::DoGeometryCollection(INT32 iContextStart, bool bCollect)
{
    _types type = kNone;

    Ptr<MgGeometry> geom;
    Ptr<MgGeometryCollection> geomCollection;

    // take the value and try to constuct the geometry
    double* doubles = m_values->GetData();
    INT32 countContext = m_types->GetCount();

    if (bCollect)
        geomCollection = new MgGeometryCollection();

    INT32 iCurrent = iContextStart;

    while (iCurrent < countContext)
    {
        type = (_types) (*m_types)[iCurrent];

        if (type == kPoint)
        {
            geom = DoPoint(iCurrent, doubles);
        }
        else if (type == kLineString)
        {
            geom = DoLineString(iCurrent, doubles, kLineString);
        }
        else if (type == kPolygon)
        {
            geom = DoPolygon(iCurrent, doubles, kPolygon);
        }
        else if (type == kMultiPoint)
        {
            geom = DoMultiPoint(iCurrent, doubles);
        }
        else if (type == kMultiLineString)
        {
            geom = DoMultiLineString(iCurrent, doubles);
        }
        else if (type == kMultiPolygon)
        {
            geom = DoMultiPolygon(iCurrent, doubles);
        }
        else if (type == kCurveString)
        {
            geom = DoCurveString(iCurrent, doubles);
        }
        else if (type == kCurvePolygon)
        {
            geom = DoCurvePolygon(iCurrent, doubles);
        }
        else if (type == kMultiCurveString)
        {
            geom = DoMultiCurveString(iCurrent, doubles);
        }
        else if (type == kMultiCurvePolygon)
        {
            geom = DoMultiCurvePolygon(iCurrent, doubles);
        }
        else if (type == kMultiGeometry)
        {
            // recursion time?
            geom = DoGeometryCollection(++iCurrent, true);
        }
        else
        {
            // invalid type
            return NULL;
        }

        if (geom == NULL)
        {
            // some error occurred constructing geometry so give up
            break;
        }

        if (bCollect)
        {
            // add to GisGeometryCollection now
            geomCollection->Add(geom);
        }
    }

    if (bCollect)
    {
        MgGeometryFactory factory;
        Ptr<MgMultiGeometry> multiGeom = factory.CreateMultiGeometry(geomCollection);
        return multiGeom.Detach();
    }

    return geom.Detach();
}

void MgParseAwkt::Done()
{
#ifdef DEBUG_TRACEACTIONS
    printf("Awkt::Done()\n");
    for (int i=0; i<m_types->GetCount(); i++)
    {
        _dims ii = (_dims)(*m_dims)[i];
        printf("context[%d]=%d, %d(%d,%d), %d\n", i, (*m_types)[i],
            ii, DimToCount(ii), DimToDimensionality(ii), (*m_starts)[i]);
    }
    // redundant with SetPoint trace
//  for (int j=0; j<m_values->GetCount(); j++)
//      printf("value[%d]=%g\n", j, (*m_values)[j]);
#endif
    // if we never got any points or geometry type give up
    if (m_values->GetCount() == 0 || m_types->GetCount() == 0 ||
        // also do consistency check before we get started
        m_types->GetCount() != m_dims->GetCount() ||
        m_types->GetCount() != m_starts->GetCount())
    {
        throw new MgInvalidArgumentException(L"MgParseAwkt.ParseAwkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    _types type = (_types) (*m_types)[0];
    bool bMultiGeometry = type == kMultiGeometry;
    INT32 iContextStart = 0;
    if (bMultiGeometry)
        iContextStart++;

    m_geometry = DoGeometryCollection(iContextStart, bMultiGeometry);
}

void MgParseAwkt::Error()
{
#ifdef DEBUG_TRACEACTIONS
    printf("Awkt::Error()\n");
#endif
}

void MgParseAwkt::SetGeometryType(INT32 iToken)
{
#ifdef DEBUG_TRACEACTIONS
    printf("SetGeometryType::");
    m_lex->PrintToken(iToken);
#endif
    switch (iToken)
    {
    case GisToken_POINT:
        m_type = kPoint;
        break;
    case GisToken_LINESTRING:
        m_type = kLineString;
        break;
    case GisToken_POLYGON:
        m_type = kPolygon;
        break;
    case GisToken_CURVESTRING:
        m_type = kCurveString;
        break;
    case GisToken_CURVEPOLYGON:
        m_type = kCurvePolygon;
        break;
    case GisToken_MULTIPOINT:
        m_type = kMultiPoint;
        break;
    case GisToken_MULTILINESTRING:
        m_type = kMultiLineString;
        break;
    case GisToken_MULTIPOLYGON:
        m_type = kMultiPolygon;
        break;
    case GisToken_MULTICURVESTRING:
        m_type = kMultiCurveString;
        break;
    case GisToken_MULTICURVEPOLYGON:
        m_type = kMultiCurvePolygon;
        break;
    case GisToken_GEOMETRYCOLLECTION:
        m_type = kMultiGeometry;
        // special case to force entry in context table
        m_types = GisIntArray::Append(m_types, m_type);
        m_dims = GisIntArray::Append(m_dims, m_dim);
        m_starts = GisIntArray::Append(m_starts, -1);
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(iToken, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgParseAwkt.SetGeometryType",
                __LINE__, __WFILE__, &arguments, L"MgInvalidAgfText", NULL);
        }
        break;
    }
}

void MgParseAwkt::SetDimensionality(INT32 iToken)
{
    switch (iToken)
    {
    case GisToken_XY:
#ifdef DEBUG_TRACEACTIONS
        printf("Set XY\n");
#endif
        m_dim = kXY;
        break;
    case GisToken_XYZ:
#ifdef DEBUG_TRACEACTIONS
        printf("Set XYZ\n");
#endif
        m_dim = kXYZ;
        break;
    case GisToken_XYM:
#ifdef DEBUG_TRACEACTIONS
        printf("Set XYM\n");
#endif
        m_dim = kXYM;
        break;
    case GisToken_XYZM:
#ifdef DEBUG_TRACEACTIONS
        printf("Set XYZM\n");
#endif
        m_dim = kXYZM;
        break;
    default:
#ifdef DEBUG_TRACEACTIONS
        printf("Set XY (default)\n");
#endif
        m_dim = kXY;
        break;
    }
}

void MgParseAwkt::SetBreak()
{
#ifdef DEBUG_TRACEACTIONS
        printf("SetBreak\n");
#endif
    m_break = true;
}

void MgParseAwkt::SetOuterBreak()
{
#ifdef DEBUG_TRACEACTIONS
        printf("SetOuterBreak\n");
#endif
    // force entry in context table
    m_types = GisIntArray::Append(m_types, kRing);
    m_dims = GisIntArray::Append(m_dims, m_dim);
    m_starts = GisIntArray::Append(m_starts, -1);
}

INT32 MgParseAwkt::DimToCount(_dims dim)
{
    switch (dim)
    {
    case kXY:
        return 2;
    case kXYZ:
    case kXYM:
        return 3;
    case kXYZM:
        return 4;
    }
    return 0;
}

INT32 MgParseAwkt::DimToDimensionality(_dims dim)
{
    switch (dim)
    {
    case kXY:
        return 0;
    case kXYZ:
        return 1;
    case kXYM:
        return 2;
    case kXYZM:
        return 3;
    }
    return 0;
}

#ifdef _DEBUG
void MgParseAwkt::Trace(wchar_t* pTrace)
{
#ifdef DEBUG_TRACETREE
    wprintf(pTrace);
#endif
}
#endif

void MgParseAwkt::SetSegmentType(INT32 iToken)
{
    switch (iToken)
    {
    case GisToken_LINESTRINGSEGMENT:
#ifdef DEBUG_TRACEACTIONS
        printf("Set LINESTRINGSEGMENT\n");
#endif
        m_type = kLineStringSegment;
        break;
    case GisToken_CIRCULARARCSEGMENT:
#ifdef DEBUG_TRACEACTIONS
        printf("Set CIRCULARARCSEGMENT\n");
#endif
        m_type = kCircularArcSegment;
        break;
    default:
#ifdef DEBUG_TRACEACTIONS
        printf("Set LINESTRINGSEGMENT (default)\n");
#endif
        m_type = kLineStringSegment;
        break;
    }
}

void MgParseAwkt::AddPoint(double x, double y, double zm, double m)
{
    // register current state along with values
    m_types = GisIntArray::Append(m_types, m_break ? -m_type : m_type);
    m_dims = GisIntArray::Append(m_dims, m_dim);
    m_starts = GisIntArray::Append(m_starts, m_values->GetCount());
    m_break = false;
#ifdef DEBUG_TRACEACTIONS
    INT32   start = m_values->GetCount();
#endif

    double  axyzm[4];
    switch (m_dim)
    {
    case kXY:
#ifdef DEBUG_TRACEACTIONS
        printf("PointXY=%g, %g (%d)\n", x, y, start);
#endif
        axyzm[0] = x;
        axyzm[1] = y;
        m_values = GisDoubleArray::Append(m_values, 2, axyzm);
        break;
    case kXYZ:
#ifdef DEBUG_TRACEACTIONS
        printf("PointXYZ=%g, %g, %g (%d)\n", x, y, zm, start);
#endif
        axyzm[0] = x;
        axyzm[1] = y;
        axyzm[2] = zm;
        m_values = GisDoubleArray::Append(m_values, 3, axyzm);
        break;
    case kXYM:
#ifdef DEBUG_TRACEACTIONS
        printf("PointXYM=%g, %g, %g (%d)\n", x, y, zm, start);
#endif
        axyzm[0] = x;
        axyzm[1] = y;
        axyzm[2] = zm;
        m_values = GisDoubleArray::Append(m_values, 3, axyzm);
        break;
    case kXYZM:
#ifdef DEBUG_TRACEACTIONS
        printf("PointXYZM=%g, %g, %g, %g (%d)\n", x, y, zm, m, start);
#endif
        axyzm[0] = x;
        axyzm[1] = y;
        axyzm[2] = zm;
        axyzm[3] = m;
        m_values = GisDoubleArray::Append(m_values, 4, axyzm);
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(m_dim, buffer);

            MgStringCollection arguments;
            arguments.Add(L"0");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgParseAwkt.ParseAwkt",
                __LINE__, __WFILE__, &arguments, L"MgInvalidAgfText", NULL);
        }
        break;
    }
}

// special lexical analyzer for limited AWKT grammar
INT32 MgParseAwkt::GetLexeme(void* pyylval)
{
    GIS_AWKT_YYSTYPE* yylval = (GIS_AWKT_YYSTYPE*) pyylval;

    INT32 iToken = m_lex->GetToken();
#ifdef DEBUG_TRACETOKEN
    m_lex->PrintToken(iToken);
#endif
    switch (iToken)
    {
    case GisToken_Integer:
        yylval->m_double = (double)m_lex->m_integer;
        return GisToken_DOUBLE;
    case GisToken_Double:
        yylval->m_double = m_lex->m_double;
        return GisToken_DOUBLE;

    case GisToken_LeftParenthesis:
        return '(';
    case GisToken_RightParenthesis:
        return ')';
    case GisToken_Comma:
        return ',';

    // synonyms
    case GisToken_ARC:
        return GisToken_CIRCULARARCSEGMENT;

#ifdef _DEBUG
    case GisToken_End:
    // keywords
    case GisToken_BEZIERSEGMENT:
    case GisToken_CIRCLE:
    case GisToken_CIRCULARARCSEGMENT:
    case GisToken_CURVEPOLYGON:
    case GisToken_CURVESTRING:
    case GisToken_CUSTOM:
    case GisToken_GEOMETRYCOLLECTION:
    case GisToken_GRIDTERRAIN:
    case GisToken_LINESTRING:
    case GisToken_LINESTRINGSEGMENT:
    case GisToken_MULTICURVEPOLYGON:
    case GisToken_MULTICURVESTRING:
    case GisToken_MULTILINESTRING:
    case GisToken_MULTIPOINT:
    case GisToken_MULTIPOLYGON:
    case GisToken_POINT:
    case GisToken_POLYGON:
    case GisToken_RECTANGLE:
    case GisToken_TINTERRAIN:
    case GisToken_XY:
    case GisToken_XYM:
    case GisToken_XYZ:
    case GisToken_XYZM:
        return iToken;
    default:
        printf("!ERROR! Unknown GisToken_=%d\n", iToken);
        return GisToken_Error;
#else
    default:
        return iToken;
#endif // _DEBUG
    }
// unreachable  return GisToken_Error;
}

// use separate lexical analyzer for AWKT
INT32 gis_awkt_yylex(MgParseAwkt* pParse)
{
    // pass in pointer to union to set LLVAL
    return pParse->GetLexeme(&pParse->gis_awkt_yylval);
}
