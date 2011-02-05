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

#ifndef FILL_H_
#define FILL_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION:
    // Class Fill is an abstract base that defines how to color/paint
    // feature geometry.
    //---------------------------------------------------------------------
    class MDFMODEL_API Fill : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        Fill();
        virtual ~Fill();

        // Operations
        // Property : FillPattern
        const MdfString& GetFillPattern() const;
        void SetFillPattern(const MdfString& strFillPattern);

        // Property : ForegroundColor
        const MdfString& GetForegroundColor() const;
        void SetForegroundColor(const MdfString& strForegroundColor);

        // Property : BackgroundColor
        const MdfString& GetBackgroundColor() const;
        void SetBackgroundColor(const MdfString& strBackgroundColor);

    private:
        // Data members
        // The Fill pattern that is used by this fill
        // during stylization of feature geometry.
        MdfString m_strFillPattern;

        // The Expression:Color string representation for the foreground color.
        MdfString m_strForeColor;

        // The Expression:Color string representation for the background color.
        MdfString m_strBackColor;
    };

END_NAMESPACE_MDFMODEL
#endif //FILL_H_
