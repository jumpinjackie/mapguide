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

#ifndef _CCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_
#define _CCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_


namespace CSLibrary
{
    typedef cs_GeodeticTransform_::csGeodeticXformParameters::csGeodeticXformParmsGridFiles_ csGridFileXformParams;

    class CCoordinateSystemGeodeticInterpolationTransformDefParams :
        public MgCoordinateSystemGeodeticInterpolationTransformDefParams,
        public CCoordinateSystemGeodeticTransformDefParams
    {
    public:
        CCoordinateSystemGeodeticInterpolationTransformDefParams(const csGridFileXformParams& params, bool isProtected);
        ~CCoordinateSystemGeodeticInterpolationTransformDefParams();

        // >>> MgCoordinateSystemGeodeticInterpolationTransformDefParams
        virtual void SetGridFiles(MgDisposableCollection* gridFiles);
        virtual MgDisposableCollection* GetGridFiles();

        virtual STRING GetFallback();
        virtual void SetFallback(CREFSTRING);

        virtual MgCoordinateSystemGeodeticTransformGridFile* NewGridFile();

        // <<< MgCoordinateSystemGeodeticInterpolationTransformDefParams

        virtual bool IsValid(); //from MgCoordinateSystemGeodeticTransformDefParams
        virtual bool IsProtected();

        virtual void Dispose();

    private:
        void CopyTo(void* target) const;

        void Reset();
        void ReleaseInstance();

    private:
        csGridFileXformParams* gridFileParams;
    };

} //namespace CSLibrary

#endif //_CCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_
