///////////////////////////////////////////////////////////////////////////////
// Existing stuff for reference only.
class MG_GEOMETRY_API MgCoordinateCollection : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetCount() const;
    virtual MgCoordinate* GetItem(INT32 index) const;
    virtual void SetItem(INT32 index, MgCoordinate* value);
    virtual void Add(MgCoordinate* value);
    virtual void Insert(INT32 index, MgCoordinate* value);
    virtual void Clear();
    virtual bool Remove(const MgCoordinate* value);
    virtual void RemoveAt(INT32 index);
    virtual bool Contains(const MgCoordinate* value) const;
    virtual INT32 IndexOf(const MgCoordinate* value) const;
    MgCoordinateCollection();
EXTERNAL_API:
    virtual ~MgCoordinateCollection();
INTERNAL_API:
    MgCoordinateIterator* GetIterator();
    MgCoordinateCollection* Copy();
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly);
protected:
    void Dispose() { delete this; }
private:
    Ptr<MgDisposableCollection> m_collection;
};

///////////////////////////////////////////////////////////////////////////////
// New stuff for Grid/Graticule generally.
enum EmgGridLineType { lnDirNone = 0,
                       lnDirEastWest,
                       lnDirNorthSouth,
                       lnDirUnknown
                     };

enum EmgGridLineTerm { lnTrmNone = 0,
                       lnTrmEast,
                       lnTrmWest,
                       lnTrmNorth,
                       lnTrmSouth,
                       lnTrmInternal,
                       lnTrmUnknown
                     };

class MgGridCoordinateCollection : public MgCoordinateCollection
{
public:
    MgGridCoordinateCollection (EmgGridLineType lnDir,double isoValue,double chord = 0.0);

    MgGridCoordinateCollection (const MgGridCoordinateCollection& rhs);     //???//
    MgGridCoordinateCollection operator= (const MgGridCoordinateCollection& rhs);   //???//

    EmgGridLineType GetLineType (void);
    double GetIsoValue (void) const;
    double GetChord (void) const;
    EmgGridLineTerm FirstPoint;
    EmgGridLineTerm LastPoint;

private:
    EmgGridLineType LineType;       // Essentially indicates if the IsoValue is an
                                    // easting/longitude or a northing/latitude
    double IsoValue;                // value represented by the line
    double ChordValue;              // an indication of the accuracy of representation
                                    // not really used, but will be available at
                                    // contsruction time and cannot reconstructed.
};

class MgMgrsCoordinateCollection : public MgGridCoordinateCOllection
{
    MgMgrsCoordinateCollection (INT32 utmZone,wchar_t quadEast,wchar_t quadWest);

    INT32 GetUtmZone (void);
    STRING GetQuadDesignation (void);
    INT32 GetMGRSLabelValue (void);

private:
    INT32 UtmZone;                  // 0 = invalid,
                                    // > 0 means northern hemisphere
                                    // < 0 means southern hemisphere
                                    // value of 61 indicates polar region
    wchar_t QuadDesignation [2];    // Easting portion of the Quad designation
    wchar_t QuadNorth;              // Northing portion of the Quad Designation
    double QuadBaseEasting;         // The base UTM easting value of the quad.
    double QuadBaseNorthing;        // The base UTM northing value of the quad.
};

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMMGRS_H_
#define _MGCOORDINATESYSTEMMGRS_H_

class MgCoordinateSystemFactory : public MgGuardDisposable
{
...
    virtual MgCoordinateSystemMgrs* GetMgrs(
        CREFSTRING sTargetCsCode,           //system used for input and output coordinates
        bool bUseTargetDatum,               //true to have the MGRS use the datum of pTargetCs. false to use WGS84
        int nLetteringScheme,               //a value from MgCoordinateSystemMgrsLetteringScheme
        bool bSetExceptionsOn);             //true to repport errors as MgExceptions. false to use MgCoordinateSystemMgrs::GetLastError

    virtual MgCoordinateSystemMgrs* GetMgrs(
        MgCoordinateSystem *pTargetCs,      //system used for input and output coordinates
        bool bUseTargetDatum,               //true to have the MGRS use the datum of pTargetCs. false to use WGS84
        int nLetteringScheme,               //a value from MgCoordinateSystemMgrsLetteringScheme
        bool bSetExceptionsOn);             //true to repport errors as MgExceptions. false to use MgCoordinateSystemMgrs::GetLastError

...
}

class MgCoordinateSystemBoundary : public MgGuardDisposable
{
    virtual MgCoordinate* GetTopLeft();  //do we want to name this NorthWest????
    virtual MgCoordinate* GetTopRight();  //do we want to name this ????
    virtual MgCoordinate* GetBottomRight();  //do we want to name this ????
    virtual MgCoordinate* GetBottomLeft();  //do we want to name this ????
}

//defines the lines of an MGRS grid of a given precision, a value from MgCoordinateSystemMgrsPrecision
class MgrsGridLines : public MgGuardDisposable
{
    virtual INT8 GetMgrsPrecision();
    virtual INT32 GetCount() const;
    virtual MgCoordinateCollection* GetLine(INT32 index);
    virtual void RemoveAt(INT32 index);
    virtual void Clear();
}

//defines the avrious grid levels of an MGRS grid
//each entry lists all the lines of a grid of a given precision, a value from MgCoordinateSystemMgrsPrecision
class MgCoordinateSystemMgrsGridLinesCollection : public MgGuardDisposable
{
    virtual INT32 GetCount() const;
    virtual MgCoordinateSystemMgrsGridLines* GetItem(INT32 index) const;
    virtual MgCoordinateSystemMgrsGridLines* GetItem(INT8 nMgrsPrecision) const;
    virtual void RemoveAt(INT32 index);
    virtual void Clear();
    virtual INT32 IndexOf(INT8 nMgrsPrecision) const;
}

class MgCoordinateSystemMgrsTickMark : public MgGuardDisposable
{
    virtual MgCoordinate* GetPosition();
    virtual MgCoordinate* GetDirectionFromPosition();
}

class MgCoordinateSystemMgrsTickMarkCollection : public MgGuardDisposable
{
    virtual INT32 GetCount() const;
    virtual MgCoordinateSystemMgrsTickMark* GetItem(INT32 index) const;
    virtual void RemoveAt(INT32 index);
    virtual void Clear();
}

class MgCoordinateSystemMgrsLabel : public MgGuardDisposable
{
    virtual MgCoordinate* GetPosition();
    virtual MgCoordinate* GetBaseLineStart();
    virtual MgCoordinate* GetBaseLineEnd();
    virtual MgCoordinate* GetBaseLineAngle();
}

class MgCoordinateSystemMgrsDoubleLabel : public MgCoordinateSystemMgrsLabel
{
    virtual double GetValue();
}

class MgCoordinateSystemMgrsTextLabel : public MgCoordinateSystemMgrsLabel
{
    virtual STRING GetValue();
}

class MgCoordinateSystemMgrsLabelCollection : public MgGuardDisposable
{
    virtual INT32 GetCount() const;
    virtual MgCoordinateSystemMgrsLabel* GetItem(INT32 index) const;
    virtual void RemoveAt(INT32 index);
    virtual void Clear();
}

class MgCoordinateSystemMgrsPrecision
{
PUBLISHED_API:
    static const INT8 Graticule     = 0; //the 6 x 8 degree zones
    static const INT8 Grid100Km     = 1; //the 100 Km squares
    static const INT8 Grid10Km      = 2; //the 10 Km squares
    static const INT8 Grid1Km       = 3; //the 1 Km squares
    static const INT8 Grid100m      = 4; //the 100 m squares
    static const INT8 Grid10m       = 5; //the 10 m squares
    static const INT8 Grid1m        = 6; //the 1 m squares
};

class MgCoordinateSystemMgrs : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT8 GetLetteringScheme()=0;

    //section that reads/writes MGRS coordinates
    virtual STRING ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision)=0;
    virtual STRING ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision)=0;
    virtual MgCoordinate* ConvertToLonLat(CREFSTRING sMgrs)=0;

    virtual STRING ConvertFromXY(double dX, double dY, int nPrecision)=0;
    virtual STRING ConvertFrom(MgCoordinate* pCsCoordinate, int nPrecision)=0;
    virtual MgCoordinate* ConvertTo(CREFSTRING sMgrs)=0;

    //error mechanism
    virtual INT32 GetLastError()=0;
    virtual void ResetLastError()=0;
    virtual bool AreExceptionsOn()=0;
    virtual void SetExceptionsOn(bool bOn)=0;

    //grids and graticules
    //get all the grid lines included inside the extents
    //If multiple precisions are requested, the various grids will all be returned at once
    //the output polylines are straight lines if the MGRS coordinate system
    //is a UTM system and if the area of coverage is within the range of this UTM system
    //the ouput polylines are segmented if the area of coverage is outside the range of this UTM system
    //or if the MGRS coordinate system is not a UTM system
    virtual MgCoordinateSystemMgrsGridLinesCollection* GetGridLines(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8[] nMgrsPrecision);                                             //precision of the grids, a list of values from MgCoordinateSystemMgrsPrecision

    //get all the grid line tick marks that can positioned on the top of a given area
    //defined in the MGRS coordinate system
    //This method doesn't make sense for MgCoordinateSystemMgrsPrecision::Graticule and will return an empty list
    virtual MgCoordinateSystemMgrsTickMarkCollection* GetTopTickMarks(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision,                                                //precision of the grids, a list of values from MgCoordinateSystemMgrsPrecision
        INT8 nSubDivisions);                                                //number of tick marks that divide a grid square of MgCoordinateSystemMgrsPrecision

    //get all the grid line tick marks that can positioned on the right of a given area
    //defined in the MGRS coordinate system
    //This method doesn't make sense for MgCoordinateSystemMgrsPrecision::Graticule and will return an empty list
    virtual MgCoordinateSystemMgrsTickMarkCollection* GetRightTickMarks(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision,                                                //precision of the grids, a list of values from MgCoordinateSystemMgrsPrecision
        INT8 nSubDivisions);                                                //number of tick marks that divide a grid square of MgCoordinateSystemMgrsPrecision

    //get all the grid line tick marks that can positioned on the bottom of a given area
    //defined in the MGRS coordinate system
    //This method doesn't make sense for MgCoordinateSystemMgrsPrecision::Graticule and will return an empty list
    virtual MgCoordinateSystemMgrsTickMarkCollection* GetBottomTickMarks(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision,                                                //precision of the grids, a list of values from MgCoordinateSystemMgrsPrecision
        INT8 nSubDivisions);                                                //number of tick marks that divide a grid square of MgCoordinateSystemMgrsPrecision

    //get all the grid line tick marks that can positioned on the top of a given area
    //defined in the MGRS coordinate system
    //This method doesn't make sense for MgCoordinateSystemMgrsPrecision::Graticule and will return an empty list
    virtual MgCoordinateSystemMgrsTickMarkCollection* GetLeftTickMarks(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision,                                                //precision of the grids, a list of values from MgCoordinateSystemMgrsPrecision
        INT8 nSubDivisions);                                                //number of tick marks that divide a grid square of MgCoordinateSystemMgrsPrecision

    //get all the grid line labels that can positioned on the outside top of a given area
    //defined in the MGRS coordinate system
    //returns a list of MgCoordinateSystemMgrsTextLabel for MgCoordinateSystemMgrsPrecision::Graticule and MgCoordinateSystemMgrsPrecision::Grid100Km
    //returns a list of MgCoordinateSystemMgrsDoubleLabel for other MgCoordinateSystemMgrsPrecision values
    virtual MgCoordinateSystemMgrsLabelCollection* GetGridTopLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision);                                               //precision of the grid to be labeled, a value from MgCoordinateSystemMgrsPrecision

    //get all the grid line labels that can positioned on the outside right of a given area
    //defined in the MGRS coordinate system
    //returns a list of MgCoordinateSystemMgrsTextLabel for MgCoordinateSystemMgrsPrecision::Graticule and MgCoordinateSystemMgrsPrecision::Grid100Km
    //returns a list of MgCoordinateSystemMgrsDoubleLabel for other MgCoordinateSystemMgrsPrecision values
    virtual MgCoordinateSystemMgrsLabelCollection* GetGridRightLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision);

    //precision of the grid to be labeled, a value from MgCoordinateSystemMgrsPrecision
    //get all the grid line labels that can positioned on the outside bottom of a given area
    //defined in the MGRS coordinate system
    //returns a list of MgCoordinateSystemMgrsTextLabel for MgCoordinateSystemMgrsPrecision::Graticule and MgCoordinateSystemMgrsPrecision::Grid100Km
    //returns a list of MgCoordinateSystemMgrsDoubleLabel for other MgCoordinateSystemMgrsPrecision values
    virtual MgCoordinateSystemMgrsLabelCollection* GetGridBottomLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision);

    //precision of the grid to be labeled, a value from MgCoordinateSystemMgrsPrecision
    //get all the grid line labels that can positioned on the outside left of a given area
    //defined in the MGRS coordinate system
    //returns a list of MgCoordinateSystemMgrsTextLabel for MgCoordinateSystemMgrsPrecision::Graticule and MgCoordinateSystemMgrsPrecision::Grid100Km
    //returns a list of MgCoordinateSystemMgrsDoubleLabel for other MgCoordinateSystemMgrsPrecision values
    virtual MgCoordinateSystemMgrsLabelCollection* GetGridLeftLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        INT8 nMgrsPrecision);                                               //precision of the grid to be labeled, a value from MgCoordinateSystemMgrsPrecision

    //returns the labels for all the 6x8 degree graticule zones that are entirely included inside the
    //area of interest
    //returns a list of MgCoordinateSystemMgrsTextLabel
    virtual MgCoordinateSystemMgrsLabelCollection* GetGraticuleWholeZoneLabels(
        const MgCoordinateSystemBoundary& extents);                         //extents of the area of interest defined in the MGRS coordinate system

    //returns the labels for all the intersections between four 6x8 degree graticule zones included inside the
    //area of interest
    //returns a list of MgCoordinateSystemMgrsTextLabel
    virtual MgCoordinateSystemMgrsLabelCollection* GetGraticuleFourCornerLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        bool bIgnoreFourCornersOfWholeZones);                               //true if we want to ignore the 6x8 degree zones that entirely fit inside the extents

    //returns the labels for all the 100Km squares that are entirely included inside the
    //area of interest
    //returns a list of MgCoordinateSystemMgrsTextLabel
    virtual MgCoordinateSystemMgrsLabelCollection* GetGrid100KmWholeSquareLabels(
        const MgCoordinateSystemBoundary& extents);                          //extents of the area of interest defined in the MGRS coordinate system

    //returns the labels for all the intersections between four 100Km squares included inside the
    //area of interest
    //returns a list of MgCoordinateSystemMgrsTextLabel
    virtual MgCoordinateSystemMgrsLabelCollection* GetGrid100KmFourCornerLabels(
        const MgCoordinateSystemBoundary& extents,                          //extents of the area of interest defined in the MGRS coordinate system
        bool bIgnoreFourCornersOfWholeZones);                               //true if we want to ignore the 100Km squares that entirely fit inside the extents

INTERNAL_API:
    virtual INT32 ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat)=0;
    virtual INT32 ConvertToLonLat(CREFSTRING sMgrs, double& dLongitude, double& dLatitude)=0;

    virtual INT32 ConvertFromXY(double dX, double dY, int nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertFrom(MgCoordinate* pCsCoordinate, int nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertTo(CREFSTRING sMgrs, MgCoordinate* pCsCoordinate)=0;
    virtual INT32 ConvertToXY(CREFSTRING sMgrs, double& dX, double& dY)=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemMgrs;
};


#endif //_MGCOORDINATESYSTEMMGRS_H_
