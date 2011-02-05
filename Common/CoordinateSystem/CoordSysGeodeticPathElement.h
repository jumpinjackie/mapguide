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


#ifndef _COORDSYSGEODETICPATHELEMENT_H_
#define _COORDSYSGEODETICPATHELEMENT_H_

namespace CSLibrary
{
    class CCoordinateSystemGeodeticPathElement : public MgCoordinateSystemGeodeticPathElement
    {
    public:
        CCoordinateSystemGeodeticPathElement(const cs_GeodeticPathElement_* const pathElement, bool isProtected);
        ~CCoordinateSystemGeodeticPathElement();

        virtual void Dispose();
        virtual bool IsValid();
        virtual bool IsProtected();

        virtual STRING GetTransformName();
        virtual void SetTransformName(CREFSTRING);

        virtual bool GetIsInversed();
        virtual void SetIsInversed(bool);

    private:
       void Reset(const cs_GeodeticPathElement_* const pathElement);
       void CleanupInstanceVariables();

    private:
        bool isProtected;
        cs_GeodeticPathElement_* pathElement;
    };

} //namespace CSLibrary

#endif //_COORDSYSGEODETICPATHELEMENT_H_
