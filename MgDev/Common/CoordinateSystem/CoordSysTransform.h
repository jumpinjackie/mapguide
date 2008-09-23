//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTRANSFORM_H_
