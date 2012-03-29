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

#include "GeometryCommon.h"

MG_IMPL_DYNCREATE(MgEnvelope)

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an "null" envelope.
//
MgEnvelope::MgEnvelope()
{
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for a region defined by one
// coordinate.
//
MgEnvelope::MgEnvelope(MgCoordinate* coord)
{
    CHECKARGUMENTNULL(coord, L"MgEnvelope.MgEnvelope");

    m_lowerLeft  = SAFE_ADDREF(coord);
    m_upperRight = SAFE_ADDREF(coord);
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for a region defined by two
// coordinates.
//
MgEnvelope::MgEnvelope(MgCoordinate* coord1, MgCoordinate* coord2)
{
    CHECKARGUMENTNULL(coord1, L"MgEnvelope.MgEnvelope");
    CHECKARGUMENTNULL(coord2, L"MgEnvelope.MgEnvelope");

    if (coord1->GetDimension() != coord2->GetDimension())
    {
        STRING buffer;
        MgStringCollection arguments;

        MgUtil::Int32ToString(coord1->GetDimension(), buffer);
        arguments.Add(L"1");
        arguments.Add(buffer);
        MgUtil::Int32ToString(coord2->GetDimension(), buffer);
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgEnvelope.MgEnvelope",
            __LINE__, __WFILE__, &arguments, L"MgCoordinateDimensionDifferent", NULL);
    }

    if (coord1->GetX() < coord2->GetX())
    {
        // coord1 is lower left
        if (coord1->GetY() > coord2->GetY())
        {
            STRING buffer;
            MgStringCollection arguments;

            MgUtil::DoubleToString(coord1->GetY(), buffer);
            arguments.Add(L"1");
            arguments.Add(buffer);
            MgUtil::DoubleToString(coord2->GetY(), buffer);
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgEnvelope.MgEnvelope",
                __LINE__, __WFILE__, &arguments, L"MgInvalidEnvelopeCoordinates", NULL);
        }

        m_lowerLeft  = SAFE_ADDREF(coord1);
        m_upperRight = SAFE_ADDREF(coord2);
    }
    else if (coord1->GetX() > coord2->GetX())
    {
        // coord2 is lower left
        if (coord2->GetY() > coord1->GetY())
        {
            STRING buffer;
            MgStringCollection arguments;

            MgUtil::DoubleToString(coord1->GetY(), buffer);
            arguments.Add(L"1");
            arguments.Add(buffer);
            MgUtil::DoubleToString(coord2->GetY(), buffer);
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgEnvelope.MgEnvelope",
                __LINE__, __WFILE__, &arguments, L"MgInvalidEnvelopeCoordinates", NULL);
        }

        m_lowerLeft  = SAFE_ADDREF(coord2);
        m_upperRight = SAFE_ADDREF(coord1);
    }
    else if (coord1->GetX() == coord2->GetX())
    {
        // coord1 is lower left
        if(coord2->GetY() > coord1->GetY())
        {
            m_lowerLeft  = SAFE_ADDREF(coord1);
            m_upperRight = SAFE_ADDREF(coord2);
        }
        else
        {
            m_lowerLeft  = SAFE_ADDREF(coord2);
            m_upperRight = SAFE_ADDREF(coord1);
        }
    }
}

/// Constructs and intializes an envelope for a region defined by two
/// coordinates, given as four doubles.
MgEnvelope::MgEnvelope(double xMin, double yMin, double xMax, double yMax)
{
    if (xMin > xMax)
    {
        STRING buffer;
        MgStringCollection arguments;

        MgUtil::DoubleToString(xMin, buffer);
        arguments.Add(L"1");
        arguments.Add(buffer);
        MgUtil::DoubleToString(xMax, buffer);
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgEnvelope.MgEnvelope",
            __LINE__, __WFILE__, &arguments, L"MgInvalidEnvelopeCoordinates", NULL);
    }

    if (yMin > yMax)
    {
        STRING buffer;
        MgStringCollection arguments;

        MgUtil::DoubleToString(yMin, buffer);
        arguments.Add(L"1");
        arguments.Add(buffer);
        MgUtil::DoubleToString(yMax, buffer);
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgEnvelope.MgEnvelope",
            __LINE__, __WFILE__, &arguments, L"MgInvalidEnvelopeCoordinates", NULL);
    }

    m_lowerLeft = new MgCoordinateXY(xMin, yMin);
    m_upperRight = new MgCoordinateXY(xMax, yMax);
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope that is a copy of another.
//
MgEnvelope::MgEnvelope(MgEnvelope* envelope)
{
    CHECKARGUMENTNULL(envelope, L"MgEnvelope.MgEnvelope");

    m_lowerLeft  = envelope->GetLowerLeftCoordinate();
    m_upperRight = envelope->GetUpperRightCoordinate();
}

///////////////////////////////////////////////////////////////////////////
// Returns the lower left coordinate of the envelope.
//
MgCoordinate* MgEnvelope::GetLowerLeftCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_lowerLeft);
}

///////////////////////////////////////////////////////////////////////////
// Returns the upper right coordinate of the envelope.
//
MgCoordinate* MgEnvelope::GetUpperRightCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_upperRight);
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum X values.
//
double MgEnvelope::GetWidth()
{
    if (IsNull())
        return DoubleNan;

    return m_upperRight->GetX() - m_lowerLeft->GetX();
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum Y values.
//
double MgEnvelope::GetHeight()
{
    if (IsNull())
        return DoubleNan;

    return m_upperRight->GetY() - m_lowerLeft->GetY();
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum Z values.
//
double MgEnvelope::GetDepth()
{
    // must have a Z dimension
    if (IsNull() || (m_lowerLeft->GetDimension() & MgCoordinateDimension::XYZ) == 0)
        return DoubleNan;

    return fabs(m_lowerLeft->GetZ() - m_upperRight->GetZ());
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this Envelope is "null" or not.
//
bool MgEnvelope::IsNull()
{
    //coordinates are either both NULL or none are NULL
    return m_lowerLeft == NULL;
}

///////////////////////////////////////////////////////////////////////////
// Makes this envelope a "null" envelope.
//
void MgEnvelope::MakeNull()
{
    m_lowerLeft  = NULL;
    m_upperRight = NULL;
}

///////////////////////////////////////////////////////////////////////////
// Enlarges the boundary of the envelope so that it contains the given
// coordinate.
//
void MgEnvelope::ExpandToInclude(MgCoordinate* coordinate)
{
    CHECKARGUMENTNULL(coordinate, L"MgEnvelope.ExpandToInclude");

    if (IsNull())
    {
        m_lowerLeft  = SAFE_ADDREF(coordinate);
        m_upperRight = SAFE_ADDREF(coordinate);
    }
    else
    {
        double x = coordinate->GetX();
        double y = coordinate->GetY();

        double llx = m_lowerLeft->GetX();
        double lly = m_lowerLeft->GetY();
        double urx = m_upperRight->GetX();
        double ury = m_upperRight->GetY();

        llx = (llx > x)? x : llx;
        lly = (lly > y)? y : lly;

        urx = (urx < x)? x : urx;
        ury = (ury < y)? y : ury;

        Ptr<MgCoordinateXY> ll = new MgCoordinateXY(llx, lly);
        if(ll == NULL)
            throw new MgOutOfMemoryException(L"MgEnvelope.ExpandToInclude", __LINE__, __WFILE__, NULL, L"", NULL);

        Ptr<MgCoordinateXY> ur = new MgCoordinateXY(urx, ury);
        if(ur == NULL)
            throw new MgOutOfMemoryException(L"MgEnvelope.ExpandToInclude", __LINE__, __WFILE__, NULL, L"", NULL);

        m_lowerLeft  = SAFE_ADDREF((MgCoordinate*)ll);
        m_upperRight = SAFE_ADDREF((MgCoordinate*)ur);
    }
}

///////////////////////////////////////////////////////////////////////////
// Enlarges the boundary of the envelope so that it contains the given
// envelope.
//
void MgEnvelope::ExpandToInclude(MgEnvelope* envelope)
{
    CHECKARGUMENTNULL(envelope, L"MgEnvelope.ExpandToInclude");

    Ptr<MgCoordinate> ll = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> rr = envelope->GetUpperRightCoordinate();

    ExpandToInclude(ll);
    ExpandToInclude(rr);
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the given coordinate lies in or on this envelope.
//
bool MgEnvelope::Contains(MgCoordinate* coordinate)
{
    CHECKARGUMENTNULL(coordinate, L"MgEnvelope.Contains");

    if (IsNull())
        return false;

    double x = coordinate->GetX();
    double y = coordinate->GetY();

    double llx = m_lowerLeft->GetX();
    double lly = m_lowerLeft->GetY();
    double urx = m_upperRight->GetX();
    double ury = m_upperRight->GetY();

    if(x < llx || x > urx || y < lly || y > ury)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the given envelope lies wholely inside this Envelope
// (inclusive of the boundary).
//
bool MgEnvelope::Contains(MgEnvelope* envelope)
{
    CHECKARGUMENTNULL(envelope, L"MgEnvelope.Contains");

    Ptr<MgCoordinate> ll = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> rr = envelope->GetUpperRightCoordinate();

    return Contains(ll) && Contains(rr);
}

///////////////////////////////////////////////////////////////////////////
// Tests if the region defined by another envelope overlaps (intersects)
// the region of this envelope.
//</returns>
bool MgEnvelope::Intersects(MgEnvelope* envelope)
{
    CHECKARGUMENTNULL(envelope, L"MgEnvelope.Intersects");

    Ptr<MgCoordinate> ll = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> rr = envelope->GetUpperRightCoordinate();

    return Contains(ll) || Contains(rr);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this envelope.
//
MgEnvelope* MgEnvelope::Transform(MgTransform* transform)
{
    return transform->Transform(this);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgEnvelope::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgEnvelope::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgEnvelope::Serialize(MgStream* stream)
{
    // coordinate dimension
    stream->WriteInt32(m_lowerLeft->GetDimension());

    // coordinates
    m_lowerLeft->Serialize(stream);
    m_upperRight->Serialize(stream);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgEnvelope::Deserialize(MgStream* stream)
{
    // coordinate dimension
    INT32 dimension;
    stream->GetInt32(dimension);

    // coordinates
    m_lowerLeft = MgGeometryUtil::ReadCoordinate(stream, dimension);
    m_upperRight = MgGeometryUtil::ReadCoordinate(stream, dimension);
}

MgByteReader* MgEnvelope::ToXml()
{
    // this XML follows the Envelope-1.0.0.xsd schema
    string str;
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";    // NOXLATE
    str += "<Envelope>";    // NOXLATE
    this->ToXml(str);
    str += "</Envelope>";   // NOXLATE

    STRING mimeType = MgMimeType::Xml;
    return MgUtil::GetByteReader(str, &mimeType);
}

void MgEnvelope::ToXml(string& str)
{
    str += "<LowerLeftCoordinate>";     // NOXLATE
    m_lowerLeft->ToXml(str);
    str += "</LowerLeftCoordinate>";    // NOXLATE

    str += "<UpperRightCoordinate>";    // NOXLATE
    m_upperRight->ToXml(str);
    str += "</UpperRightCoordinate>";   // NOXLATE
}

void MgEnvelope::Grow(double offset)
{
    if ( offset > 0.0 && !IsNull() )
    {
        double minX = m_lowerLeft->GetX();
        double minY = m_lowerLeft->GetY();

        double maxX = m_upperRight->GetX();
        double maxY = m_upperRight->GetY();

        minX -= offset;
        minY -= offset;
        maxX += offset;
        maxY += offset;

        Ptr<MgCoordinateXY> ll = new MgCoordinateXY(minX, minY);
        if (ll == NULL)
            throw new MgOutOfMemoryException(L"MgEnvelope.Grow", __LINE__, __WFILE__, NULL, L"", NULL);

        Ptr<MgCoordinateXY> ur = new MgCoordinateXY(maxX, maxY);
        if (ur == NULL)
            throw new MgOutOfMemoryException(L"MgEnvelope.Grow", __LINE__, __WFILE__, NULL, L"", NULL);

        m_lowerLeft  = SAFE_ADDREF((MgCoordinate*)ll);
        m_upperRight = SAFE_ADDREF((MgCoordinate*)ur);
    }
}
