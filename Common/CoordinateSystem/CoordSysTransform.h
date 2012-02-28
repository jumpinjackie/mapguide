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

#ifndef _CCOORDINATESYSTEMTRANSFORM_H_
#define _CCOORDINATESYSTEMTRANSFORM_H_

namespace CSLibrary
{

class CCoordinateSystemTransform : public MgCoordinateSystemTransform
{
public:
    CCoordinateSystemTransform(MgCoordinateSystem* source, MgCoordinateSystem* target);
    virtual ~CCoordinateSystemTransform();

    virtual MgCoordinate* Transform(double x, double y);
    virtual MgCoordinate* TransformM(double x, double y, double m);
    virtual MgCoordinate* Transform(double x, double y, double z);
    virtual MgCoordinate* TransformM(double x, double y, double z, double m);
    virtual MgCoordinate* Transform(MgCoordinate* coordinate);
    virtual MgEnvelope* Transform(MgEnvelope* envelope);

    virtual void TransformCoordinate(MgCoordinate* coordinate);

    virtual bool IsValidSourcePoint(double x, double y);
    virtual bool IsValidSourcePoint(double x, double y, double z);
    virtual bool IsValidTargetPoint(double x, double y);
    virtual bool IsValidTargetPoint(double x, double y, double z);

    virtual MgCoordinateSystem* GetSource();
    virtual MgCoordinateSystem* GetTarget();
    virtual void SetSourceAndTarget(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget);

    // Geodetic Transformation Information
    virtual INT32 GetGeodeticTransformationCount();
    virtual MgCoordinateSystemGeodeticTransformDef* GetGeodeticTransformation (INT32 index);
    virtual INT32 GetGeodeticTransformationDirection (INT32 index);
    virtual MgCoordinateSystemGeodeticPath* GetExplicitGeodeticPath();

    virtual void IgnoreDatumShiftWarning(bool bIgnoreDatumShiftWarning);
    virtual bool IsIgnoreDatumShiftWarning();
    virtual void IgnoreOutsideDomainWarning(bool bIgnoreOutsideDomainWarning);
    virtual bool IsIgnoreOutsideDomainWarning();

    virtual INT32 GetSourceWarningCount (void);
    virtual INT32 GetdatumWarningCount (void);
    virtual INT32 GetTargetWarningCount (void);

    virtual INT32 GetLastTransformStatus();
    virtual void ResetLastTransformStatus();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X and Y ordinates.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double* x, double* y);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X and Y ordinates.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double x[], double y[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y ordinates and the measure.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param m
    /// The measure to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double* x, double* y, double* m);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y ordinates and the measure.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param m
    /// The measures to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double x[], double y[], double m[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y, and Z ordinates.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param z
    /// The Z ordinate to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double* x, double* y, double* z);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y, and Z ordinates.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param z
    /// The Z ordinates to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double x[], double y[], double z[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y, Z ordinates and the measure.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param z
    /// The Z ordinate to transform.
    /// \param m
    /// The measure to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double* x, double* y, double* z, double* m);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y, Z ordinates and the measure.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param z
    /// The Z ordinates to transform.
    /// \param m
    /// The measures to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double x[], double y[], double z[], double m[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transform 2D conversion on an array of 2D XY points.
    ///
    /// \param xy
    /// The array of 2D XY points to be converted.
    /// \param pointCount
    /// The number of 2D points in the array which are to be converted.
    /// \return
    /// Nothing.
    /// \remark
    /// Use the Get...WarningCount functions above to determine the status of
    /// the conversion.  A conversion of all points will be attemted regardless
    /// of the status of any specific point conversion.  Warning counts will
    /// accumulate on successive calls to this function unless the
    /// ResetLastTransformStatus member function is called.  This function
    /// will throw an exception only in the case of what is considered a
    /// total failure.  That is, a condition which suggests that there is a
    /// a problem with the conversion process (i.e. a physical I>O error on
    /// disk) that is not related to the data being converted.  In the case of
    /// this function, GetConversionStatus will return the most serious (i.e.
    /// highest value) warning condition encountered.
    virtual void Transform2D (double xy[][2],INT32 pointCount);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms a 3D conversion on an array of 3D XYZ points.
    ///
    /// \param xy
    /// The array of 3D XYZ points to be converted.
    /// \param pointCount
    /// The number of 3D points in the array which are to be converted.
    /// \return
    /// Nothing.
    /// \remark
    /// Use the Get...WarningCount functions above to determine the status of
    /// the conversion.  A conversion of all points will be attemted regardless
    /// of the status of any specific point conversion.  Warning counts will
    /// accumulate on successive calls to this function unless the
    /// ResetLastTransformStatus member function is called.  This function
    /// will throw an exception only in the case of what is considered a
    /// total failure.  That is, a condition which suggests that there is a
    /// a problem with the conversion process (i.e. a physical I>O error on
    /// disk) that is not related to the data being converted.  In the case of
    /// this function, GetConversionStatus will return the most serious (i.e.
    /// highest value) warning condition encountered.
    virtual void Transform3D (double xy[][3],INT32 pointCount);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates a MgCoordinateCollection which represents in the target
    /// coordinate system the linear line segment provided in the source
    /// coordinate system.
    ///
    /// \param fromPnt
    /// The result of the transformation is returned here.  While the result
    /// can be a single straight line segment, more often it will be a multi-
    /// segment approximation of a complex curve.
    /// \param fromPnt
    /// The starting point of the linear segment in source system coordinates.
    /// \param toPnt
    /// The end point of the linear segment in source system coordinates.
    /// \param chordValue
    /// The returned line string is to segmented to the degree necessary such
    /// that the distance between the LineString approximation of the true
    /// cuve and the true curve is never more than this value.  Units must
    /// be the same as the target coordinate system.
    /// \param maxPoints
    /// The generation algorithm will always quit after generating this
    /// number of points in result.
    /// \return
    /// Status of the operation, nature of which is yet to be determined.
    /// \remarks
    /// Does not, currently, have code to deal with exceptions thrown by the
    /// functions it calls.
    virtual MgLineString* GridLine (MgCoordinate* fromPnt,MgCoordinate* toPnt,
                                                          double curvePrecision,
                                                          UINT32 maxPoints = 500);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Give a line segement in coordinates of the source system via the the
    /// fromPnt and toPnt parameters, this function returns the position of
    /// a point on that line segment which, when converted to target
    /// coordinates, will have an ordinate value equal to the ordinateValue
    /// parameter.  Used, primarily, to locate the position of a tick line
    /// on a map border.
    ///
    /// \param position
    /// The result of the calculation is returned here.  This must be a
    /// valid pointer to an exisitng MgCOordinate object.  THis function will
    /// not alter the reference count status in any way.
    /// \param ordinateValue
    /// The specific value whose position is to be located.
    /// \param orientation
    /// A value of the MgCoordinateSystemGridOrientation class.  This parameter
    /// indicates whether the ordinateValue parameter is an X ordinate or a Y
    /// ordinate.
    /// \param fromPnt
    /// A two dimensional point given in coordinates of the source coordinate
    /// system of this <b>Transform<\b> object which represents the starting
    /// point of the line segment which is to be processed.
    /// \param toPnt
    /// A two dimensional point given in coordinates of the source coordinate
    /// system of this <b>Transform<\b> object which represents the ending
    /// point of the line segment which is to be processed.
    /// \return
    /// Will return a zero for success.  A positive non-zero return indicates
    /// a failure to calculate a position for one of the normal reasons:
    /// 1> for no such value on this line segment;
    /// 2> line orientation is inappropriate for this ordinate
    /// For failure of anyother type, and MgException is thrown.
    /// \remarks
    /// The order of the two points provided is immaterial.  This object
    /// attempts to eliminate possible duplicate positions by ignoring
    /// the to pont in the calculation of poosition.
    virtual INT32 PositionOfValue (MgCoordinate* position,double ordinateValue,
                                                          INT32 orientation,
                                                          MgCoordinate* fromPnt,
                                                          MgCoordinate* toPnt);

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members
    Ptr<MgCoordinateSystem> m_pCsSource;
    Ptr<MgCoordinateSystem> m_pCsTarget;
    cs_Dtcprm_ *m_pDtcprm;
    cs_Csprm_ m_src;
    cs_Csprm_ m_dst;

    bool m_bSrcIsGeographic;
    bool m_bDatumXfrmIsNull;
    bool m_bIsReentrant;
    bool m_bIgnoreDatumShiftWarning;
    bool m_bIgnoreOutsideDomainWarning;
    bool m_bSourceTargetSame;
    INT32 m_nTransformStatus;

    // Status Accumulation
    INT32 m_nSourceCount;
    INT32 m_nDatumCount;
    INT32 m_nTargetCount;

    //Private member functions
    void SetCatalog(MgCoordinateSystemCatalog *pCatalog);
    bool IsInitialized();
    void Uninitialize();
    bool IsValidPoint(cs_Csprm_& csprm, double x, double y, double z);

private:
    //Unimplemented stuff
    CCoordinateSystemTransform(const CCoordinateSystemTransform&);
    CCoordinateSystemTransform& operator=(const CCoordinateSystemTransform&);

    // Private function required to support the PositionOfValue function.
    int TransformInverse (double& xx,double& yy);
    INT32 TransformPoint2D (double dCoords [3]);        /* 2D conversion without Critical Section */
    INT32 TransformPoint3D (double dCoords [3]);        /* 3D conversion without Critical Section */
    void InterpretStatus (INT32 status);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTRANSFORM_H_
