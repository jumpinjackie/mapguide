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

#ifndef LINESYMBOLIZATION2D_H_
#define LINESYMBOLIZATION2D_H_

#include "Symbolization.h"
#include "MdfOwnerCollection.h"
#include "Stroke.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The LineSymbolization2D class defines a way to specify the stylization
    // details for features with line or polyline geometric primitives.
    //-------------------------------------------------------------------------
    class MDFMODEL_API LineSymbolization2D : public Symbolization
    {
    public:
        // Construction, destruction, initialization
        LineSymbolization2D();
        virtual ~LineSymbolization2D();

        // Property : Stroke
        Stroke* GetStroke();
        void AdoptStroke(Stroke* pstkAdoptedStroke);
        Stroke* OrphanStroke();

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolizationVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        LineSymbolization2D(const LineSymbolization2D&);
        LineSymbolization2D& operator=(LineSymbolization2D&);

        // Data members
        // The Stroke subclass that is applied by this LineSymbolization2D
        // to line or polyline geometric primitives.
        Stroke* m_pstkStroke;
    };

    typedef MdfOwnerCollection<LineSymbolization2D> LineSymbolizationCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<LineSymbolization2D>;

END_NAMESPACE_MDFMODEL
#endif //LINESYMBOLIZATION2D_H_
