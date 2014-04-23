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

#ifndef TEXTFRAME_H_
#define TEXTFRAME_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API TextFrame : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        TextFrame();
        virtual ~TextFrame();

        const MdfString& GetLineColor() const;
        void SetLineColor(const MdfString& lineColor);

        const MdfString& GetFillColor() const;
        void SetFillColor(const MdfString& fillColor);

        const MdfString& GetOffsetX() const;
        void SetOffsetX(const MdfString& offsetX);

        const MdfString& GetOffsetY() const;
        void SetOffsetY(const MdfString& offsetY);

    private:
        // Hidden copy constructor and assignment operator.
        TextFrame(const TextFrame&);
        TextFrame& operator=(const TextFrame&);

        // Data members
        MdfString m_sLineColor;
        MdfString m_sFillColor;
        MdfString m_sOffsetX;
        MdfString m_sOffsetY;
    };

END_NAMESPACE_MDFMODEL
#endif // TEXTFRAME_H_
