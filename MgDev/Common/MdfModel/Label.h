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

#ifndef LABEL_H_
#define LABEL_H_

#include "MdfModel.h"
#include "TextSymbol.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // A Label class is essentially a PointSymbolization2D. Labels are placed
    // relative to an origin point that is determined for each feature. A single
    // TextSymbol can be used for the Symbol property to provide a traditional
    // text label. Other types of Symbols can be used by themselves, or in
    // combination, to create more intricate labels.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Label : public MdfRootObject
    {
    public:
        // Orientation Enum is used to control the orientation of a Label with
        // Construction, destruction, initialization
        Label();
        virtual ~Label();

        // Operations
        // Property : Symbol
        TextSymbol* GetSymbol();
        void AdoptSymbol(TextSymbol* pflAdoptedSymbol);
        TextSymbol* OrphanSymbol();

    private:
        // Hidden copy constructor and assignment operator.
        Label(const Label&);
        Label& operator=(const Label&);

        // Data members
        // the label's symbol.
        TextSymbol* m_Symbol;
    };

END_NAMESPACE_MDFMODEL
#endif //LABEL_H_
