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

#ifndef GRIDCOLORSTYLE_H_
#define GRIDCOLORSTYLE_H_

#include "FeatureTypeStyle.h"
#include "GridColorRule.h"
#include "HillShade.h"
#include <vector>

BEGIN_NAMESPACE_MDFMODEL

    //-----------------------------------------------------------------------------
    // DESCRIPTION:
    // Class GridColorStyle is a kind of FeatureTypeStyle. It is used to specify
    // how to style each pixel. It includes some GridColorRules. For each pixel, it
    // one rule's filter expression is true, then that rule's grid color will stylize
    // the pixel.
    //-----------------------------------------------------------------------------
    class MDFMODEL_API GridColorStyle : public FeatureTypeStyle
    {
    public:
        // Construction, destruction, initialization
        GridColorStyle();
        virtual ~GridColorStyle();

        // Property : HillShade, optional.
        const HillShade* GetHillShade() const;
        HillShade* GetHillShade();
        void AdoptHillShade(HillShade* pHillShade);
        HillShade* OrphanHillShade();

        // Property: Transparency Color, optional
        const MdfString& GetTransparencyColor() const;
        void SetTransparencyColor(const MdfString& strTransparencyColor);

        // Property: Brightness Factor, optional
        // Use 0 if not specified
        double GetBrightnessFactor() const;
        void SetBrightnessFactor(double dBrightnessFactor);

        // Property: Contrast Factor, optional
        // Use 0 if not specified.
        double GetContrastFactor() const;
        void SetContrastFactor(double dContrastFactor);

        // Visitor Pattern to be implemented by all subclasses
        virtual void AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        GridColorStyle(const GridColorStyle&);
        GridColorStyle& operator = (const GridColorStyle&);

        // HillShade
        HillShade* m_spHillShade;

        // Transparency Color
        MdfString                m_strTransparencyColor;

        // Brightness Factor
        double                   m_dBrightnessFactor;

        // Contrast Factor
        double                   m_dContrastFactor;
    };

END_NAMESPACE_MDFMODEL
#endif // GRIDCOLORSTYLE_H_
