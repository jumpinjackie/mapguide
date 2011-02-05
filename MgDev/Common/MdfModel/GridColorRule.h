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

#ifndef GRIDCOLORRULE_H_
#define GRIDCOLORRULE_H_

#include "Rule.h"
#include "MdfOwnerCollection.h"
#include "GridColor.h"
#include <vector>

BEGIN_NAMESPACE_MDFMODEL

    //-----------------------------------------------------------------------------
    // DESCRIPTION:
    // Class GridColorRule includes a filter, a grid color, and a legned label.
    // It encapsulates a grid color for a grid source. For each pixel, if the filter
    // expression is true, then the grid color will fill the pixel.
    //-----------------------------------------------------------------------------
    class MDFMODEL_API GridColorRule : public Rule
    {
    public:
        GridColorRule();
        virtual ~GridColorRule();

        // Property: GridColor
        const GridColor* GetGridColor() const;
        GridColor* GetGridColor();
        void AdoptGridColor(GridColor *pGridColor);
        GridColor* OrphanGridColor();

    private:
        // Hidden copy constructor and assignment operator.
        GridColorRule(const GridColorRule&);
        GridColorRule& operator = (const GridColorRule&);

        // Grid Color
        GridColor* m_spGridColor;
    };

END_NAMESPACE_MDFMODEL
#endif // GRIDCOLORRULE_H_
