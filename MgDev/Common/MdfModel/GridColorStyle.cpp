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

#include "stdafx.h"
#include "GridColorStyle.h"
#include "IFeatureTypeStyleVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColorStyle class
//-------------------------------------------------------------------------
GridColorStyle::GridColorStyle() : FeatureTypeStyle()
{
    // default values
    this->m_spHillShade = NULL;
    this->m_dBrightnessFactor = 0.0;
    this->m_dContrastFactor = 0.0;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColorStyle::~GridColorStyle()
{
    delete this->m_spHillShade;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HillShde property.
//          The HillShade is used to specify how to shade given a band and a light.
// RETURNS: The poniter of HillShade.
//-------------------------------------------------------------------------
const HillShade* GridColorStyle::GetHillShade() const
{
    return this->m_spHillShade;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HillShde property.
//          The HillShade is used to specify how to shade given a band and a light.
// RETURNS: The poniter of HillShade.
//-------------------------------------------------------------------------
HillShade* GridColorStyle::GetHillShade()
{
    return this->m_spHillShade;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HillShde property.
//          The HillShade is used to specify how to shade given a band and a light.
// PARAMETERS:
//        Input:
//          pHillShade - Adopted HillShade object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void GridColorStyle::AdoptHillShade(HillShade* pHillShade)
{
    if (this->m_spHillShade != pHillShade)
    {
        delete this->m_spHillShade;
        this->m_spHillShade = pHillShade;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HillShde property.
//          The HillShade is used to specify how to shade given a band and a light.
// RETURNS: The pointer to the orphaned HillShade object. It may be NULL.
//-------------------------------------------------------------------------
HillShade* GridColorStyle::OrphanHillShade()
{
    HillShade* pRet = this->m_spHillShade;
    this->m_spHillShade = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the TransparencyColor property.
//          The TransparencyColor is a string. If the pixel color is equal to the
//          the transparency color, then it will be set transparent.
// RETURNS: The Transparency Color.
//-------------------------------------------------------------------------
const MdfString& GridColorStyle::GetTransparencyColor() const
{
    return this->m_strTransparencyColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the TransparencyColor property.
//          The TransparencyColor is a string. If the pixel color is equal to the
//          the transparency color, then it will be set transparent.
// PARAMETERS:
//      Input:
//          strTransparencyColor - The transparency color.
//-------------------------------------------------------------------------
void GridColorStyle::SetTransparencyColor(const MdfString& strTransparencyColor)
{
    this->m_strTransparencyColor = strTransparencyColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BrightnessFactor property.
//          The BrightnessFactor is a double. It is used to adjust the pixel
//          color's brightness.
// RETURNS: The Brightness Factor.
//-------------------------------------------------------------------------
double GridColorStyle::GetBrightnessFactor() const
{
    return this->m_dBrightnessFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BrightnessFactor property.
//          The BrightnessFactor is a double. It is used to adjust the pixel
//          color's brightness.
// PARAMETERS:
//      Input:
//          dBrightnessFactor - The brightness factor.
//-------------------------------------------------------------------------
void GridColorStyle::SetBrightnessFactor(double dBrightnessFactor)
{
    this->m_dBrightnessFactor = dBrightnessFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ContrastFactor property.
//          The ContrastFactor is a double. It is used to adjust the pixel
//          color's contrast.
// RETURNS: The Contrast Factor.
//-------------------------------------------------------------------------
double GridColorStyle::GetContrastFactor() const
{
    return this->m_dContrastFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ContrastFactor property.
//          The ContrastFactor is a double. It is used to adjust the pixel
//          color's contrast.
// PARAMETERS:
//      Input:
//          dContrastFactor - The contrast factor.
//-------------------------------------------------------------------------
void GridColorStyle::SetContrastFactor(double dContrastFactor)
{
    this->m_dContrastFactor = dContrastFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in FeatureTypeStyle and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          iftsVisitor - The IFeatureTypeStyleVisitor interface which sports
//                        methods that accept the final concrete type this
//                        GridColorStyle represents as an argument.
//-------------------------------------------------------------------------
void GridColorStyle::AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor)
{
    iftsVisitor.VisitGridColorStyle(*this);
}
