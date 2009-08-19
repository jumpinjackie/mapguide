//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

    virtual void IgnoreDatumShiftWarning(bool bIgnoreDatumShiftWarning);
    virtual bool IsIgnoreDatumShiftWarning();
    virtual void IgnoreOutsideDomainWarning(bool bIgnoreOutsideDomainWarning);
    virtual bool IsIgnoreOutsideDomainWarning();
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

    bool m_bIgnoreDatumShiftWarning;
    bool m_bIgnoreOutsideDomainWarning;
    bool m_bSourceTargetSame;
    INT32 m_nTransformStatus;

    //Private member functions
    void SetCatalog(MgCoordinateSystemCatalog *pCatalog);
    bool IsInitialized();
    void Uninitialize();
    void TransformPoint(double& x, double& y, double *pdZ);
    bool IsValidPoint(cs_Csprm_& csprm, double x, double y, double z);

private:
    //Unimplemented stuff
    CCoordinateSystemTransform(const CCoordinateSystemTransform&);
    CCoordinateSystemTransform& operator=(const CCoordinateSystemTransform&);

    inline void TransformPointInternal(double& x, double& y, double *pdZ, bool isGeographic,
        double lonMin, double lonMax, double latMin, double latMax);
    int TransformInverse (double& xx,double& yy);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTRANSFORM_H_
