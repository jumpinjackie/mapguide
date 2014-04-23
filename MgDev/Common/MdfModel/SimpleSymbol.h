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

#ifndef SIMPLESYMBOL_H_
#define SIMPLESYMBOL_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "SimpleSymbolDefinition.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API SimpleSymbol : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        SimpleSymbol();
        virtual ~SimpleSymbol();

        SimpleSymbolDefinition* GetSymbolDefinition();
        void AdoptSymbolDefinition(SimpleSymbolDefinition* symbolDefinition);
        SimpleSymbolDefinition* OrphanSymbolDefinition();

        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& resourceId);

        const MdfString& GetRenderingPass() const;
        void SetRenderingPass(const MdfString& renderingPass);

    private:
        // Hidden copy constructor and assignment operator.
        SimpleSymbol(const SimpleSymbol&);
        SimpleSymbol& operator=(const SimpleSymbol&);

        // Data members
        SimpleSymbolDefinition* m_pSymbolDefinition;
        MdfString m_sResourceId;
        MdfString m_sRenderingPass;
    };

    typedef MdfOwnerCollection<SimpleSymbol> SimpleSymbolCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<SimpleSymbol>;

END_NAMESPACE_MDFMODEL
#endif // SIMPLESYMBOL_H_
