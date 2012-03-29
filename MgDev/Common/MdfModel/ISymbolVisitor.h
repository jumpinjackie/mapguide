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

#ifndef ISYMBOLVISITOR_H_
#define ISYMBOLVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class MarkSymbol;
    class ImageSymbol;
    class FontSymbol;
    class W2DSymbol;
    class BlockSymbol;
    class TextSymbol;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ISymbolVisitor is an interface that is used for determining the
    // the concrete class type of a pointer or reference to the abstract Symbol
    // class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API ISymbolVisitor
    {
    public:
        virtual void VisitMarkSymbol(MarkSymbol& symMark) = 0;
        virtual void VisitImageSymbol(ImageSymbol& symImage) = 0;
        virtual void VisitFontSymbol(FontSymbol& symFont) = 0;
        virtual void VisitW2DSymbol(W2DSymbol& symW2D) = 0;
        virtual void VisitBlockSymbol(BlockSymbol& symBlock) = 0;
        virtual void VisitTextSymbol(TextSymbol& symText) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // ISYMBOLVISITOR_H_
