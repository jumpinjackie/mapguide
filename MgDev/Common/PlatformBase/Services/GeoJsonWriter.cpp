#include "PlatformBase.h"
#include <algorithm>
#include <iterator>
#include <sstream>

MgGeoJsonWriter::MgGeoJsonWriter()
{
    m_agfRw = new MgAgfReaderWriter();
    m_precisionEnabled = false;
    m_precision = 7;
}

INT32 MgGeoJsonWriter::GetPrecision() { return m_precision; }

void MgGeoJsonWriter::SetPrecision(INT32 precision) { m_precision = precision; }

STRING MgGeoJsonWriter::FeatureToGeoJson(MgFeatureReader* featureReader, MgTransform* transform)
{
    Ptr<MgClassDefinition> clsDef = featureReader->GetClassDefinition();
    Ptr<MgPropertyDefinitionCollection> idProps = clsDef->GetIdentityProperties();
    if (idProps->GetCount() == 1)
    {
        INT32 idx = 0;
        Ptr<MgPropertyDefinition> idProp = idProps->GetItem(idx);
        return FeatureToGeoJson(featureReader, transform, idProp->GetName(), clsDef->GetDefaultGeometryPropertyName());
    }
    else
    {
        return FeatureToGeoJson(featureReader, transform, L"", L"");
    }
}

STRING MgGeoJsonWriter::FeatureToGeoJson(MgReader* reader, MgTransform* transform, CREFSTRING idPropertyName, CREFSTRING geomPropName)
{
    STRING ret;
    INT32 idIndex = -1;
    INT32 gIndex = -1;
    STRING idVal;
    STRING geomVal;

    std::vector<STRING> propVals;

    MG_TRY()

    CHECKARGUMENTNULL(reader, L"MgGeoJsonWriter.FeatureToGeoJson");

    if (!idPropertyName.empty())
        idIndex = reader->GetPropertyIndex(idPropertyName);
    if (!geomPropName.empty())
    {
        gIndex = reader->GetPropertyIndex(geomPropName);
        if (gIndex >= 0)
        {
            //Must check out as a geometry property
            INT32 propType = reader->GetPropertyType(gIndex);
            if (propType != MgPropertyType::Geometry)
                gIndex = -1;
        }
    }

    //Pre-load id and geometry values first
    if (idIndex >= 0 && !reader->IsNull(idIndex))
    {
        ValueToString(reader, idIndex, idVal);
    }
    if (gIndex >= 0 && !reader->IsNull(gIndex))
    {
        try
        {
            Ptr<MgByteReader> agf = reader->GetGeometry(gIndex);
            Ptr<MgGeometry> geom;
            if (NULL != transform)
                geom = m_agfRw->Read(agf, transform);
            else
                geom = m_agfRw->Read(agf);
            
            Ptr<MgGeometry> g = geom->Tessellate();
            ToGeoJson(g, geomVal);
        }
        catch (MgException* ex)
        {
            SAFE_RELEASE(ex);
            geomVal = L"\"geometry\": null";
        }
    }

    //Iterate the other properties, skipping over the geometry and id ones if they have been read
    INT32 propCount = reader->GetPropertyCount();
    for (INT32 i = 0; i < propCount; i++)
    {
        STRING name = reader->GetPropertyName(i);
        INT32 propType = reader->GetPropertyType(i);
        if (!reader->IsNull(i))
        {
            if (idIndex != i && gIndex != i) //Non id/Non geom property
            {
                //GeoJSON only allows one geometry
                if (propType == MgPropertyType::Geometry && geomVal.empty())
                {
                    try
                    {
                        Ptr<MgByteReader> agf = reader->GetGeometry(i);
                        Ptr<MgGeometry> geom;
                        if (NULL != transform)
                            geom = m_agfRw->Read(agf, transform);
                        else
                            geom = m_agfRw->Read(agf);

                        Ptr<MgGeometry> g = geom->Tessellate();
                        ToGeoJson(g, geomVal);
                    }
                    catch (MgException* ex)
                    {
                        SAFE_RELEASE(ex);
                        geomVal = L"\"geometry\": null";
                    }
                }
                else
                {
                    STRING propFragment = L"\"";
                    propFragment += name;
                    propFragment += L"\": ";

                    STRING val;
                    ValueToString(reader, i, val);
                    propFragment += val;

                    propVals.push_back(propFragment);
                }
            }
        }
        else //null property
        {
            STRING propFragment = L"\"";
            propFragment += name;
            propFragment += L"\": null";

            propVals.push_back(propFragment);
        }
    }

    ret = L"{ \"type\": \"Feature\", "; //START FEATURE
    if (!idVal.empty())
    {
        ret += L"\"id\": ";
        ret += idVal;
        ret += L", ";
    }
    if (!geomVal.empty())
    {
        ret += geomVal;
        ret += L", ";
    }
    ret += L"\"properties\": {"; //START PROPERTIES
    switch (propVals.size())
    {
    case 0:
        break;
    case 1:
        ret += propVals[0];
        break;
    default:
        //Implode the vector to a comma-delimited string of "key": <value> pairs
        std::wostringstream os;
        std::ostream_iterator<STRING, wchar_t> outIt(os, L",");
        std::copy(propVals.begin(), propVals.end() - 1, outIt);
        os << *propVals.rbegin();
        ret += os.str();
        break;
    }
    ret += L"}"; //END PROPERTIES
    ret += L"}"; //END FEATURE

    MG_CATCH_AND_THROW(L"MgGeoJsonWriter.FeatureToGeoJson")

    return ret;
}

STRING MgGeoJsonWriter::GeometryToGeoJson(MgGeometry* geom)
{
    STRING ret;
    MG_TRY()

    ret = L"{ \"type\": \"Feature\", "; //START FEATURE
    ret += L"\"properties\": {}, "; //PROPERTIES
    
    STRING geomVal;
    Ptr<MgGeometry> g = geom->Tessellate();
    ToGeoJson(g, geomVal);
    ret += geomVal; //GEOMETRY

    ret += L"}"; //END FEATURE

    MG_CATCH_AND_THROW(L"MgGeoJsonWriter.GeometryToGeoJson")
    return ret;
}

void MgGeoJsonWriter::ValueToString(MgReader* reader, INT32 index, REFSTRING str)
{
    INT32 propType = reader->GetPropertyType(index);
    switch (propType)
    {
    case MgPropertyType::Boolean:
        {
            str = reader->GetBoolean(index) ? L"true" : L"false";
        }
        break;
    case MgPropertyType::Byte:
        {
            MgUtil::Int32ToString(reader->GetByte(index), str);
        }
        break;
    case MgPropertyType::DateTime:
        {
            Ptr<MgDateTime> dt = reader->GetDateTime(index);
            str = L"\"";
            str += dt->ToString();
            str += L"\"";
        }
        break;
    case MgPropertyType::Decimal:
    case MgPropertyType::Double:
        {
            double val = reader->GetDouble(index);
            //NOTE: Not applying precision here as that is for coordinate output
            MgUtil::DoubleToString(val, str);
        }
        break;
    case MgPropertyType::Int16:
        {
            INT16 val = reader->GetInt16(index);
            MgUtil::Int32ToString(val, str);
        }
        break;
    case MgPropertyType::Int32:
        {
            INT32 val = reader->GetInt32(index);
            MgUtil::Int32ToString(val, str);
        }
        break;
    case MgPropertyType::Int64:
        {
            INT64 val = reader->GetInt64(index);
            MgUtil::Int64ToString(val, str);
        }
        break;
    case MgPropertyType::Single:
        {
            float val = reader->GetSingle(index);
            MgUtil::SingleToString(val, str);
        }
        break;
    case MgPropertyType::String:
        {
            str = L"\"";
            STRING strVal = EscapeJsonString(reader->GetString(index));
            str += strVal;
            str += L"\"";
        }
        break;
    }
}

STRING MgGeoJsonWriter::EscapeJsonString(CREFSTRING input)
{
    std::wostringstream ss;
    for (STRING::const_iterator iter = input.begin(); iter != input.end(); iter++)
    {
        switch (*iter)
        {
            case '\\': ss << L"\\\\"; break;
            case '"': ss << L"\\\""; break;
            case '/': ss << L"\\/"; break;
            case '\b': ss << L"\\b"; break;
            case '\f': ss << L"\\f"; break;
            case '\n': ss << L"\\n"; break;
            case '\r': ss << L"\\r"; break;
            case '\t': ss << L"\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

void MgGeoJsonWriter::ToGeoJson(MgGeometry* geom, REFSTRING str, bool bIncludePrefix)
{
    STRING prefix;
    if (bIncludePrefix)
        prefix = L"\"geometry\": ";

    INT32 geomType = geom->GetGeometryType();
    switch (geomType)
    {
    case MgGeometryType::Point:
        {
            MgPoint* pt = (MgPoint*)geom;
            Ptr<MgCoordinate> coord = pt->GetCoordinate();
            str += prefix;
            str += L"{ \"type\": \"Point\", \"coordinates\": ";
            STRING strCoords;
            CoordinateToGeoJson(coord, strCoords);
            str += strCoords;
            str += L"}";
        }
        break;
    case MgGeometryType::LineString:
        {
            str += prefix;
            str += L"{ \"type\": \"LineString\", \"coordinates\": ";
            MgLineString* line = (MgLineString*)geom;
            Ptr<MgCoordinateIterator> coords = line->GetCoordinates();
            STRING strCoords;
            CoordinatesToGeoJson(coords, strCoords);
            str += strCoords;
            str += L"}";
        }
        break;
    case MgGeometryType::Polygon:
        {
            str += prefix;
            str += L"{ \"type\": \"Polygon\", \"coordinates\": ";
            MgPolygon* poly = (MgPolygon*)geom;
            STRING strPoly;
            PolygonToGeoJson(poly, strPoly);
            str += strPoly;
            str += L"}";
        }
        break;
    case MgGeometryType::MultiPoint:
        {
            str += prefix;
            str += L"{ \"type\": \"MultiPoint\", \"coordinates\": ";
            MgMultiPoint* multi = (MgMultiPoint*)geom;
            STRING strMulti;
            MultiPointToGeoJson(multi, strMulti);
            str += strMulti;
            str += L"}";
        }
        break;
    case MgGeometryType::MultiLineString:
        {
            str += prefix;
            str += L"{ \"type\": \"MultiLineString\", \"coordinates\": ";
            MgMultiLineString* multi = (MgMultiLineString*)geom;
            STRING strMulti;
            MultiLineStringToGeoJson(multi, strMulti);
            str += strMulti;
            str += L"}";
        }
        break;
    case MgGeometryType::MultiPolygon:
        {
            str += prefix;
            str += L"{ \"type\": \"MultiPolygon\", \"coordinates\": ";
            MgMultiPolygon* multi = (MgMultiPolygon*)geom;
            STRING strMulti;
            MultiPolygonToGeoJson(multi, strMulti);
            str += strMulti;
            str += L"}";
        }
        break;
    case MgGeometryType::MultiGeometry:
        {
            str += prefix;
            str += L"{ \"type\": \"GeometryCollection\", \"geometries\": [";
            MgMultiGeometry* multi = (MgMultiGeometry*)geom;
            bool bFirst = true;
            INT32 count = multi->GetCount();
            for (INT32 i = 0; i < count; i++)
            {
                if (!bFirst)
                    str += L",";

                Ptr<MgGeometry> g = multi->GetGeometry(i);
                STRING strGeom;
                ToGeoJson(g, strGeom, false);

                bFirst = false;
            }
            str += L"]";
            str += L"}";
        }
        break;
    default:
        str = L"\"geometry\": null";
        break;
    }
}

void MgGeoJsonWriter::CoordinateToGeoJson(MgCoordinate* coord, REFSTRING str)
{
    str = L"[";
    STRING sx;
    STRING sy;
    if (m_precisionEnabled)
    {
        MgUtil::DoubleToString(coord->GetX(), sx, m_precision);
        MgUtil::DoubleToString(coord->GetY(), sy, m_precision);
    }
    else
    {
        MgUtil::DoubleToString(coord->GetX(), sx);
        MgUtil::DoubleToString(coord->GetY(), sy);
    }
    str += sx;
    str += L", ";
    str += sy;
    str += L"]";
}

void MgGeoJsonWriter::CoordinatesToGeoJson(MgCoordinateIterator* coords, REFSTRING str)
{
    str = L"[";
    bool bFirst = true;
    while (coords->MoveNext())
    {
        if (!bFirst)
            str += L",";

        Ptr<MgCoordinate> coord = coords->GetCurrent();

        str += L"[";
        STRING sx;
        STRING sy;
        if (m_precisionEnabled)
        {
            MgUtil::DoubleToString(coord->GetX(), sx, m_precision);
            MgUtil::DoubleToString(coord->GetY(), sy, m_precision);
        }
        else 
        {
            MgUtil::DoubleToString(coord->GetX(), sx);
            MgUtil::DoubleToString(coord->GetY(), sy);
        }
        str += sx;
        str += L", ";
        str += sy;
        str += L"]";

        bFirst = false;
    }
    str += L"]";
}

void MgGeoJsonWriter::PolygonToGeoJson(MgPolygon* poly, REFSTRING str)
{
    str = L"[";

    Ptr<MgRing> extRing = poly->GetExteriorRing();
    if (NULL != extRing.p)
    {
        Ptr<MgCoordinateIterator> coords = extRing->GetCoordinates();
        STRING strCoords;
        CoordinatesToGeoJson(coords, strCoords);
        str += strCoords;
    }
    INT32 irCount = poly->GetInteriorRingCount();
    if (irCount > 0)
    {
        if (NULL != extRing.p)
        {
            str += L",";
        }
        for (INT32 i = 0; i < irCount; i++)
        {
            Ptr<MgRing> ir = poly->GetInteriorRing(i);
            Ptr<MgCoordinateIterator> coords = ir->GetCoordinates();
            STRING strCoords;
            CoordinatesToGeoJson(coords, strCoords);
            str += strCoords;
            if (i < irCount - 1)
            {
                str += L",";
            }
        }
    }
    str += L"]";
}

void MgGeoJsonWriter::MultiPointToGeoJson(MgMultiPoint* multi, REFSTRING str)
{
    str = L"[";

    bool bFirst = true;
    INT32 count = multi->GetCount();
    for (INT32 i = 0; i < count; i++)
    {
        if (!bFirst)
            str += L",";

        Ptr<MgPoint> point = multi->GetPoint(i);
        Ptr<MgCoordinate> coord = point->GetCoordinate();
        STRING coordStr;
        CoordinateToGeoJson(coord, coordStr);

        str += coordStr;

        bFirst = false;
    }

    str += L"]";
}

void MgGeoJsonWriter::MultiLineStringToGeoJson(MgMultiLineString* multi, REFSTRING str)
{
    str = L"[";

    bool bFirst = true;
    INT32 count = multi->GetCount();
    for (INT32 i = 0; i < count; i++)
    {
        if (!bFirst)
            str += L",";

        Ptr<MgLineString> line = multi->GetLineString(i);
        Ptr<MgCoordinateIterator> coords = line->GetCoordinates();
        STRING coordsStr;
        CoordinatesToGeoJson(coords, coordsStr);

        str += coordsStr;

        bFirst = false;
    }

    str += L"]";
}

void MgGeoJsonWriter::MultiPolygonToGeoJson(MgMultiPolygon* multi, REFSTRING str)
{
    str = L"[";

    bool bFirst = true;
    INT32 count = multi->GetCount();
    for (INT32 i = 0; i < count; i++)
    {
        if (!bFirst)
            str += L",";

        Ptr<MgPolygon> poly = multi->GetPolygon(i);
        STRING strPoly;
        
        PolygonToGeoJson(poly, strPoly);

        str += strPoly;

        bFirst = false;
    }

    str += L"]";
}

void MgGeoJsonWriter::Dispose()
{
    delete this;
}

void MgGeoJsonWriter::SetPrecisionEnabled(bool enabled)
{
    m_precisionEnabled = enabled;
}

bool MgGeoJsonWriter::IsPrecisionEnabled()
{
    return m_precisionEnabled;
}