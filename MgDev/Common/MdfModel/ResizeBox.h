//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef RESIZEBOX_H_
#define RESIZEBOX_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API ResizeBox : public MdfRootObject
    {
    public:
        // Used with string properties
        static const wchar_t* sGrowControlDefault;
        static const wchar_t* sGrowControlValues;

    public:
        // Construction, destruction, initialization
        ResizeBox();
        virtual ~ResizeBox();

        const MdfString& GetSizeX() const;
        void SetSizeX(const MdfString& sizeX);

        const MdfString& GetSizeY() const;
        void SetSizeY(const MdfString& sizeY);

        const MdfString& GetPositionX() const;
        void SetPositionX(const MdfString& positionX);

        const MdfString& GetPositionY() const;
        void SetPositionY(const MdfString& positionY);

        const MdfString& GetGrowControl() const;
        void SetGrowControl(const MdfString& growControl);

    private:
        // Hidden copy constructor and assignment operator.
        ResizeBox(const ResizeBox&);
        ResizeBox& operator=(const ResizeBox&);

        // Data members
        MdfString m_sSizeX;
        MdfString m_sSizeY;
        MdfString m_sPositionX;
        MdfString m_sPositionY;
        MdfString m_sGrowControl;
    };

END_NAMESPACE_MDFMODEL
#endif // RESIZEBOX_H_
