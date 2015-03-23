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

#ifndef BAND_H_
#define BAND_H_

#include "Stylization.h"
#include "MdfModel.h"
#include "Vector3D.h"
#include "Point2D.h"
#include "Point3D.h"
#include "Foundation.h"
#include <assert.h>
//////////////////////////////////////////////////////////////////////////

// Forward declaration.
class BandData;
class GridData;
class LineBuffer;
class PointIterator;

///<summary>
/// Class use to save band data, include the name description and the vlaue data.
/// It support many types that were list in the BandDataType enum.
///</summary>
class Band
{
public:
    ///<summary>
    /// All the types that Band support are list in the enum.
    ///</summary>
    enum BandDataType
    {
        UnsignedInt8    = 0,
        UnsignedInt16    ,
        UnsignedInt32   ,
        UnsignedInt64   ,
        Int8            ,
        Int16           ,
        Int32            ,
        Int64            ,
        Boolean            ,
        Bit2            ,
        Bit4            ,
        Double32        ,
        Double64
    };

public:

    ///<summary>
    /// Constructs an instance of this class with an owner grid.
    ///</summary>
    STYLIZATION_API                         Band(BandDataType dataType, GridData* pOwnerGrid);

    ///<summary>
    /// Constructs an instance of this class without an owner grid.
    ///</summary>
    STYLIZATION_API                         Band(BandDataType    dataType,
                                                    const Point2D&  point,
                                                    double          xExtent,
                                                    double          yExtent,
                                                    unsigned int    nXCount,
                                                    unsigned int    nYCount );

    /// <summary>
    ///   Destroys an instance of this class.
    /// </summary>
    STYLIZATION_API virtual                 ~Band();

    /// <summary>
    ///   Get the data type of this band
    /// </summary>
    STYLIZATION_API inline BandDataType     GetDataType() const;

    ///<summary>
    /// Gets the value at a given pixel location
    ///</summary>
    STYLIZATION_API bool                    GetValue(unsigned int i, unsigned int j, BandDataType datatype, void* pValue) const;

    ///<summary>
    /// Sets a value at a given pixel location. Desired data type must be specified
    ///</summary>
    STYLIZATION_API bool                    SetValue(unsigned int i, unsigned int j, BandDataType datatype, void* pValue);

    ///<summary>
    /// Sets a row of pixels, starting at a given column.
    ///</summary>
    STYLIZATION_API bool                    SetRowValue(unsigned int xFrom, unsigned int y, BandDataType datatype, char * pFrom, unsigned int nCount);

    ///<summary>
    /// Sets all pixels to the given value.
    ///</summary>
    STYLIZATION_API bool                    SetAllToValue(BandDataType datatype, void* pvalue);

    ///<summary>
    /// This method can used to get any data type value. The return is double type.
    /// For example, maybe the data type is Unisgned INT8, the value is 12.
    /// If use this method get the value, the value will be 12.0000.
    ///</summary>
    STYLIZATION_API bool                    GetValueAsDouble(unsigned int i, unsigned int j, double& value) const;
    STYLIZATION_API double                  GetValueAsDouble (unsigned int i, unsigned int j) const;

    ///<summary>
    /// The the Normal at the [i,j] point. This is a approximately algorithm.
    ///</summary>
    STYLIZATION_API bool                    GetNormal(unsigned int i, unsigned int j, Vector3D& normal, double scale = 1.0) const;

    ///<summary>
    /// Function to retrieve the band that stores aspect values. It is in float format.
    ///</summary>
    STYLIZATION_API const Band*          GetAspectBand() const;

    ///<summary>
    /// Function to retrieve the band that stores the slope values. It is in float format.
    ///</summary>
    STYLIZATION_API const Band*          GetSlopeBand() const;

    ///<summary>
    /// Get the color value at point use approximately algorithm.
    STYLIZATION_API UINT32        GetBilinearColor(const Point2D& point) const;
    ///</summary>

    ///<summary>
    /// Get the color value at position [x,y]
    /// we alway use 4 byte to represent a color value.
    ///</summary>
    STYLIZATION_API UINT32        GetColorValue(unsigned int i, unsigned int j) const;

    ///<summary>
    /// Transform the point position to the index in band.
    ///</summary>
    STYLIZATION_API void                    PosToIndex(double x, double y, int& i, int& j) const;

    ///<summary>
    /// Transform the index in band to the point position.
    ///</summary>
    STYLIZATION_API void                    IndexToPos(unsigned int i, unsigned int j, double& x, double& y) const;

    ///<summary>
    /// Deep Clone a new Band. The caller has the responsiblity to release the new Band.
    ///</summary>
    STYLIZATION_API Band*                DeepClone();

    ///<summary>
    /// Help method to deep copy a Band to another Band. The two bands banddata type xcount
    /// ycount must equal, or it will return false.
    ///</summary>
    STYLIZATION_API bool                    DeepCopy(const Band* pSource);

public:
    // Accessors

    ///<summary>
    /// Get the resolution at X direction.
    ///</summary>
    STYLIZATION_API unsigned int            GetXCount() const;

    ///<summary>
    /// Get the resolution at Y direction.
    ///</summary>
    STYLIZATION_API unsigned int            GetYCount ()const;

    ///<summary>
    /// Get name of the band.
    ///</summary>
    STYLIZATION_API inline const wchar_t*   GetName() const;

    ///<summary>
    /// Set name for the band.
    ///</summary>
    STYLIZATION_API inline void             SetName(const wchar_t* name);

    ///<summary>
    /// Get unit name of the band data.
    ///</summary>
    STYLIZATION_API inline const wchar_t*   GetUnitName() const;

    ///<summary>
    /// Set unit name for the band data.
    ///</summary>
    STYLIZATION_API inline void             SetUnitName(const wchar_t* unitname);

    ///<summary>
    /// Get description for the band.
    ///</summary>
    STYLIZATION_API inline const wchar_t*   GetDescription() const;

    ///<summary>
    /// Set description for the band.
    ///</summary>
    STYLIZATION_API inline void             SetDescription(const wchar_t* description);

    ///<summary>
    /// Get min value in the band.
    ///</summary>
    STYLIZATION_API double                  GetMinZ() const;

    ///<summary>
    /// Get max value in the band.
    ///</summary>
    STYLIZATION_API double                  GetMaxZ() const;

    ///<summary>
    /// Get the owner grid of the band.
    ///</summary>
    STYLIZATION_API inline GridData*         GetOwnerGrid() const;

    ///<summary>
    /// Set the owner grid for the band.
    ///</summary>
    STYLIZATION_API inline void             SetOwerGrid(GridData* pGrid);

    ///<summary>
    /// Set the NULL Value for this band.
    /// If the NULL Value changed, the data meanning maybe changed, so we have to set the data change flag.
    ///</summary>
    STYLIZATION_API void                    SetNullValue(BandDataType datatype, void* nullValue);

    ///<summary>
    /// Get the NULL Value for this band.
    ///</summary>
    STYLIZATION_API BandDataType            GetNullValue(void* ret) const;

    ///<summary>
    /// Check is this value is valid for this band.
    ///</summary>
    STYLIZATION_API bool                    IsValid(BandDataType datatype, void* value) const;

    ///<summary>
    /// Check is value at [i,j] is valid for this band.
    ///</summary>
    STYLIZATION_API bool                    IsValid(unsigned int i, unsigned int j) const;

    ///<summary>
    /// The extent and original point information was saved in the owner grid
    /// so it will get these information from the owner grid. If the owner grid
    /// is empty, all below get method will failed.
    ///</summary>
    STYLIZATION_API double                  GetXUnitDistance() const;
    STYLIZATION_API double                  GetYUnitDistance() const;
    STYLIZATION_API double                  GetXExtent() const;
    STYLIZATION_API double                  GetYExtent() const;
    STYLIZATION_API const Point2D&          GetOriginalPoint2D() const ;
    STYLIZATION_API MdfModel::Box2D         GetBoundingBox() const;
    STYLIZATION_API bool                    HasSamePlacement(const Band& band) const;

    ///<summary>
    /// get the mean value of this Band.
    ///</summary>
    ///<returns> the mean value of this band</returns>
    STYLIZATION_API double                  GetMean();

    ///<summary>
    /// get the deviation value of this Band.
    ///</summary>
    ///<returns>the deviation value of this band</returns>
    STYLIZATION_API double                  GetDeviation();

    ///<summary>
    /// get the raw pointer to the data
    ///</summary>
    ///<returns>the raw pointer to the data</returns>
    STYLIZATION_API unsigned char*          GetRawPointer();

protected:
    ///<summary>
    /// Internal help method to caculate min/max value in this band.
    ///</summary>
    void                                    CalculatedMinAndMax() const;

    ///<summary>
    /// Internal help method to set the data change flag.
    /// If the flag set to true, we have to recaculate the min/max value in this band.
    ///</summary>
    inline void                             SetDataChangedFlag();

    ///<summary>
    /// Internal help method to create the BandData class according the data type.
    ///</summary>
    static    BandData*                     CreateDataGrid(BandDataType dataType, unsigned int width, unsigned int height);

private:

    bool GetNearByDoubleValues(unsigned int i, unsigned int j,
                                double&             center,
                                double&             top,
                                double&             bottom,
                                double&             left,
                                double&             right) const;



    // Band number for Queried
    // Expression for Calculated
    // Name for Custom
    MdfModel::MdfString                     m_strName;

    // Optional descriptive label (can be written back to XML config file)
    MdfModel::MdfString                     m_strDescription;

    // The null value for the band. Not all 64 bits of this
    // integer will be used for all data types, we just decalre the
    // maximum# of bits that we will need based on the possible BandDataTypes
    INT64                                 m_nullValue;

    // The data type of the band and BandData
    BandDataType                            m_dataType;
    BandData*                               m_pBandData;

    // The unit name of this band data
    MdfModel::MdfString                     m_unitName;

    // A cache for min/max value in the band.
    mutable double                          m_minz;
    mutable double                          m_maxz;
    // A flag indicate that need we recaculate the max and min value.
    mutable bool                            m_bDataChanged;

    // The owner grid that this band belongs to.
    // If it is NULL, the below geometry information will works.
    // Or we just get the geometry information from the owner grid.
    GridData*                                m_pOwnerGrid;

    // The geometry information for this band.
    // we can use these information to build a bounding box.
    double                                  m_xExtent;
    double                                  m_yExtent;
    unsigned int                            m_nXCount;
    unsigned int                            m_nYCount;
    Point2D                                 m_westSourthPoint;

    // Cache for Aspect values of this band.
    mutable std::auto_ptr<Band>          m_spAspectBand;

    // Cache for Slope values of this band.
    mutable std::auto_ptr<Band>          m_spSlopeBand;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//inline methods implement
/////////////////////////////////////////////////////////////////////////////////////////////

Band::BandDataType Band::GetDataType() const
{
    return m_dataType;
}

const wchar_t* Band::GetName() const
{
    return m_strName.empty() ? NULL : m_strName.c_str();
}

void Band::SetName(const wchar_t* name)
{
    m_strName = (name != NULL ? name : L"");
}

const wchar_t* Band::GetUnitName() const
{
    return m_unitName.empty() ? NULL : m_unitName.c_str();
}

void Band::SetUnitName(const wchar_t* unitname)
{
    m_unitName = (unitname != NULL ? unitname : L"");
}

const wchar_t* Band::GetDescription() const
{
    return m_strDescription.empty() ? NULL : m_strDescription.c_str();
}

void Band::SetDescription(const wchar_t* description)
{
    m_strDescription = (description != NULL ? description : L"");
}

GridData* Band::GetOwnerGrid() const
{
    return m_pOwnerGrid;
}

void Band::SetOwerGrid(GridData* pGrid)
{
    m_pOwnerGrid = pGrid;
}

void Band::SetDataChangedFlag()
{
    m_bDataChanged = true;
}

#endif
