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

#ifndef _CCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H
#define _CCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H

namespace CSLibrary
{
    class CCoordinateSystemGeodeticTransformGridFile :
        public MgCoordinateSystemGeodeticTransformGridFile
    {
    public:
        CCoordinateSystemGeodeticTransformGridFile(const csGeodeticXfromParmsFile_* const fileFormatParam, bool isProtected);
        ~CCoordinateSystemGeodeticTransformGridFile();

        // >>> MgCoordinateSystemGeodeticTransformGridFile
        virtual INT32 GetFileFormat();
        virtual void SetFileFormat(INT32 gridFileFormat);

        virtual bool GetIsInverseDirection();
        virtual void SetIsInverseDirection(bool isInverseDirection);

        virtual STRING GetFileName();
        virtual void SetFileName(CREFSTRING fileName);

        virtual bool IsProtected();
        virtual bool IsValid();

        // <<< MgCoordinateSystemGeodeticTransformGridFile

        virtual void Dispose();

        virtual void CopyTo(csGeodeticXfromParmsFile_* destGridFile) const;

    private:
        void ReleaseInstance();

    private:
        csGeodeticXfromParmsFile_* gridFileDesc;
        bool isProtected;
    };
}

#endif //_CCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H
