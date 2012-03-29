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

#ifndef SYMBOLDEFINITION_H_
#define SYMBOLDEFINITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "ISymbolDefinitionVisitor.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API SymbolDefinition : public MdfRootObject
    {
    public:
        // Destruction
        virtual ~SymbolDefinition();

        void SetName(const MdfString& name);
        const MdfString& GetName() const;

        void SetDescription(const MdfString& description);
        const MdfString& GetDescription() const;

        virtual void AcceptVisitor(ISymbolDefinitionVisitor& isdVisitor) = 0;

    protected:
        // Construction, initialization
        // Default constructor is protected to make this class abstract.
        SymbolDefinition();

    private:
        // Data members
        MdfString m_sName;
        MdfString m_sDescription;
    };

END_NAMESPACE_MDFMODEL
#endif // SYMBOLDEFINITION_H_
