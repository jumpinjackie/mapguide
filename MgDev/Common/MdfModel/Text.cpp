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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The Text class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Text.h"

using namespace MDFMODEL_NAMESPACE;

// initialize values for string properties
const wchar_t* Text::sHAlignmentDefault    = L"'Center'";                                  // NOXLATE
const wchar_t* Text::sHAlignmentValues     = L"Left|Center|Right";                         // NOXLATE
const wchar_t* Text::sVAlignmentDefault    = L"'Halfline'";                                // NOXLATE
const wchar_t* Text::sVAlignmentValues     = L"Bottom|Baseline|Halfline|Capline|Top";      // NOXLATE
const wchar_t* Text::sJustificationDefault = L"'FromAlignment'";                           // NOXLATE
const wchar_t* Text::sJustificationValues  = L"Left|Center|Right|Justified|FromAlignment"; // NOXLATE

// FontName and Markup are open-ended string properties - they only have default values,
// and not a fixed set of allowed values
const wchar_t* Text::sFontNameDefault      = L"'Arial'";                                   // NOXLATE
const wchar_t* Text::sMarkupDefault        = L"'Plain'";                                   // NOXLATE


//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Text class.
//-------------------------------------------------------------------------
Text::Text()
{
    // default values
    // NOTE: values used in IOText::Write must match these
//  this->m_sContent        = L"";         // NOXLATE
    this->m_sFontName       = Text::sFontNameDefault;
    this->m_sBold           = L"false";    // NOXLATE
    this->m_sItalic         = L"false";    // NOXLATE
    this->m_sUnderlined     = L"false";    // NOXLATE
    this->m_sOverlined      = L"false";    // NOXLATE
    this->m_sObliqueAngle   = L"0.0";      // NOXLATE
    this->m_sTrackSpacing   = L"1.0";      // NOXLATE
    this->m_sHeight         = L"4.0";      // NOXLATE
    this->m_sHeightScalable = L"true";     // NOXLATE
    this->m_sAngle          = L"0.0";      // NOXLATE
    this->m_sPositionX      = L"0.0";      // NOXLATE
    this->m_sPositionY      = L"0.0";      // NOXLATE
    this->m_sHAlignment     = Text::sHAlignmentDefault;
    this->m_sVAlignment     = Text::sVAlignmentDefault;
    this->m_sJustification  = Text::sJustificationDefault;
    this->m_sLineSpacing    = L"1.05";     // NOXLATE
    this->m_sTextColor      = L"ff000000"; // NOXLATE
//  this->m_sGhostColor     = L"";         // NOXLATE
    this->m_sMarkup         = Text::sMarkupDefault;

    this->m_pFrame = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Text::~Text()
{
    delete this->m_pFrame;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetContent() const
{
    return this->m_sContent;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetContent(const MdfString& content)
{
    this->m_sContent = content;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetFontName() const
{
    return this->m_sFontName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetFontName(const MdfString& fontName)
{
    this->m_sFontName = fontName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetBold() const
{
    return this->m_sBold;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetBold(const MdfString& bold)
{
    this->m_sBold = bold;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetItalic() const
{
    return this->m_sItalic;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetItalic(const MdfString& italic)
{
    this->m_sItalic = italic;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetUnderlined() const
{
    return this->m_sUnderlined;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetUnderlined(const MdfString& underlined)
{
    this->m_sUnderlined = underlined;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetOverlined() const
{
    return this->m_sOverlined;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetOverlined(const MdfString& overlined)
{
    this->m_sOverlined = overlined;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetObliqueAngle() const
{
    return this->m_sObliqueAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetObliqueAngle(const MdfString& obliqueAngle)
{
    this->m_sObliqueAngle = obliqueAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetTrackSpacing() const
{
    return this->m_sTrackSpacing;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetTrackSpacing(const MdfString& trackSpacing)
{
    this->m_sTrackSpacing = trackSpacing;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetHeight() const
{
    return this->m_sHeight;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetHeight(const MdfString& height)
{
    this->m_sHeight = height;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetHeightScalable() const
{
    return this->m_sHeightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetHeightScalable(const MdfString& heightScalable)
{
    this->m_sHeightScalable = heightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetAngle() const
{
    return this->m_sAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetAngle(const MdfString& angle)
{
    this->m_sAngle = angle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetPositionX() const
{
    return this->m_sPositionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetPositionX(const MdfString& positionX)
{
    this->m_sPositionX = positionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetPositionY() const
{
    return this->m_sPositionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetPositionY(const MdfString& positionY)
{
    this->m_sPositionY = positionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetHorizontalAlignment() const
{
    return this->m_sHAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetHorizontalAlignment(const MdfString& hAlignment)
{
    this->m_sHAlignment = hAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetVerticalAlignment() const
{
    return this->m_sVAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetVerticalAlignment(const MdfString& vAlignment)
{
    this->m_sVAlignment = vAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetJustification() const
{
    return this->m_sJustification;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetJustification(const MdfString& justification)
{
    this->m_sJustification = justification;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetLineSpacing() const
{
    return this->m_sLineSpacing;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetLineSpacing(const MdfString& lineSpacing)
{
    this->m_sLineSpacing = lineSpacing;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetTextColor() const
{
    return this->m_sTextColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetTextColor(const MdfString& textColor)
{
    this->m_sTextColor = textColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetGhostColor() const
{
    return this->m_sGhostColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetGhostColor(const MdfString& ghostColor)
{
    this->m_sGhostColor = ghostColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Text::GetMarkup() const
{
    return this->m_sMarkup;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::SetMarkup(const MdfString& markup)
{
    this->m_sMarkup = markup;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
TextFrame* Text::GetFrame()
{
    return this->m_pFrame;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::AdoptFrame(TextFrame* frame)
{
    if (this->m_pFrame != frame)
    {
        delete this->m_pFrame;
        this->m_pFrame = frame;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
TextFrame* Text::OrphanFrame()
{
    TextFrame* pRet = this->m_pFrame;
    this->m_pFrame = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Text::AcceptVisitor(IGraphicElementVisitor& igeVisitor)
{
    igeVisitor.VisitText(*this);
}
