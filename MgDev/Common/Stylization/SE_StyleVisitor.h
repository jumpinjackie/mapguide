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

#ifndef SE_STYLEVISITOR_H_
#define SE_STYLEVISITOR_H_

#include "SE_BufferPool.h"
#include "SE_ExpressionBase.h"
#include "SE_SymbolDefProxies.h"


namespace MDFMODEL_NAMESPACE
{
    class CompositeSymbolization;
    class SimpleSymbolDefinition;
    class CompoundSymbolDefinition;
}

class SE_StyleVisitor : public MdfModel::IGraphicElementVisitor, public MdfModel::ISymbolDefinitionVisitor, public SE_ExpressionBase
{
public:
    STYLIZATION_API SE_StyleVisitor(SE_SymbolManager* resources, SE_BufferPool* bp);

    /* IGraphicElementVisitor */
    virtual void VisitPath(MdfModel::Path& path);
    virtual void VisitImage(MdfModel::Image&);
    virtual void VisitText(MdfModel::Text&);

    /* ISymbolDefinitionVisitor */
    virtual void VisitSimpleSymbolDefinition(MdfModel::SimpleSymbolDefinition& simpleSymbol);
    virtual void VisitCompoundSymbolDefinition(MdfModel::CompoundSymbolDefinition& compoundSymbol);

    SE_PointStyle* ProcessPointUsage(MdfModel::PointUsage& pointUsage);
    SE_LineStyle* ProcessLineUsage(MdfModel::LineUsage& lineUsage);
    SE_AreaStyle* ProcessAreaUsage(MdfModel::AreaUsage& areaUsage);

    STYLIZATION_API void Convert(std::vector<SE_SymbolInstance*>& result, MdfModel::CompositeSymbolization* symbolization);

private:
    bool ParseDouble(const wchar_t*& str, double& val);
    bool ParseDoublePair(const wchar_t*& str, double& x, double& y);
    bool ParseGeometry(const MdfModel::MdfString& geometry, SE_LineBuffer& buffer);

    SE_SymbolManager* m_resources;
    SE_BufferPool* m_bp;
    SE_SymbolInstance* m_symbolInstance;
    SE_Style* m_style;
    SE_Primitive* m_primitive;
    std::vector<const wchar_t*> m_resIdStack;

    MdfModel::SymbolInstance::UsageContext m_usageContext;
};

#endif
