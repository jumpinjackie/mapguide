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

#ifndef _CCOORDSYSGEODETICSTANDALONETRANSFORMDEFPARAMS_H_
#define _CCOORDSYSGEODETICSTANDALONETRANSFORMDEFPARAMS_H_

namespace CSLibrary
{
    class CCoordinateSystemGeodeticStandaloneTransformDefParams :
        public MgCoordinateSystemGeodeticStandaloneTransformDefParams,
        public CCoordinateSystemGeodeticTransformDefParams
    {
    public:
        CCoordinateSystemGeodeticStandaloneTransformDefParams(INT32 standaloneTransformMethod, bool isProtected);
        virtual ~CCoordinateSystemGeodeticStandaloneTransformDefParams();

        virtual void Dispose();
        virtual bool IsValid();

        virtual bool IsProtected();
        virtual void CopyTo(void* target) const;

        virtual INT32 GetTransformationMethod();
        virtual void SetTransformationMethod(INT32 standaloneMethodCode);

    private:
        INT32 m_nStandaloneMethodCode;
    };

} //namespace CSLibrary

#endif //_CCOORDSYSGEODETICSTANDALONETRANSFORMDEFPARAMS_H_
