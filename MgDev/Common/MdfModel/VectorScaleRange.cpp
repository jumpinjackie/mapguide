//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The VectorScaleRange class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "VectorScaleRange.h"
#include "AreaRule.h"
#include "LineRule.h"
#include "PointRule.h"
#include "CompositeRule.h"

#include "BlockSymbol.h"
#include "FontSymbol.h"
#include "ImageSymbol.h"
#include "MarkSymbol.h"
#include "TextSymbol.h"
#include "W2DSymbol.h"

using namespace MDFMODEL_NAMESPACE;

// we must undefine the max macro so the limits max function compiles correctly
#include <limits>
using namespace std;
#undef max

//-------------------------------------------------------------------------
// Contants : Represents the maximum value for the map scale.
//-------------------------------------------------------------------------
const double VectorScaleRange::MAX_MAP_SCALE = 1000000000000.0; // one trillion

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the VectorScaleRange class.
//-------------------------------------------------------------------------
VectorScaleRange::VectorScaleRange()
{
    // default values
    this->m_dMinScale = 0.0;
    this->m_dMaxScale = MAX_MAP_SCALE;
    this->m_elevationSettings = NULL;
    this->m_usedColorList = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
VectorScaleRange::~VectorScaleRange()
{
    // delete members which are pointers explicitely
    if (this->m_elevationSettings != NULL)
        delete this->m_elevationSettings;
    if (this->m_usedColorList != NULL)
        delete this->m_usedColorList;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value component of this VectorScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The minimum value.
//-------------------------------------------------------------------------
double VectorScaleRange::GetMinScale() const
{
    return this->m_dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the minimum value component of this VectorScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMinScale - the minimum value.
//-------------------------------------------------------------------------
void VectorScaleRange::SetMinScale(const double& dMinScale)
{
    this->m_dMinScale = dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value component of this VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The maximum value.
//-------------------------------------------------------------------------
double VectorScaleRange::GetMaxScale() const
{
    return this->m_dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the maximum value component of this VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMaxScale - The maximum value.
//-------------------------------------------------------------------------
void VectorScaleRange::SetMaxScale(const double& dMaxScale)
{
    this->m_dMaxScale = dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the collection of FeatureTypeStyles
//          defined in this VectorScaleRange.
// RETURNS: A pointer to a modifiable FeatureTypeStyleCollection.
//-------------------------------------------------------------------------
FeatureTypeStyleCollection* VectorScaleRange::GetFeatureTypeStyles()
{
    return &this->m_collFeatureTypeStyles;
}

//-------------------------------------------------------------------------
// PURPOSE: Retrieves the elevation settings for this scale range
// RETURNS: A pointer to the ElevationSettings object
//-------------------------------------------------------------------------
ElevationSettings* VectorScaleRange::GetElevationSettings()
{
    return this->m_elevationSettings;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt an ElevationSettings object
// PARAMETERS:
//      Input:
//         elevationSettings - A pointer to the ElevationSettings object
//-------------------------------------------------------------------------
void VectorScaleRange::AdoptElevationSettings(ElevationSettings* elevationSettings)
{
    if (this->m_elevationSettings != elevationSettings)
    {
        if (this->m_elevationSettings != NULL)
            delete this->m_elevationSettings;
        this->m_elevationSettings = elevationSettings;
    }
}

//-------------------------------------------------------------------------
/// PURPOSE: Accessor method for the base colors defined in this Layer and scaleRange.
/// RETURNS: A pointer to the list of colors of the collected colors (maybe empty but not null)
//-------------------------------------------------------------------------
PSTRCOLORLIST VectorScaleRange::GetUsedColorList()
{
    // lazy instantiation
    if (this->m_usedColorList)	return this->m_usedColorList;
    // this should be sufficiently thread safe as the object is created immediately
    PSTRCOLORLIST usedColorList = this->m_usedColorList = new STRCOLORLIST();
    // compute new color list by iterating through the featuretypecollection
    FeatureTypeStyleCollection* pftsColl = this->GetFeatureTypeStyles();
    int ftsccount = pftsColl->GetCount();
    for (int j=0; j< ftsccount; j++)
    {
        FeatureTypeStyle* pfts = pftsColl->GetAt(j);

        // iterate through the rulecollection
        RuleCollection* ruleColl = pfts->GetRules();
        int rccount = ruleColl->GetCount();
        for (int k=0; k < rccount; k++)
        {
            Rule* rule = ruleColl->GetAt(k);

            // get the label which will be the key in the color map
            const MdfString& label = rule->GetLegendLabel();

            // do the casting to access the relevant members
            // this is bad style (instead of virtual functions GetColors() in each subclass)
            // but we save touching too many different files
            AreaRule* paRule = dynamic_cast<AreaRule*>(rule);
            LineRule* plRule = dynamic_cast<LineRule*>(rule);
            PointRule* ppRule = dynamic_cast<PointRule*>(rule);
            //CompositeRule* pcRule = dynamic_cast<CompositeRule*>(rule);  // no colors in there

            // AreaRule Symbolization.....
            if (paRule != NULL)
            {					AreaSymbolization2D* pasym = paRule->GetSymbolization();
                if (pasym->GetFill() != NULL) 
                {	// create copies of all strings!!! so we can safely delete the resulting list later
                    usedColorList->push_back(pasym->GetFill()->GetForegroundColor().substr());
                    usedColorList->push_back(pasym->GetFill()->GetBackgroundColor().substr());
                }
                if (pasym->GetEdge() != NULL) 
                    usedColorList->push_back(pasym->GetEdge()->GetColor().substr());
            }

            // LineRule Symbolization.....
            if (plRule != NULL)
            {
                LineSymbolizationCollection* plsymcol = plRule->GetSymbolizations();
                // iterate through the linesymbolizations
                int lsccount = plsymcol->GetCount();
                for (int l=0; l < lsccount; l++)
                {
                    LineSymbolization2D* plsym = plsymcol->GetAt(l);
                    if (plsym->GetStroke() != NULL)
                        usedColorList->push_back(plsym->GetStroke()->GetColor().substr());
                }
            }
            // PointRule Symbolization.....
            if (ppRule != NULL)
            {
                PointSymbolization2D* ppsym = ppRule->GetSymbolization();
                if (ppsym)
                {
                    Symbol *sym   = ppsym->GetSymbol();
                    MdfModel::BlockSymbol* blockSymbol = dynamic_cast<MdfModel::BlockSymbol*>(sym);
                    MdfModel::FontSymbol* fontSymbol = dynamic_cast<MdfModel::FontSymbol*>(sym);
                    MdfModel::MarkSymbol* markSymbol = dynamic_cast<MdfModel::MarkSymbol*>(sym);
                    MdfModel::TextSymbol* textSymbol = dynamic_cast<MdfModel::TextSymbol*>(sym);
                    MdfModel::W2DSymbol* w2dSymbol = dynamic_cast<MdfModel::W2DSymbol*>(sym);
                    if (blockSymbol != NULL)
                    {
                        usedColorList->push_back(blockSymbol->GetBlockColor().substr());
                        usedColorList->push_back(blockSymbol->GetLayerColor().substr());
                    }
                    if (fontSymbol != NULL)
                    {
                        usedColorList->push_back(fontSymbol->GetForegroundColor().substr());
                    }
                    if (markSymbol != NULL)
                    {
                        if (markSymbol->GetEdge() != NULL)
                            usedColorList->push_back(markSymbol->GetEdge()->GetColor().substr());
                        if (markSymbol->GetFill() != NULL) {
                            usedColorList->push_back(markSymbol->GetFill()->GetForegroundColor().substr());
                            usedColorList->push_back(markSymbol->GetFill()->GetBackgroundColor().substr());
                        }
                    }
                    if (textSymbol != NULL)
                    {
                        usedColorList->push_back(textSymbol->GetForegroundColor().substr());
                        usedColorList->push_back(textSymbol->GetBackgroundColor().substr());
                    }
                    if (w2dSymbol != NULL)
                    {
                        usedColorList->push_back(w2dSymbol->GetFillColor().substr());
                        usedColorList->push_back(w2dSymbol->GetLineColor().substr());
                        usedColorList->push_back(w2dSymbol->GetTextColor().substr());
                    }
                } // if pointSymbolization
            } // end pointRule

        } // for GetRules
    } // for GetFeatureTypeStyles
    return this->m_usedColorList;
}
