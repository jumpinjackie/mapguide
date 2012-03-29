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

#ifndef PATH_H_
#define PATH_H_

#include "MdfModel.h"
#include "GraphicElement.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION: Building block of the geometry of a SymbolDefinition
    //-------------------------------------------------------------------------
    class MDFMODEL_API Path : public GraphicElement
    {
    public:
        // Used with string properties
        static const wchar_t* sLineCapDefault;
        static const wchar_t* sLineCapValues;
        static const wchar_t* sLineJoinDefault;
        static const wchar_t* sLineJoinValues;

    public:
        // Construction, destruction, initialization
        Path();
        virtual ~Path();

        const MdfString& GetGeometry() const;
        void SetGeometry(const MdfString& geometry);

        const MdfString& GetFillColor() const;
        void SetFillColor(const MdfString& fillColor);

        const MdfString& GetLineColor() const;
        void SetLineColor(const MdfString& lineColor);

        const MdfString& GetLineWeight() const;
        void SetLineWeight(const MdfString& lineWeight);

        const MdfString& GetLineWeightScalable() const;
        void SetLineWeightScalable(const MdfString& lineWeightScalable);

        const MdfString& GetLineCap() const;
        void SetLineCap(const MdfString& lineCap);

        const MdfString& GetLineJoin() const;
        void SetLineJoin(const MdfString& lineJoin);

        const MdfString& GetLineMiterLimit() const;
        void SetLineMiterLimit(const MdfString& lineMiterLimit);

        const MdfString& GetScaleX() const;
        void SetScaleX(const MdfString& scaleX);

        const MdfString& GetScaleY() const;
        void SetScaleY(const MdfString& scaleY);

        virtual void AcceptVisitor(IGraphicElementVisitor& igeVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        Path(const Path&);
        Path& operator=(const Path&);

        // Data members
        MdfString m_sGeometry;
        MdfString m_sFillColor;
        MdfString m_sLineColor;
        MdfString m_sLineWeight;
        MdfString m_sLineWeightScalable;
        MdfString m_sLineCap;
        MdfString m_sLineJoin;
        MdfString m_sLineMiterLimit;
        MdfString m_sScaleX;
        MdfString m_sScaleY;
    };

END_NAMESPACE_MDFMODEL
#endif // PATH_H_
