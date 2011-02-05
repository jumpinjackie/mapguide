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

#include "stdafx.h"
#include "BandData.h"
#include "Band.h"
#include "GridData.h"
#include "MathHelper.h"
#include "LineBuffer.h"
#include "Color.h"
#include "GeometryAlgorithms.h"

//*************************************************************************************************************
Band::Band(BandDataType dataType, GridData* pOwnerGrid):
                        m_dataType(dataType), m_pOwnerGrid(pOwnerGrid)
{
    assert( NULL != pOwnerGrid );

    m_nXCount = pOwnerGrid->GetXCount();
    m_nYCount = pOwnerGrid->GetYCount();
    m_nullValue = *(INT64*)&DBL_NAN;

    m_pBandData = CreateDataGrid(dataType, GetXCount(), GetYCount());

    m_minz = +DBL_MAX;
    m_maxz = -DBL_MAX;

    SetDataChangedFlag();
}

//*************************************************************************************************************

Band::Band(BandDataType dataType, const Point2D& point, double xExtent, double yExtent,
                 unsigned int nXCount, unsigned int nYCount):
                    m_dataType(dataType), m_pOwnerGrid(NULL),
                    m_westSourthPoint(point), m_xExtent (xExtent), m_yExtent (yExtent),
                    m_nXCount (nXCount), m_nYCount (nYCount)
{
    assert( NULL == m_pOwnerGrid );
    assert( 0 != nXCount);
    assert( 0 != nYCount);

    m_nullValue = *(INT64*)&DBL_NAN;

    m_pBandData = CreateDataGrid(dataType, GetXCount(), GetYCount());

    m_minz = +DBL_MAX;
    m_maxz = -DBL_MAX;

    SetDataChangedFlag();
}

//*************************************************************************************************************

Band::~Band()
{
    delete m_pBandData;
}

//*************************************************************************************************************

BandData* Band::CreateDataGrid(BandDataType dataType, unsigned int width, unsigned int height)
{
    switch (dataType)
    {
    case UnsignedInt8:
    case Int8:
        return new ByteBandData(1, width, height);

    case UnsignedInt16:
    case Int16:
        return new ByteBandData(2, width, height);

    case UnsignedInt32:
    case Int32:
    case Double32:
        return new ByteBandData(4, width, height);

    case UnsignedInt64:
    case Int64:
    case Double64:
        return new ByteBandData(8, width, height);

    case Boolean:
        return new BitBandData(1, width, height);

    case Bit2:
        return new BitBandData(2, width, height);

    case Bit4:
        return new BitBandData(4, width, height);
    }

    return NULL;
}


//*************************************************************************************************************

bool Band::GetValue(unsigned int i, unsigned int j, BandDataType datatype, void* pValue) const
{
    bool success = false;

    switch(datatype)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        {
            success = m_pBandData->GetValue(i, j, *reinterpret_cast<INT8*>(pValue));
        }
        break;

    case UnsignedInt16:
    case Int16:
        {
            success = m_pBandData->GetValue(i, j, *reinterpret_cast<INT16*>(pValue));
        }
        break;

#ifdef _WIN32
    case UnsignedInt32:
    case Int32:
        {
            assert (Int32 == m_dataType || UnsignedInt32 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->GetNativeInt32(i, j, *reinterpret_cast<INT32*>(pValue));
        }
        break;

    case Double32:
        {
            assert (Double32 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->GetNativeFloat(i, j, *reinterpret_cast<float*>(pValue));
        }
        break;

    case UnsignedInt64:
    case Int64:
        {
            assert (Int64 == m_dataType || UnsignedInt64 == m_dataType);
            success = m_pBandData->GetValue(i, j, *reinterpret_cast<INT64*>(pValue));
        }
        break;
    case Double64:
        {
            assert (Double64 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->GetNativeDouble(i, j, *reinterpret_cast<double*>(pValue));
        }
        break;
#else
        // Linux implementation here
#endif


    default:
        break;
    }

    return success;
}

//*************************************************************************************************************

bool Band::SetValue(unsigned int i, unsigned int j, BandDataType datatype, void* pValue)
{
    bool success = false;

    switch(datatype)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        {
            success = m_pBandData->SetValue(i, j, reinterpret_cast<INT8*>(pValue));
        }
        break;

    case UnsignedInt16:
    case Int16:
        {
            success = m_pBandData->SetValue(i, j, reinterpret_cast<INT16*>(pValue));
        }
        break;

#ifdef _WIN32
    case UnsignedInt32:
    case Int32:
        {
            assert (Int32 == m_dataType || UnsignedInt32 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->SetNativeInt32(i, j, *reinterpret_cast<INT32*>(pValue));
        }
        break;

    case Double32:
        {
            assert (Double32 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->SetNativeFloat(i, j, *reinterpret_cast<float*>(pValue));
        }
        break;

    case UnsignedInt64:
    case Int64:
        {
            assert (Int64 == m_dataType || UnsignedInt64 == m_dataType);
            success = m_pBandData->SetValue(i, j, reinterpret_cast<INT64*>(pValue));
        }
        break;
    case Double64:
        {
            assert (Double64 == m_dataType);
            success = ((ByteBandData*)m_pBandData)->SetNativeDouble(i, j, *reinterpret_cast<double*>(pValue));
        }
        break;
#else
        // Linux implementation
#endif


    default:
        break;
    }

    if (success)
    {
        SetDataChangedFlag();
        return true;
    }
    return false;
}

//*************************************************************************************************************

bool Band::GetValueAsDouble(unsigned int i, unsigned int j, double& dvalue) const
{
    bool success = false;

    switch(m_dataType)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        {
            INT8 val;
            success = m_pBandData->GetValue(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 1) != 0); //ensure it is not equal to the null value
            dvalue = (double)val;
        }
        break;

    case UnsignedInt16:
        {
            INT16 val;
            success = m_pBandData->GetValue(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 2) != 0); //ensure it is not equal to the null value
            dvalue = (double)(UINT16)val;
        }
        break;
    case Int16:
        {
            INT16 val;
            success = m_pBandData->GetValue(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 2) != 0); //ensure it is not equal to the null value
            dvalue = (double)val;
        }
        break;

#ifdef _WIN32
    case UnsignedInt32:
        {
            int val;
            success = ((ByteBandData*)m_pBandData)->GetNativeInt32(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 4) != 0); //ensure it is not equal to the null value
            dvalue = (double)(unsigned int)val;
        }
        break;

    case Int32:
        {
            int val;
            success = ((ByteBandData*)m_pBandData)->GetNativeInt32(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 4) != 0); //ensure it is not equal to the null value
            dvalue = (double)val;
        }
        break;

    case Double32:
        {
            float val;
            success = ((ByteBandData*)m_pBandData)->GetNativeFloat(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 4) != 0); //ensure it is not equal to the null value
            dvalue = (double) val;
        }
        break;

    case UnsignedInt64:
        {
            INT64 val;
            success = m_pBandData->GetValue(i, j, val);
            success = success && memcmp(&val, &m_nullValue, 8) != 0; //ensure it is not equal to the null value
            dvalue = (double)(UINT64)val;
        }
        break;

    case Int64:
        {
            INT64 val;
            success = m_pBandData->GetValue(i, j, val);
            success = success && (memcmp(&val, &m_nullValue, 8) != 0); //ensure it is not equal to the null value
            dvalue = (double)val;
        }
        break;

    case Double64:
        {
            success = ((ByteBandData*)m_pBandData)->GetNativeDouble(i, j, dvalue);
            success = success && (memcmp(&dvalue, &m_nullValue, 8) != 0); //ensure it is not equal to the null value
        }
        break;
#else
    // Linux implementation
#endif

    default:
        break;
    }

    return success;
}

//*************************************************************************************************************

double Band::GetValueAsDouble (unsigned int i, unsigned int j) const
{
    double dValue;
    if (GetValueAsDouble(i, j, dValue))
    {
        return dValue;
    }

    return DBL_NAN;
}

//*************************************************************************************************************

bool Band::GetNormal(unsigned int i, unsigned int j, Vector3D& normal, double scale) const
{

    //                . top
    //
    //        .left    . current .right
    //
    //                . below

    //get given point elevation and the 4 closest points on the grid
    double currentz, topz, belowz, leftz, rightz;
    if (false == GetNearByDoubleValues(i, j, currentz, topz, belowz, leftz, rightz))
    {
        return false;
    }

    if (CompareDoubles(scale, 1) != 0)
    {
        currentz *= scale;
        topz     *= scale;
        belowz   *= scale;
        leftz    *= scale;
        rightz   *= scale;
    }

    // Initialize Vector
    GridData* pOwner = GetOwnerGrid();

    double dThisVecX = (leftz - rightz) * pOwner->GetInvScaledDX() * 0.5;
    double dThisVecY = (belowz - topz) * pOwner->GetInvScaledDY() * 0.5;

    double dThisVecZ = 1;

    // Normalize
    double idThisLen = 1.0 / sqrt(dThisVecX * dThisVecX + dThisVecY * dThisVecY + dThisVecZ * dThisVecZ);

    dThisVecX *= idThisLen;
    dThisVecY *= idThisLen;
    dThisVecZ *= idThisLen;

    // Set to result
    normal.x = dThisVecX;
    normal.y = dThisVecY;
    normal.z = dThisVecZ;

    return true;
}

const Band* Band::GetAspectBand() const
{
    bool bCalculate = (NULL == m_spAspectBand.get());
    if (NULL == m_spAspectBand.get())
    {
        m_spAspectBand.reset(new Band(Band::Double32, m_pOwnerGrid));
    }
    if (m_bDataChanged)
    {
        CalculatedMinAndMax();
        bCalculate = true;
    }
    if (bCalculate)
    {
        unsigned int width = GetXCount();
        unsigned int height = GetYCount();
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                double center, top, bottom, left, right;
                if (GetNearByDoubleValues(x, y, center, top, bottom, left, right))
                {
                    float fAspect = static_cast<float>(GeometryAlgorithms::CalculateAspect(
                        center,
                        top,
                        bottom,
                        left,
                        right,
                        GetXUnitDistance() * this->GetOwnerGrid()->GetCoordSysUnitLength(),
                        GetYUnitDistance() * this->GetOwnerGrid()->GetCoordSysUnitLength()));
                    m_spAspectBand->SetValue(x, y, Band::Double32, &fAspect);
                }
                else
                {
                    m_spAspectBand->SetValue(x, y, Band::Double32, (void*)&FLT_NAN);
                }
            }
        }
    }
    return m_spAspectBand.get();
}

const Band* Band::GetSlopeBand() const
{
    bool bCalculate = (NULL == m_spSlopeBand.get());
    if (NULL == m_spSlopeBand.get())
    {
        m_spSlopeBand.reset(new Band(Band::Double32, m_pOwnerGrid));
    }
    if (m_bDataChanged)
    {
        CalculatedMinAndMax();
        bCalculate = true;
    }
    if (bCalculate)
    {
        unsigned int width = GetXCount();
        unsigned int height = GetYCount();
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                double center, top, bottom, left, right;
                if (GetNearByDoubleValues(x, y, center, top, bottom, left, right))
                {
                    float fSlope = static_cast<float>(GeometryAlgorithms::CalculateSlope(
                        center,
                        top,
                        bottom,
                        left,
                        right,
                        GetXUnitDistance() * this->GetOwnerGrid()->GetCoordSysUnitLength(),
                        GetYUnitDistance() * this->GetOwnerGrid()->GetCoordSysUnitLength()));
                    m_spSlopeBand->SetValue(x, y, Band::Double32, &fSlope);
                }
                else
                {
                    m_spSlopeBand->SetValue(x, y, Band::Double32, (void*)&FLT_NAN);
                }
            }
        }
    }
    return m_spSlopeBand.get();
}

//*************************************************************************************************************

double Band::GetMinZ() const
{
    if (m_bDataChanged)
    {
        CalculatedMinAndMax();
    }

    return m_minz;
}

double Band::GetMaxZ() const
{
    if (m_bDataChanged)
    {
        CalculatedMinAndMax();
    }

    return m_maxz;
}

void Band::CalculatedMinAndMax() const
{
    m_minz = +DBL_MAX;
    m_maxz = -DBL_MAX;

    for (unsigned int i=0; i<GetXCount(); ++i)
    {
        for (unsigned int j=0; j<GetYCount(); ++j)
        {
            double dValue;
            bool ret = GetValueAsDouble(i,j,dValue);
            if (!ret)
            {
                continue;
            }

            if (dValue < m_minz)
            {
                m_minz = dValue;
            }

            if (dValue > m_maxz)
            {
                m_maxz = dValue;
            }
        }
    }

    m_bDataChanged = false;
}

//*************************************************************************************************************

void Band::PosToIndex(double x, double y, int&i, int& j) const
{
    i = Double2Int((x - GetOriginalPoint2D().x ) / GetXUnitDistance() );
    j = Double2Int(GetYCount() - 1 - (y - GetOriginalPoint2D().y ) / GetYUnitDistance() );
}

//*************************************************************************************************************

void Band::IndexToPos(unsigned int i, unsigned int j, double& x, double& y)const
{
    x = GetOriginalPoint2D().x + i * GetXUnitDistance();
    y = GetOriginalPoint2D().y + (GetYCount() - 1 - j) * GetYUnitDistance();
}


UINT32 Band::GetBilinearColor(const Point2D& point) const
{
    if ( Band::UnsignedInt32 != GetDataType())
    {
        return (UINT32)-1;
    }

    double di = (point.x - GetOriginalPoint2D().x ) / GetXUnitDistance();
    double dj = GetYCount() - 1 - (point.y - GetOriginalPoint2D().y ) / GetYUnitDistance();

    if ( (0 == CompareDoubles( di, Double2Int(di) ) )
        &&(0 == CompareDoubles( dj, Double2Int(dj) ) ) )
    {
        UINT32 color;
        if (GetValue(Double2Int(di), Double2Int(dj), UnsignedInt32, &color))
        {
            return color;
        }
        else
        {
            return (UINT32)-1;
        }
    }

    //otherwise interpolate

    //four grid points of this cell
    int i0 = Double2Int(floor(di));
    int i1 = Double2Int(ceil(di));

    int j0 = Double2Int(floor(dj));
    int j1 = Double2Int(ceil(dj));

    double t = di - i0;
    double u = dj - j0;

    UINT32 z[4];

    int invalid = 0;
    if (!GetValue(i0, j0, UnsignedInt32, &z[0]))
    {
        ++invalid;
        z[0] = 0;
    }
    if (!GetValue(i1, j0, UnsignedInt32, &z[1]))
    {
        ++invalid;
        z[1] = 0;
    }
    if (!GetValue(i1, j1, UnsignedInt32, &z[2]))
    {
        ++invalid;
        z[2] = 0;
    }
    if (!GetValue(i0, j1, UnsignedInt32, &z[3]))
    {
        ++invalid;
        z[3] = 0;
    }

    if (0 < invalid)
        return (UINT32)-1;

    Color z0(z[0]);
    Color z1(z[1]);
    Color z2(z[2]);
    Color z3(z[3]);

    //standard bilinear interpolation on a square cell
    Color color((unsigned char)((1-t)*(1-u)*z0.GetA() + t*(1-u)*z1.GetA() + t*u*z2.GetA() + (1-t)*u*z3.GetA()) ,
                (unsigned char)((1-t)*(1-u)*z0.GetR() + t*(1-u)*z1.GetR() + t*u*z2.GetR() + (1-t)*u*z3.GetR()) ,
                (unsigned char)((1-t)*(1-u)*z0.GetG() + t*(1-u)*z1.GetG() + t*u*z2.GetG() + (1-t)*u*z3.GetG()) ,
                (unsigned char)((1-t)*(1-u)*z0.GetB() + t*(1-u)*z1.GetB() + t*u*z2.GetB() + (1-t)*u*z3.GetB()) );

    return color.GetARGB();
}

//*************************************************************************************************************

UINT32 Band::GetColorValue(unsigned int i, unsigned int j) const
{
    if (UnsignedInt32 != m_dataType)
        return (UINT32)-1;

    unsigned int ColorValue;
    if (!GetValue(i, j, UnsignedInt32, &ColorValue))
        return (UINT32)-1;

    return ColorValue;
}

//*************************************************************************************************************

double Band::GetXUnitDistance() const
{
    if (NULL != m_pOwnerGrid)
    {
        return m_pOwnerGrid->GetXUnitDistance();
    }

    return m_nXCount ? m_xExtent / m_nXCount : 0;
}

//*************************************************************************************************************

double Band::GetYUnitDistance() const
{
    if (NULL != m_pOwnerGrid)
    {
        return m_pOwnerGrid->GetYUnitDistance();
    }

    return m_nYCount ? m_yExtent / m_nYCount : 0;
}

//*************************************************************************************************************

double Band::GetXExtent () const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetXExtent() : m_xExtent;
}

//*************************************************************************************************************

double Band::GetYExtent () const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetYExtent() : m_yExtent;
}

//*************************************************************************************************************

const Point2D& Band::GetOriginalPoint2D() const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetOriginalPoint2D() : m_westSourthPoint;
}

//*************************************************************************************************************

MdfModel::Box2D Band::GetBoundingBox() const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetBoundingBox() : MdfModel::Box2D(   m_westSourthPoint.x,
                                                                                        m_westSourthPoint.y,
                                                                                        m_westSourthPoint.x + m_xExtent,
                                                                                        m_westSourthPoint.y + m_yExtent );
}


//*************************************************************************************************************

bool Band::HasSamePlacement (const Band& band) const
{
    return GetOriginalPoint2D() == band.GetOriginalPoint2D()
                && GetXExtent() == band.GetXExtent()
                && GetYExtent() == band.GetYExtent()
                && GetXUnitDistance() == band.GetXUnitDistance()
                && GetYUnitDistance() == band.GetYUnitDistance()
                && GetXCount() == band.GetXCount ()
                && GetYCount () == band.GetYCount ();
}

//*************************************************************************************************************
//
//inline void Band::PostSetValue(const char* pValue)
//{
//    CalculatedMinAndMax( Band::ValueToDouble(pValue, GetDataType()) );
//}

//*************************************************************************************************************

unsigned int Band::GetXCount() const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetXCount() : m_nXCount;
}

//*************************************************************************************************************

unsigned int Band::GetYCount ()const
{
    return (NULL != m_pOwnerGrid) ? m_pOwnerGrid->GetYCount() : m_nYCount;
}

//*************************************************************************************************************

bool Band::IsValid(unsigned int i, unsigned int j) const
{
    double value = 0;
    return GetValueAsDouble(i, j, value);
}

//*************************************************************************************************************

bool Band::IsValid(BandDataType datatype, void* value) const
{
    assert(datatype == m_dataType);

    switch(datatype)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        return memcmp(value, &m_nullValue, 1) != 0;

    case UnsignedInt16:
    case Int16:
        return memcmp(value, &m_nullValue, 2) != 0;

    case UnsignedInt32:
    case Int32:
    case Double32:
        return memcmp(value, &m_nullValue, 4) != 0;

    case UnsignedInt64:
    case Int64:
    case Double64:
        return memcmp(value, &m_nullValue, 8) != 0;
    }

    return false;
}

//*************************************************************************************************************

bool Band::GetNearByDoubleValues(unsigned int i, unsigned int j,
                                    double&             center,
                                    double&             top,
                                    double&             bottom,
                                    double&             left,
                                    double&             right) const
{
    if (i >= m_nXCount || j >= m_nYCount)
        return false;

    if (m_dataType == Double32 )
    {
        unsigned int index = j * m_nXCount + i;

        float fcenter, fleft, fright, ftop, fbottom;

#ifdef _WIN32
        fcenter = ((ByteBandData*)m_pBandData)->GetNativeDouble32(index);

        //check for null value and bail if pixel has no data
        if (memcmp(&fcenter, &m_nullValue, 4) == 0)
            return false;

        if (!i)
            fleft = fcenter;
        else
            fleft = ((ByteBandData*)m_pBandData)->GetNativeDouble32(index-1);

        if (i == m_nXCount - 1)
            fright = fcenter;
        else
            fright = ((ByteBandData*)m_pBandData)->GetNativeDouble32(index+1);

        if (!j)
            ftop = fcenter;
        else
            ftop = ((ByteBandData*)m_pBandData)->GetNativeDouble32(index-m_nXCount);

        if (j == m_nYCount - 1)
            fbottom = fcenter;
        else
            fbottom = ((ByteBandData*)m_pBandData)->GetNativeDouble32(index+m_nXCount);

        if (memcmp(&fleft, &m_nullValue, 4) == 0)
            fleft = fcenter;
        if (memcmp(&fright, &m_nullValue, 4) == 0)
            fright = fcenter;
        if (memcmp(&ftop, &m_nullValue, 4) == 0)
            ftop = fcenter;
        if (memcmp(&fbottom, &m_nullValue, 4) == 0)
            fbottom = fcenter;
#else
        // TODO: Linux implementation
        fleft = 0.0f;
        fright = 0.0f;
        ftop = 0.0f;
        fbottom = 0.0f;
#endif

        left = fleft;
        right = fright;
        top = ftop;
        bottom = fbottom;

        return true;
    }
    else if (m_dataType == Double64)
    {
        unsigned int index = j * m_nXCount + i;

#ifdef _WIN32
        center = ((ByteBandData*)m_pBandData)->GetNativeDouble64(index);

        //check for null value and bail if pixel has no data
        if (memcmp(&center, &m_nullValue, 8) == 0)
            return false;

        if (!i)
            left = center;
        else
            left = ((ByteBandData*)m_pBandData)->GetNativeDouble64(index-1);

        if (i == m_nXCount - 1)
            right = center;
        else
            right = ((ByteBandData*)m_pBandData)->GetNativeDouble64(index+1);

        if (!j)
            top = center;
        else
            top = ((ByteBandData*)m_pBandData)->GetNativeDouble64(index-m_nXCount);

        if (j == m_nYCount - 1)
            bottom = center;
        else
            bottom = ((ByteBandData*)m_pBandData)->GetNativeDouble64(index+m_nXCount);
#else
        // Linux implementation
#endif

        if (memcmp(&left, &m_nullValue, 8) == 0)
            left = center;
        if (memcmp(&right, &m_nullValue, 8) == 0)
            right = center;
        if (memcmp(&top, &m_nullValue, 8) == 0)
            top = center;
        if (memcmp(&bottom, &m_nullValue, 8) == 0)
            bottom = center;

        return true;
    }
    else
    {
        bool bcenter = GetValueAsDouble(i, j, center);

        if (!bcenter)
            return false;

        bool btop = GetValueAsDouble(i, j - 1, top);
        bool bbottom = GetValueAsDouble(i, j+1, bottom);
        bool bleft = GetValueAsDouble(i-1, j, left);
        bool bright = GetValueAsDouble(i+1, j, right);

        if (!btop)
            top = center;
        if (!bbottom)
            bottom = center;
        if (!bleft)
            left = center;
        if (!bright)
            right = center;

        return true;

    }
}

//*************************************************************************************************************



//*************************************************************************************************************
///<summary>
/// get the mean value of this Band.
///</summary>
///<returns> the mean value of this band</returns>
double Band::GetMean()
{
    double mean = 0;
    unsigned int  nullvalueCount = 0;

    for (unsigned j = 0; j < this->GetYCount(); ++j)
    {
        for (unsigned i = 0; i < this->GetXCount(); ++i)
        {
            double val;
            bool bValid = GetValueAsDouble (i, j, val);
            if (bValid)
            {
                mean += val;
            }
            else
            {
                ++nullvalueCount;
            }
        }
    }

    double count = (double)(this->GetXCount()) * this->GetYCount()  - nullvalueCount;
    mean /= count;
    return mean;
}



///<summary>
/// get the deviation value of this Band.
///</summary>
///<returns>the deviation value of this band</returns>
double Band::GetDeviation()
{
    double mean = this->GetMean ();
    unsigned  int nullvalueCount = 0;

    double variance = 0;
    for (unsigned j = 0; j < this->GetYCount(); ++j)
    {
        for (unsigned i = 0; i < this->GetXCount(); ++i)
        {
            double val;
            bool bValid = this->GetValueAsDouble (i, j, val);
            if (bValid)
            {
                double vm = val - mean;
                variance += vm * vm;
            }
            else
            {
                ++nullvalueCount;
            }
        }
    }

    double count = (double)(this->GetXCount()) * this->GetYCount() - nullvalueCount;
    return sqrt(variance / count);
}


//*************************************************************************************************************

Band* Band::DeepClone()
{
    Band* pBand = new Band(   GetDataType(),
                                    GetOriginalPoint2D(),
                                    GetXExtent(),
                                    GetYExtent(),
                                    GetXCount(),
                                    GetYCount() );
    assert(pBand);

    if (!pBand->DeepCopy(this))
    {
        delete pBand;
        pBand = NULL;
    }

    return pBand;
}

bool Band::DeepCopy(const Band* pSource)
{
    if (NULL == pSource) return false;
    if (GetDataType() != pSource->GetDataType() ||
        GetXCount()   != pSource->GetXCount()   ||
        GetYCount()   != pSource->GetYCount()   )
    {
        return false;
    }

    m_strName        = pSource->m_strName;
    m_strDescription = pSource->m_strDescription;
    m_nullValue      = pSource->m_nullValue;
    m_minz           = pSource->m_minz;
    m_maxz           = pSource->m_maxz;
    m_pOwnerGrid     = pSource->m_pOwnerGrid;

    return m_pBandData->DeepCopy(pSource->m_pBandData);
}


//*************************************************************************************************************
bool Band::SetRowValue(unsigned int xFrom, unsigned int y, BandDataType datatype, char* pFrom, unsigned int nCount)
{
    bool res = false;

    switch(datatype)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        res = m_pBandData->SetRowValue(xFrom, y,  reinterpret_cast<INT8*> (pFrom), nCount);
        break;

    case UnsignedInt16:
    case Int16:
        res = m_pBandData->SetRowValue(xFrom, y,  reinterpret_cast<INT16*> (pFrom), nCount);
        break;

    case UnsignedInt32:
    case Int32:
    case Double32:
        res = m_pBandData->SetRowValue(xFrom, y,  reinterpret_cast<INT32*> (pFrom), nCount);
        break;

    case UnsignedInt64:
    case Int64:
    case Double64:
        res = m_pBandData->SetRowValue(xFrom, y,  reinterpret_cast<INT64*> (pFrom), nCount);
        break;
    }

    if (res)
    {
        SetDataChangedFlag();
        return true;
    }

    return false;
}


bool Band::SetAllToValue(BandDataType datatype, void* pvalue)
{
    bool res = false;

    switch(datatype)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        res = m_pBandData->SetAllToValue(reinterpret_cast<INT8*> (pvalue));
        break;

    case UnsignedInt16:
    case Int16:
        res = m_pBandData->SetAllToValue(reinterpret_cast<INT16*> (pvalue));
        break;

    case UnsignedInt32:
    case Int32:
    case Double32:
        res = m_pBandData->SetAllToValue(reinterpret_cast<INT32*>  (pvalue));
        break;

    case UnsignedInt64:
    case Int64:
    case Double64:
        res = m_pBandData->SetAllToValue(reinterpret_cast<INT64*> (pvalue));
        break;
    }

    if (res)
    {
        SetDataChangedFlag();
        return true;
    }

    return false;
}

unsigned char* Band::GetRawPointer()
{
    return m_pBandData ? m_pBandData->GetRawPointer() : NULL;
}


void Band::SetNullValue(BandDataType type, void* nullValue)
{
    assert(type == m_dataType);

    m_nullValue = 0;

    switch(type)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        memcpy(&m_nullValue, nullValue, 1);
        break;

    case UnsignedInt16:
    case Int16:
        memcpy(&m_nullValue, nullValue, 2);
        break;

    case UnsignedInt32:
    case Int32:
    case Double32:
        memcpy(&m_nullValue, nullValue, 4);
        break;

    case UnsignedInt64:
    case Int64:
    case Double64:
        memcpy(&m_nullValue, nullValue, 8);
        break;
    }

    SetDataChangedFlag();
}

Band::BandDataType Band::GetNullValue(void* ret) const
{
    switch(m_dataType)
    {
    case UnsignedInt8:
    case Int8:
    case Boolean:
    case Bit2:
    case Bit4:
        memcpy(ret, &m_nullValue, 1);
        break;

    case UnsignedInt16:
    case Int16:
        memcpy(ret, &m_nullValue, 2);
        break;

    case UnsignedInt32:
    case Int32:
    case Double32:
        memcpy(ret, &m_nullValue, 4);
        break;

    case UnsignedInt64:
    case Int64:
    case Double64:
        memcpy(ret, &m_nullValue, 8);
        break;
    }

    return m_dataType;
}
