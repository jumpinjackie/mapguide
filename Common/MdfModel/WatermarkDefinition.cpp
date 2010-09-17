//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
// The WatermarkDefinition class implementation.
// All the data objects in the WatermarkDefinition (Content, Appearance, Position etc) 
// are accessible. While unfettered access to all the stored objects in provided, 
// these data are still owned by the WatermarkDefinition object.
// Methods that have a prefix of "Adopt", imply that the object passed to them
// has to be created on the heap and the WaterDefinition object is now responsible 
// for its deletion. Methods that have a prefix of "Orphan" imply that the pointer
// returned, points to an object on the heap and its is the caller who is responsible
// for its deletion. Methods that have a "Get" prefix simply give access without
// a change of ownership.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "WatermarkDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the WatermarkDefinition class.
//-------------------------------------------------------------------------
WatermarkDefinition::WatermarkDefinition()
    :m_source(NULL), m_appearance(NULL), m_position(NULL)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
WatermarkDefinition::~WatermarkDefinition()
{
    delete this->m_source;
    delete this->m_appearance;
}


//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the source property.
//          The SymbolDefinition is the type of source content used in watermark.
// RETURNS: The pointer of symbol definition.
//-------------------------------------------------------------------------
const SymbolDefinition* WatermarkDefinition::GetSource() const
{
    return this->m_source;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the source property.
//          The SymbolDefinition is the type of source content used in watermark.
// RETURNS: The pointer of symbol definition.
//-------------------------------------------------------------------------
SymbolDefinition* WatermarkDefinition::GetSource()
{
    return this->m_source;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the source property.
//          The SymbolDefinition is the type of source content used in watermark.
// PARAMETERS:
//        Input:
//          pSource - Adopted SymbolDefinition object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkDefinition::AdoptSource(SymbolDefinition *pSource)
{
    if (this->m_source != pSource)
    {
        delete this->m_source;
        this->m_source = pSource;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the source property.
//          The SymbolDefinition is the type of source content used in watermark.
// RETURNS: The pointer to the orphaned SymbolDefinition object. It may be NULL.
//-------------------------------------------------------------------------
SymbolDefinition* WatermarkDefinition::OrphanSource()
{
    SymbolDefinition* pRet = this->m_source;
    this->m_source = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance property.
//          The WatermarkAppearance is the type of appearance in watermark.
// RETURNS: The pointer of watermark appearance.
//-------------------------------------------------------------------------
const WatermarkAppearance* WatermarkDefinition::GetAppearance() const
{
    return this->m_appearance;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance property.
//          The WatermarkAppearance is the type of appearance in watermark.
// RETURNS: The pointer of watermark appearance.
//-------------------------------------------------------------------------
WatermarkAppearance* WatermarkDefinition::GetAppearance()
{
    return this->m_appearance;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance property.
//          The WatermarkAppearance is the type of appearance in watermark.
// PARAMETERS:
//        Input:
//          pAppearance - Adopted WatermarkAppearance object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkDefinition::AdoptAppearance(WatermarkAppearance *pAppearance)
{
    if (this->m_appearance != pAppearance)
    {
        delete this->m_appearance;
        this->m_appearance = pAppearance;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance property.
//          The WatermarkAppearance is the type of appearance in watermark.
// RETURNS: The pointer to the orphaned WatermarkAppearance object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkAppearance* WatermarkDefinition::OrphanAppearance()
{
    WatermarkAppearance* pRet = this->m_appearance;
    this->m_appearance = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property.
//          The WatermarkPosition is the type of position in watermark.
// RETURNS: The pointer of watermark position.
//-------------------------------------------------------------------------
const WatermarkPosition* WatermarkDefinition::GetPosition() const
{
    return this->m_position;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property.
//          The WatermarkPosition is the type of position in watermark.
// RETURNS: The pointer of watermark position.
//-------------------------------------------------------------------------
WatermarkPosition* WatermarkDefinition::GetPosition()
{
    return this->m_position;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property.
//          The WatermarkPosition is the type of position in watermark.
// PARAMETERS:
//        Input:
//          pPosition - Adopted WatermarkPosition object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkDefinition::AdoptPosition(WatermarkPosition *pPosition)
{
    if (this->m_position != pPosition)
    {
        delete this->m_position;
        this->m_position = pPosition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property.
//          The WatermarkPosition is the type of position in watermark.
// RETURNS: The pointer to the orphaned WatermarkPosition object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkPosition* WatermarkDefinition::OrphanPosition()
{
    WatermarkPosition* pRet = this->m_position;
    this->m_position = NULL;
    return pRet;
}

#ifdef _WIN32
#ifdef _DEBUG
int WatermarkDefinition::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
