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


#ifndef _CCOORDINATESYSTEMGEODETICPATH_H_
#define _CCOORDINATESYSTEMGEODETICPATH_H_

namespace CSLibrary
{

    class CCoordinateSystemGeodeticPath : public MgCoordinateSystemGeodeticPath
    {
    public:
        CCoordinateSystemGeodeticPath(MgCoordinateSystemCatalog* pCatalog);
        ~CCoordinateSystemGeodeticPath();

        virtual void Dispose();

        void Initialize(const cs_GeodeticPath_& pathDef);
        void Reset();

        virtual MgCoordinateSystemGeodeticPath* CreateClone();
        virtual MgCoordinateSystemGeodeticPathElement* NewPathElement();

        //properties
        virtual STRING GetPathName();
        virtual void SetPathName(CREFSTRING);

        virtual STRING GetDescription();
        virtual void SetDescription(CREFSTRING);

        virtual STRING GetGroup();
        virtual void SetGroup(CREFSTRING);

        virtual STRING GetSource();
        virtual void SetSource(CREFSTRING);

        virtual STRING GetTargetDatum();
        virtual void SetTargetDatum(CREFSTRING);

        virtual STRING GetSourceDatum();
        virtual void SetSourceDatum(CREFSTRING);

        virtual double GetAccuracy();
        virtual void SetAccuracy(double accuracy);

        virtual bool GetIsReversible();
        virtual void SetIsReversible(bool isReversible);

        virtual INT32 GetEpsgCode();
        virtual void SetEpsgCode(INT32 epsgCode);

        virtual INT32 GetEpsgVariant();
        virtual void SetEpsgVariant(INT32 epsgVariant);

        virtual MgDisposableCollection* GetPathElements();
        virtual void SetPathElements(MgDisposableCollection* pathElements);

        virtual UINT8* SerializeFrom(UINT8* pStream);
        virtual UINT8* SerializeTo(UINT8* pStream);
        virtual UINT32 GetSizeSerialized();

        //helper - don't delete
        virtual bool IsEncrypted();

        virtual bool IsProtected();
        virtual bool IsValid();

        void CopyTo(cs_GeodeticPath_& pathDef) const;

    private:
        void CleanupInstanceVariables();

    private:
        cs_GeodeticPath_* pathDefinition;
        Ptr<MgCoordinateSystemCatalog> catalog;
       
        enum CCoordinateSystemGeodeticPathObjectVersions
        {
            kGpRelease0  = 0   // Initial Release
        };
    };

} //namespace CSLibrary

#endif //_CCOORDINATESYSTEMGEODETICPATH_H_
