//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_STYLEVISITOR_H
#define SE_STYLEVISITOR_H

#include "SE_ExpressionBase.h"
#include "IGraphicElementVisitor.h"
#include "ISymbolDefinitionVisitor.h"

class SE_SymbolManager;
class SE_LineBuffer;
class SE_LineBufferPool;
struct SE_Style;
struct SE_RenderStyle;
struct SE_Primitive;
struct SE_RenderPrimitive;
struct SE_Symbolization;
struct SE_Matrix;

namespace MDFMODEL_NAMESPACE
{
    class CompositeSymbolization;
    class SimpleSymbolDefinition;
    class CompoundSymbolDefinition;
}

class SE_StyleVisitor : public MdfModel::IGraphicElementVisitor, public MdfModel::ISymbolDefinitionVisitor, private SE_ExpressionBase
{
public:
    SE_StyleVisitor(SE_SymbolManager* resources, SE_LineBufferPool* lbp);

    /* IGraphicElementVisitor */
    virtual void VisitPath(MdfModel::Path& path);
    virtual void VisitImage(MdfModel::Image&);
    virtual void VisitText(MdfModel::Text&);

    /* ISymbolDefinitionVisitor */
    virtual void VisitSimpleSymbolDefinition(MdfModel::SimpleSymbolDefinition& simpleSymbol);
    virtual void VisitCompoundSymbolDefinition(MdfModel::CompoundSymbolDefinition& compoundSymbol);

    void ProcessPointUsage(MdfModel::PointUsage& pointUsage);
    void ProcessLineUsage(MdfModel::LineUsage& lineUsage);
    void ProcessAreaUsage(MdfModel::AreaUsage& areaUsage);

    void Convert(std::vector<SE_Symbolization*>& styles, MdfModel::CompositeSymbolization* symbolization);
private:
    SE_Style* ParseSymbol(MdfModel::CompoundSymbolDefinition* symbol);
    void ParseSymbol(SimpleSymbolDefinition* symbol, SE_Matrix& xform, SE_Symbol* primitives, SE_RenderSymbol* renderPrimitives);
    bool ParseDouble(const wchar_t*& str, double& val);
    bool ParseDoublePair(const wchar_t*& str, double& x, double& y);
    bool ParseGeometry(const MdfModel::MdfString& geometry, SE_LineBuffer& buffer);

    SE_SymbolManager* m_resources;
    SE_LineBufferPool* m_lbp;
    SE_Symbolization* m_symbolization;
    SE_Style* m_style;
    SE_Primitive* m_primitive;
};

#endif // SE_STYLEVISITOR_H
