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

#ifndef STROKE_H_
#define STROKE_H_

#include "MdfModel.h"
#include "SizeContexts.h"
#include "LengthConverter.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // A Stroke class defines a way to stylize lines. An abstract class, its
    // concrete classes are used by LineSymbolization and AreaSymbolization to
    // define the stylization for lines and polygon outlines.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Stroke : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        Stroke();
        virtual ~Stroke();

        // Operations
        // Property : LineStyle
        const MdfString& GetLineStyle() const;
        void SetLineStyle(const MdfString& strLineStyle);

        // Property : Unit
        LengthUnit GetUnit() const;
        void SetUnit(LengthUnit eUnit);

        // Property : Thickness  Type : Expression:Double
        const MdfString& GetThickness() const;
        void SetThickness(const MdfString& strThicknessExpr);

        // Property : ForegroundColor
        const MdfString& GetColor() const;
        void SetColor(const MdfString& strColor);

        // Property : SizeContext
        SizeContext GetSizeContext() const;
        void SetSizeContext(SizeContext sizeContext);

    private:
        // Data members

        // The LineStyle stored on the Server.
        MdfString m_strLineStyle;

        // Enum that specifies if the thickness mentioned is in
        // screen space or map space.
        SizeContext m_eSizeContext;

        // Enum that specifies the unit in which thickness is defined.
        LengthUnit m_eUnit;

        // A string representation of the Thickness Expression:Int.
        MdfString m_strThickness;

        // The Expression:Color string representation for the color.
        MdfString m_strColor;
    };

END_NAMESPACE_MDFMODEL
#endif // STROKE_H_
