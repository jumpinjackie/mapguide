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

#ifndef IMAGESYMBOL_H_
#define IMAGESYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The ImageSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D. It uses Mark shapes, which are 2D
    // polygons, along with specifications for coloring both the edges and the
    // inside of the shape for creating the symbol.
    //-------------------------------------------------------------------------
    class MDFMODEL_API ImageSymbol : public Symbol
    {
    public:
        // Construction, destruction, initialization.
        ImageSymbol();
        virtual ~ImageSymbol();

        // Operations
        // Property : Content
        const MdfString& GetContent() const;
        void SetContent(const MdfString& content);

        // Property : ImageLibrary
        const MdfString& GetImageLibrary() const;
        void SetImageLibrary(const MdfString& strImageLibrary);

        // Property : ImageName
        const MdfString& GetImageName() const;
        void SetImageName(const MdfString& strImageName);

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        ImageSymbol(const ImageSymbol&);
        ImageSymbol& operator=(const ImageSymbol&);

        // Data members
        // The width of the symbol stored in meters
        MdfString m_content;

        // The ImageLibrary that is used by this ImageSymbol.
        MdfString m_strImageLibrary;

        // The ImageName that is used by this ImageSymbol.
        MdfString m_strImageName;
   };

END_NAMESPACE_MDFMODEL
#endif // IMAGESYMBOL_H_
