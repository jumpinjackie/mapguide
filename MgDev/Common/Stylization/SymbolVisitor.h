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

#ifndef SYMBOLVISITOR_H_
#define SYMBOLVISITOR_H_

#include "Stylization.h"

//a visitor implementation that determines the feature type style
//we are dealing with
class SymbolVisitor : public MdfModel::ISymbolVisitor
{
public:
    enum eSymbolType
    {
        stUnknown    = 0,
        stMark       = 1,
        stBlock      = 2,
        stW2D        = 3,
        stFont       = 4,
        stImage      = 5,
        stText       = 6    //will not get that as a point symbol -- it's for labels only
    };

    SymbolVisitor()
    {
        m_type = stUnknown;
    }

    virtual void VisitMarkSymbol(MdfModel::MarkSymbol&)
    {
        m_type = stMark;
    }

    virtual void VisitImageSymbol(MdfModel::ImageSymbol&)
    {
        m_type = stImage;
    }

    virtual void VisitFontSymbol(MdfModel::FontSymbol&)
    {
        m_type = stFont;
    }

    virtual void VisitW2DSymbol(MdfModel::W2DSymbol&)
    {
        m_type = stW2D;
    }

    virtual void VisitBlockSymbol(MdfModel::BlockSymbol&)
    {
        m_type = stBlock;
    }

    virtual void VisitTextSymbol(MdfModel::TextSymbol&)
    {
        //should not get this type of symbol
        m_type = stText;
    }

    eSymbolType GetSymbolType()
    {
        return m_type;
    }

    //static helper
    static eSymbolType DetermineSymbolType(MdfModel::Symbol* symbol)
    {
        SymbolVisitor vis;
        symbol->AcceptVisitor(vis);
        return vis.GetSymbolType();
    }

private:
    eSymbolType m_type;
};

#endif
