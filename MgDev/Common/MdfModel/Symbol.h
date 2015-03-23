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

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "MdfModel.h"
#include "SizeContexts.h"
#include "LengthConverter.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    class ISymbolVisitor;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The Symbol class is used to define a way to stylize features containing
    // point geometric primitives. Its use of expressions for its properties
    // allows it to support multivariate theming. Symbol is an abstract class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Symbol : public MdfRootObject
    {
    public:

        // Construction, destruction, initialization.
        Symbol();
        virtual ~Symbol();

        // Operations
        // Property : SizeContext
        SizeContext GetSizeContext() const;
        void SetSizeContext(SizeContext eSizeContext);

        // Property : Unit
        LengthUnit GetUnit() const;
        void SetUnit(LengthUnit eUnit);

        // Property : SizeX
        const MdfString& GetSizeX() const;
        void SetSizeX(const MdfString& strWidth);

        // Property : SizeY
        const MdfString& GetSizeY() const;
        void SetSizeY(const MdfString& strHeight);

        // Property : InsertionPointX
        const MdfString& GetInsertionPointX() const;
        void SetInsertionPointX(const MdfString& strWidth);

        // Property : InsertionPointY
        const MdfString& GetInsertionPointY() const;
        void SetInsertionPointY(const MdfString& strHeight);

        // Property : MaintainAspect
        bool GetMaintainAspect() const;
        void SetMaintainAspect(bool bMaintainAspect);

        // Property : Rotation  Type : Expression:Double
        const MdfString& GetRotation() const;
        void SetRotation(const MdfString& strRotationExpr);

        // Visitor Pattern method to be implemented by all concrete
        // subclasses.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor) = 0;

    private:
        // Data members
        // Enum that specifies if the size (X and Y) mentioned is in
        // screen space or map space.
        SizeContext m_eSizeContext;

        // Enum that specifies the unit in which size (X and Y) are defined.
        LengthUnit m_eUnit;

        // The S dimension of the symbol stored in meters
        MdfString m_strSizeX;

        // the Y dimension of the symbol stored in meters
        MdfString m_strSizeY;

        // The S dimension of the symbol stored in meters
        MdfString m_strInsertionPointX;

        // the Y dimension of the symbol stored in meters
        MdfString m_strInsertionPointY;

        // The string representation of the Expression:Double for the Rotation.
        MdfString m_strRotation;

        // A bool for the width and height to maintain their current aspect
        // ratio. Used by the UI while editing the Symbol.
        bool m_bMaintainAspect;
    };

END_NAMESPACE_MDFMODEL
#endif // SYMBOL_H_
