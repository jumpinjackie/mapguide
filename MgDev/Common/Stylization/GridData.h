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

#ifndef GRIDDATA_H_
#define GRIDDATA_H_

#include "MdfModel.h"
#include "Band.h"
#include "HillShade.h"

///<summary>
/// This class keep a set of Band.
/// And all the Band in the grid, has the same original point exten and count.
///</summary>
class RS_Raster;
class GridData
{
public:
    typedef std::vector<Band*>        GISGRIDVECTOR;

public:
    ///<summary>
    /// Because all the bands in the same grid have the same original point extent and count,
    /// so this information is kept in the GridData class.
    ///</summary>
    STYLIZATION_API                     GridData(   const Point2D&  point,
                                                    double          xExtent,
                                                    double          yExtent,
                                                    unsigned int    nXCount,
                                                    unsigned int    nYCount);
    /// <summary>
    ///   Destroys an instance of this class.
    /// </summary>
    STYLIZATION_API                        ~GridData();

    /// <summary>
    ///   Populates the GridData from an FdoIRaster
    /// </summary>
    STYLIZATION_API void                     ReadRaster(RS_Raster*       pRaster,
                                                        FdoString*       pBandName,
                                                        unsigned long    cols,
                                                        unsigned long    rows,
                                                        double           xMin,
                                                        double           yMin,
                                                        double           xMax,
                                                        double           yMax,
                                                        bool             bAlignment);



    ///<summary>
    /// Get the Band with band name. If it is not in the grid, return NULL.
    ///</summary>
    STYLIZATION_API Band*            GetBand(const MdfModel::MdfString& name) const;

    ///<summary>
    /// Get the Band with the band index. If it is not in the grid, return NULL.
    /// The index of the Band maybe will changed after some insert operation,
    /// so please don't keep the band index to get the band next time.
    /// In the most case, please you'd better get the Band with the band name.
    /// You can use this method and GetBandCount method to iterator the grid get all
    /// the bands.
    ///</summary>
    STYLIZATION_API Band*            GetBand(unsigned int index) const;
    STYLIZATION_API inline size_t       GetBandCount() const;

    ///<summary>
    /// This method is used to create a new band in the grid, the band name
    /// will be set as the name parament. And this band will be insert to the grid.
    /// If the name is duplicated, this method will return NULL.
    ///</summary>
    STYLIZATION_API Band*            CreateBand(Band::BandDataType dataType, const MdfModel::MdfString& name);

    ///<summary>
    /// Insert a band into the grid. Because a band is identity with it's name in the grid,
    /// so please make sure the name of band is not empty before insert it to the grid,
    /// or this method will return false. And if there is a band in the grid has the same name
    /// with the insert band, it will return false too.
    ///</summary>
    STYLIZATION_API bool                InsertBand( Band* pBand);

    ///<summary>
    /// If the elevation band is a band in the grid, set the elevation band
    /// with the band name and index of the band in the grid.
    ///</summary>
    STYLIZATION_API bool                SetElevationBand(const MdfModel::MdfString& name);
    STYLIZATION_API bool                SetElevationBand(unsigned int index);

    ///<summary>
    /// If the elevation band is not a band in the grid, use this method to set the
    /// elevation band for the grid.
    ///</summary>
    STYLIZATION_API inline void         SetElevationBand(Band* pBand);

    ///<summary>
    /// Get the elevation band in this grid.
    ///</summary>
    STYLIZATION_API inline Band*     GetElevationBand() const;

    ///<summary>
    /// Get a band for stylization use. If it is empty, we will create one band (double type).
    ///</summary>
    STYLIZATION_API Band*            GetElevationBandDataForStylization();

    ///<summary>
    /// Get the draped color band in this grid.
    ///</summary>
    STYLIZATION_API  Band*            GetDrapedColorBand() const;

    ///<summary>
    /// Create a draped color band in this grid.
    /// In some cases, the draped color band is not required for the grid.
    /// So we just offer this method to create the drpaed color band when it is required.
    ///</summary>
    STYLIZATION_API  void                CreateDrapedColorBand();

    ///<summary>
    /// Get the color band in this grid.
    ///</summary>
    STYLIZATION_API Band*            GetColorBand() const;

    ///<summary>
    /// Function to retrieve the band that stores the hillshade values. It is in float format.
    ///</summary>
    STYLIZATION_API const Band*      GetCacheHillShadeBand(const MdfModel::HillShade *pHS) const;

    ///<summary>
    /// Function to set the band that stores the hillshade values. It is in float format.
    ///</summary>
    STYLIZATION_API bool                SetCacheHillShadeBand(
                                                              const MdfModel::HillShade *pHS,
                                                              Band                   *pHSBand);

    ///<summary>
    /// Set the raw color band without hillshade effect if the color band includes effect.
    ///</summary>
    STYLIZATION_API bool                SetNoHillShadeColorBand(Band *pNoHillShadeColorBand);

    ///<summary>
    /// Get the raw color band without hillshade effect if the color band includes effect.
    ///</summary>
    STYLIZATION_API const Band*      GetNoHillShadeColorBand() const;

    ///<summary>
    /// Get the raw color band without hillshade effect if the color band includes effect.
    ///</summary>
    STYLIZATION_API Band*            GetNoHillShadeColorBand();

    ///<summary>
    /// Get the geometry information for the bands in this grid.
    ///</summary>
    STYLIZATION_API inline double       GetXUnitDistance() const;

    STYLIZATION_API inline double       GetYUnitDistance() const;

    STYLIZATION_API inline double       GetInvScaledDX() const;

    STYLIZATION_API inline double       GetInvScaledDY() const;

    STYLIZATION_API inline unsigned int GetXCount() const;

    STYLIZATION_API inline unsigned int GetYCount() const;

    STYLIZATION_API inline double       GetXExtent () const;

    STYLIZATION_API inline double       GetYExtent () const;


    STYLIZATION_API inline const Point2D&            GetOriginalPoint2D() const;

    STYLIZATION_API inline MdfModel::Box2D            GetBoundingBox() const;

    STYLIZATION_API inline void         SetCoordSysUnitLength(double dUnitLength);

    STYLIZATION_API inline double       GetCoordSysUnitLength() const;

    STYLIZATION_API Band::BandDataType  GetGridDataType(RS_Raster* pRaster,
                                                        bool bBandDataType = true) const;

private:
    // Point to the elevation band in this grid.
    Band*                            m_pElevationBand;

    // Point to the draped color band in this grid.
    Band*                            m_pDrapedColorBand;

    // Cached band for stylizationed band.
    Band*                            m_pStylizedBand;

    // Point to the color band in this grid. This band was excluded from the collection.
    mutable Band*                    m_pColorBand;

    // The collection of the bands saved in this grid.
    GISGRIDVECTOR                        m_BandVector;

    // The geometry information for all bands saved in this grid.
    // we can use these information to build a bounding box.
    double                                m_xExtent;
    double                                m_yExtent;
    unsigned int                        m_nXCount;
    unsigned int                        m_nYCount;

    double                                m_dx;
    double                                m_dy;
    double                                m_isdx;
    double                                m_isdy;

    Point2D                                m_westSourthPoint;

    // indicates the scale to transform the unit distance per pixel to elevation units.
    double                              m_dCoordSysUnitLength;

    // Cache for hillshade values for this band
    mutable std::auto_ptr<Band>             m_spHillShadeBand;
    mutable std::auto_ptr<MdfModel::HillShade> m_spMdfHillShade;

    // Store the raw color band without hillshade effect if the color band includes effect.
    std::auto_ptr<Band>              m_spNoHillShadeColorBand;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//inline methods implement
/////////////////////////////////////////////////////////////////////////////////////////////
size_t GridData::GetBandCount() const
{
    return m_BandVector.size();
}

Band* GridData::GetElevationBand()const
{
    return m_pElevationBand;
}

double GridData::GetXUnitDistance() const
{
    return m_dx;
}

double GridData::GetYUnitDistance() const
{
    return m_dy;
}

double GridData::GetInvScaledDX() const
{
    return m_isdx;
}

double GridData::GetInvScaledDY() const
{
    return m_isdy;
}

unsigned int GridData::GetXCount() const
{
    return m_nXCount;
}

unsigned int GridData::GetYCount ()const
{
    return m_nYCount;
}

double GridData::GetXExtent () const
{
    return m_xExtent;
}

double GridData::GetYExtent () const
{
    return m_yExtent;
}

const Point2D& GridData::GetOriginalPoint2D() const
{
    return m_westSourthPoint;
}

MdfModel::Box2D GridData::GetBoundingBox() const
{
    return MdfModel::Box2D( m_westSourthPoint.x,
                            m_westSourthPoint.y,
                            m_westSourthPoint.x + m_xExtent,
                            m_westSourthPoint.y + m_yExtent );
}

void GridData::SetElevationBand(Band* pBand)
{
    m_pElevationBand = pBand;
}

void GridData::SetCoordSysUnitLength(double dUnitLength)
{
    m_dCoordSysUnitLength = dUnitLength;
}

double GridData::GetCoordSysUnitLength() const
{
    return m_dCoordSysUnitLength;
}

#endif
