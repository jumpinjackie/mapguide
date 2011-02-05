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

#ifndef _CCOORDINATESYSTEMGEODETICTRANSFORMATION_H_
#define _CCOORDINATESYSTEMGEODETICTRANSFORMATION_H_

namespace CSLibrary
{

class CCoordinateSystemGeodeticTransformation : public MgCoordinateSystemGeodeticTransformation
{
public:
    CCoordinateSystemGeodeticTransformation(MgCoordinateSystemCatalog* pCatalog, MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget);
    CCoordinateSystemGeodeticTransformation(MgCoordinateSystemCatalog* pCatalog, MgCoordinateSystemGeodeticTransformDef* transformationDef, bool createInversed);
    virtual ~CCoordinateSystemGeodeticTransformation();

    virtual void SetSourceAndTarget(MgCoordinateSystemDatum *pSource, MgCoordinateSystemDatum *pTarget);
    virtual MgCoordinateSystemDatum* GetSource();
    virtual MgCoordinateSystemDatum* GetTarget();

    virtual void Shift(MgCoordinate* pLonLat);
    virtual MgCoordinate* Shift(double dLongitude, double dLatitude);
    virtual MgCoordinate* Shift(double dLongitude, double dLatitude, double dZ);

    virtual double GetOffsetX();
    virtual double GetOffsetY();
    virtual double GetOffsetZ();
    virtual void SetOffset(double x, double y, double z);
    virtual double GetBursaWolfeTransformRotationX();
    virtual double GetBursaWolfeTransformRotationY();
    virtual double GetBursaWolfeTransformRotationZ();
    virtual double GetBursaWolfeTransformBwScale();
    virtual void SetBursaWolfeTransform(double dRotationX, double dRotationY, double dRotationZ, double dBwScale);
    virtual INT32 GetGeodeticTransformationMethod();
    virtual void SetGeodeticTransformationMethod(INT32 nGeodeticTransformationMethod);
    virtual double GetMaxOffset();
    virtual bool IsLegalOffset(double dOffset);
    virtual double GetMaxRotation();
    virtual bool IsLegalRotation(double dRotation);
    virtual double GetBwScaleMin();
    virtual double GetBwScaleMax();
    virtual bool IsLegalBwScale(double dBwScale);

protected:
    //from MgDisposable
    virtual void Dispose();

protected:
    //Private member functions
    bool IsInitialized() const;
    void Uninitialize();
    MgCoordinateSystemDatum* GetWGS84IfNull(MgCoordinateSystemDatum *pDatum);
    void SetCatalog(MgCoordinateSystemCatalog* pCatalog);
    bool GetDefinitionForGeodeticTransformationParameter(cs_Dtdef_& def);
    MgCoordinateSystemDatum* GetWGS84();
    void SetupFromTransformationDef(MgCoordinateSystemGeodeticTransformDef* transformationDef, bool createInversed);

protected:
    //Data members
    MgCoordinateSystemDatum* m_pDtSource;
    MgCoordinateSystemDatum* m_pDtTarget;
    cs_Dtcprm_ *m_pDtcprm;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

private:
    //Unimplemented stuff
    CCoordinateSystemGeodeticTransformation(const CCoordinateSystemGeodeticTransformation&);
    CCoordinateSystemGeodeticTransformation& operator=(const CCoordinateSystemGeodeticTransformation&);
    CCoordinateSystemGeodeticTransformation();
};

} // End of namespace

#endif //_CCOORDINATESYSTEMGEODETICTRANSFORMATION_H_
