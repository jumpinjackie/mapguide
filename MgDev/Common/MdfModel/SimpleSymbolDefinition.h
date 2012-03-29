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

#ifndef SIMPLESYMBOLDEFINITION_H_
#define SIMPLESYMBOLDEFINITION_H_

#include "MdfModel.h"
#include "SymbolDefinition.h"
#include "GraphicElement.h"
#include "ResizeBox.h"
#include "PointUsage.h"
#include "LineUsage.h"
#include "AreaUsage.h"
#include "Parameter.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API SimpleSymbolDefinition : public SymbolDefinition
    {
    public:
        // Construction, destruction, initialization
        SimpleSymbolDefinition();
        virtual ~SimpleSymbolDefinition();

        GraphicElementCollection* GetGraphics();

        ResizeBox* GetResizeBox();
        void AdoptResizeBox(ResizeBox* resizeBox);
        ResizeBox* OrphanResizeBox();

        PointUsage* GetPointUsage();
        void AdoptPointUsage(PointUsage* pointUsage);
        PointUsage* OrphanPointUsage();

        LineUsage* GetLineUsage();
        void AdoptLineUsage(LineUsage* lineUsage);
        LineUsage* OrphanLineUsage();

        AreaUsage* GetAreaUsage();
        void AdoptAreaUsage(AreaUsage* areaUsage);
        AreaUsage* OrphanAreaUsage();

        ParameterCollection* GetParameterDefinition();

        virtual void AcceptVisitor(ISymbolDefinitionVisitor& isdVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        SimpleSymbolDefinition(const SimpleSymbolDefinition&);
        SimpleSymbolDefinition& operator=(const SimpleSymbolDefinition&);

        // Data members
        GraphicElementCollection m_collGraphics;
        ResizeBox* m_pResizeBox;
        PointUsage* m_pPointUsage;
        LineUsage* m_pLineUsage;
        AreaUsage* m_pAreaUsage;
        ParameterCollection m_collParameters;
    };

END_NAMESPACE_MDFMODEL
#endif // SIMPLESYMBOLDEFINITION_H_
