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

#ifndef COMPOUNDSYMBOLDEFINITION_H_
#define COMPOUNDSYMBOLDEFINITION_H_

#include "MdfModel.h"
#include "SymbolDefinition.h"
#include "SimpleSymbol.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API CompoundSymbolDefinition : public SymbolDefinition
    {
    public:
        // Construction, destruction, initialization
        CompoundSymbolDefinition();
        virtual ~CompoundSymbolDefinition();

        SimpleSymbolCollection* GetSymbols();

        virtual void AcceptVisitor(ISymbolDefinitionVisitor& isdVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        CompoundSymbolDefinition(const CompoundSymbolDefinition&);
        CompoundSymbolDefinition& operator=(const CompoundSymbolDefinition&);

        // Data members
        SimpleSymbolCollection m_collSymbols;
    };

END_NAMESPACE_MDFMODEL
#endif // COMPOUNDSYMBOLDEFINITION_H_
