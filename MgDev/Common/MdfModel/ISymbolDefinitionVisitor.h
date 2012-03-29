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

#ifndef ISYMBOLDEFINITIONVISITOR_H_
#define ISYMBOLDEFINITIONVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class SimpleSymbolDefinition;
    class CompoundSymbolDefinition;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ISymbolDefinitionVisitor is an interface that is used for determining
    // the concrete class type of a pointer or reference to the abstract
    // SymbolDefinition class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API ISymbolDefinitionVisitor
    {
    public:
        // Operations
        virtual void VisitSimpleSymbolDefinition(SimpleSymbolDefinition& simpleSymbol) = 0;
        virtual void VisitCompoundSymbolDefinition(CompoundSymbolDefinition& compoundSymbol) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // ISYMBOLDEFINITIONVISITOR_H_
