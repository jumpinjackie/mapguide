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

#ifndef IGRAPHICELEMENTVISITOR_H_
#define IGRAPHICELEMENTVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class Path;
    class Image;
    class Text;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The IGraphicElementVisitor is an interface that is used for determining
    // the concrete class type of a pointer or reference to the abstract
    // GraphicElement class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API IGraphicElementVisitor
    {
    public:
        // Operations
        virtual void VisitPath(Path& path) = 0;
        virtual void VisitImage(Image& image) = 0;
        virtual void VisitText(Text& text) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // IGRAPHICELEMENTVISITOR_H_
