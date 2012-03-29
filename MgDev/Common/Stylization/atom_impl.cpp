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
#include "atom.h"
#include <stdlib.h> // for atoi
/*
    ATOM_IMPL.CPP
    =============
    This file implements those concrete classes defined in atom.h

    Note that users will need to write concrete implementations that
    implement the interfaces defined in atom.h, or they can rely on
    those implementations presented in atom_element*.h.
*/

#define _CRT_ATOF_DEFINED // needed to get rid of a stupid MS error concerning unneeded _locale_t
#include "math.h"


#ifndef _WIN32// for Linux builds only.
double _wtof(const wchar_t* _Str)
    { return (double)wcstold(_Str, NULL);  /* radix is always 10 */ }
int _wtoi(const wchar_t *_Str)
    { return (int)wcstol(_Str, NULL, 10);  /* radix is always 10 */ }
#endif


namespace RichText { namespace ATOM
{

//
//  Base StyleParticle implementation.
//

void Particle::Append(const Particle* pEnd)
{
    if(this->m_pNext)
        this->m_pNext->Append(pEnd);
    else
        this->m_pNext = const_cast<Particle*>(pEnd);
}


ATOM::Status Particle::AddToList(Particle*& pList, const Particle& oParticle)
{

    Particle* pParticle = oParticle.Clone();
    if(!pParticle)
        return ATOM::Status::keNoMemory;

    // Start a new list, or add to the end.
    if(pList == NULL)
        pList = pParticle;
    else
        pList->Append(pParticle);

    return ATOM::Status::keOk;
}



// Dummy assignment; do nothing at this level.
ATOM::StyleParticle& ATOM::StyleParticle::operator= (const ATOM::StyleParticle&)
{
    return *this;
}

// Derivations implemented via macro.
ATOM_STYLE_PARTICLE_IMPL(Typeface,            StRange)
ATOM_STYLE_PARTICLE_IMPL(AltTypefaces,        StRange)           // Alternate typefaces, to be tried if Typeface isn't found.
ATOM_STYLE_PARTICLE_IMPL(PitchFamily,         PitchFamily::Type) // Font-matching heuristics if font isn't known.
ATOM_STYLE_PARTICLE_IMPL(CharacterSet,        CharacterSet::Type)// Font-matching heuristics for which character set.
ATOM_STYLE_PARTICLE_IMPL(Size,                Measure)
ATOM_STYLE_PARTICLE_IMPL(CapSize,             Measure)
ATOM_STYLE_PARTICLE_IMPL(FontWeight,          FontWeight::Type)
ATOM_STYLE_PARTICLE_IMPL(Italic,              bool)              // Italic variation of font?
ATOM_STYLE_PARTICLE_IMPL(FillColor,           Color)
ATOM_STYLE_PARTICLE_IMPL(StrokeColor,         Color)
ATOM_STYLE_PARTICLE_IMPL(StrokeWeight,        Measure)           // a line weight measure
ATOM_STYLE_PARTICLE_IMPL(StrokeBehind,        bool)              // for SFA vs SVG: does the stroke get rendered behind fill?
ATOM_STYLE_PARTICLE_IMPL(Underline,           TextLine::Type)    // None/Single/Double/Dotted.  Sink may support only
ATOM_STYLE_PARTICLE_IMPL(Overline,            TextLine::Type)    // ..                          a limited subset
ATOM_STYLE_PARTICLE_IMPL(Strikethrough,       TextLine::Type)    // ..                          and modify rendering.
ATOM_STYLE_PARTICLE_IMPL(CaseShift,           CaseShift::Type)   // Small Caps, etc.
ATOM_STYLE_PARTICLE_IMPL(TrackingAugment,     Measure)
ATOM_STYLE_PARTICLE_IMPL(Justification,       Justification::Type)
ATOM_STYLE_PARTICLE_IMPL(VerticalAlignment,   VerticalAlignment::Type)
ATOM_STYLE_PARTICLE_IMPL(HorizontalAlignment, HorizontalAlignment::Type)
ATOM_STYLE_PARTICLE_IMPL(AdvanceAlignment,    Measure)
ATOM_STYLE_PARTICLE_IMPL(ReferenceExpansion,  ReferenceExpansion::Type)// Reference Expansion

ATOM_STYLE_PARTICLE_IMPL(BackgroundColor,     Color)             // Color (with Alpha) of the background.
ATOM_STYLE_PARTICLE_TBLR_IMPL(InnerPadding,   Measure)           // Top/Bottom/Left/Right InnerPadding: space between border and text
ATOM_STYLE_PARTICLE_TBLR_IMPL(BorderLine,     BorderLine::Type)  // Top/Bottom/Left/Right BorderStyle: kind of border-line along edge
ATOM_STYLE_PARTICLE_TBLR_IMPL(BorderWidth,    Measure)           // ... BorderWidth: linewidth of the border
ATOM_STYLE_PARTICLE_TBLR_IMPL(BorderColor,    Color)             // ... BorderColor: color of the border
ATOM_STYLE_PARTICLE_TBLR_IMPL(OuterPadding,   Measure)           // ... OuterPadding: space between border and adjacent text.

//
//  Base TransformParticle implementation.
//

// Derivations (except for each SetMatrix method)
// implemented by macro
TRANSFORM_PARTICLE_XY_IMPL(Scale,NUMBER)
void ScaleTransformParticle::SetMatrix(ATOM::Matrix& m) const
{
    m[0][0] = this->m_x;
    m[1][1] = this->m_y;
}

bool ScaleTransformParticle::IsIdentity() const
{
    return this->m_x == 1 && this->m_y == 1;
}

TRANSFORM_PARTICLE_XY_IMPL(Skew,RadialMeasure)
void SkewTransformParticle::SetMatrix(ATOM::Matrix& m) const
{
    m[0][1] = tan(this->m_x.Radians());
    m[1][0] = tan(this->m_y.Radians());
}

bool SkewTransformParticle::IsIdentity() const
{
    return this->m_x.Radians() == 0 && this->m_y.Radians() == 0;
}

TRANSFORM_PARTICLE_XY_IMPL(Translation,NUMBER)
void TranslationTransformParticle::SetMatrix(ATOM::Matrix& m) const
{
    m[0][2] = this->m_x;
    m[1][2] = this->m_y;
}

bool TranslationTransformParticle::IsIdentity() const
{
    return this->m_x == 0 && this->m_y == 0;
}

TRANSFORM_PARTICLE_IMPL(Rotation,RadialMeasure)
void RotationTransformParticle::SetMatrix(ATOM::Matrix& m) const
{
    // Assumes this->m_v is in radians.
    NUMBER c = cos(this->m_v.Radians());
    NUMBER s = sin(this->m_v.Radians());

    m[0][0] = c;   m[0][1] = -s;
    m[1][0] = s;   m[1][1] =  c;
}

bool RotationTransformParticle::IsIdentity() const
{
    return this->m_v.Radians() == 0;
}

TRANSFORM_PARTICLE_IMPL(Arbitrary, Matrix)

void ArbitraryTransformParticle::SetMatrix(ATOM::Matrix& m) const
{
    m = this->m_v;
}

bool ArbitraryTransformParticle::IsIdentity() const
{
    NUMBER* pMat = (NUMBER*)this->m_v;
    return pMat[elem_00] == 1 && pMat[elem_01] == 0 && pMat[elem_02] == 0
        && pMat[elem_10] == 0 && pMat[elem_11] == 1 && pMat[elem_12] == 0
        && pMat[elem_20] == 0 && pMat[elem_21] == 0 && pMat[elem_22] == 1;
}





/*
LocationParticle::LocationParticle()
: m_pNext(NULL)
{}

const LocationParticle* LocationParticle::Next() const
{
    return m_pNext;
}

void LocationParticle::SetNext(LocationParticle* pNext)
{
    m_pNext = pNext;
}


void LocationParticle::Append(const LocationParticle* pEnd)
{
    if(this->m_pNext)
        this->m_pNext->Append(pEnd);
    else
        this->m_pNext = const_cast<LocationParticle*>(pEnd);
}
*/

/// ==================================================


ATOM_PARTICLE_IMPL(Location,Bookmark)
BookmarkLocationParticle::BookmarkLocationParticle(int iIndex)
: m_iIndex(iIndex)
{
}

LocationParticle*    BookmarkLocationParticle::Clone() const
{
    return new BookmarkLocationParticle(m_iIndex);
}

LocationParticle&    BookmarkLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
        this->m_iIndex = ((const BookmarkLocationParticle&)o).m_iIndex;
    }

    return *this;
}

int BookmarkLocationParticle::Index() const
{
    return m_iIndex;
}

bool                 BookmarkLocationParticle::operator==(const LocationParticle& o) const
{
    if(Type() != o.Type())
        return false;
    else
        return this->m_iIndex == ((BookmarkLocationParticle&)o).m_iIndex;
}



ATOM_PARTICLE_IMPL(Location,ReturnToBookmark)
ReturnToBookmarkLocationParticle::ReturnToBookmarkLocationParticle(int iIndex)
: m_iIndex(iIndex)
{
}

LocationParticle*    ReturnToBookmarkLocationParticle::Clone() const
{
    return new ReturnToBookmarkLocationParticle(m_iIndex);
}


LocationParticle&    ReturnToBookmarkLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
        this->m_iIndex = ((const ReturnToBookmarkLocationParticle&)o).m_iIndex;
    }

    return *this;
}

bool                 ReturnToBookmarkLocationParticle::operator==(const LocationParticle& o) const
{
    if(Type() != o.Type())
        return false;
    else
        return this->m_iIndex == ((ReturnToBookmarkLocationParticle&)o).m_iIndex;
}

int ReturnToBookmarkLocationParticle::Index() const
{
    return m_iIndex;
}


ATOM_PARTICLE_IMPL(Location,ConditionalReturnToBookmark)
ConditionalReturnToBookmarkLocationParticle::ConditionalReturnToBookmarkLocationParticle(int iIndex, ConditionType eType)
: m_iIndex(iIndex)
, m_eCondition(eType)
{
}

LocationParticle* ConditionalReturnToBookmarkLocationParticle::Clone() const
{
    return new ConditionalReturnToBookmarkLocationParticle(Index(),m_eCondition);
}

LocationParticle& ConditionalReturnToBookmarkLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
        this->m_iIndex     = ((const ConditionalReturnToBookmarkLocationParticle&)o).m_iIndex;
        this->m_eCondition = ((const ConditionalReturnToBookmarkLocationParticle&)o).m_eCondition;
    }
    return *this;
}

bool ConditionalReturnToBookmarkLocationParticle::operator==(const LocationParticle& o) const
{
    if(Type() != o.Type())
        return false;
    else
        return (this->m_iIndex     == ((ConditionalReturnToBookmarkLocationParticle&)o).m_iIndex
            &&  this->m_eCondition == ((ConditionalReturnToBookmarkLocationParticle&)o).m_eCondition);
}

int ConditionalReturnToBookmarkLocationParticle::Index() const
{
    return m_iIndex;
}

ATOM::ConditionalReturnToBookmarkLocationParticle::ConditionType ConditionalReturnToBookmarkLocationParticle::Condition() const
{
    return this->m_eCondition;
}

ATOM_PARTICLE_IMPL(Location,Relative)
RelativeLocationParticle::RelativeLocationParticle(ATOM::Measure mAdvance, ATOM::Measure mRise)
: m_mAdvance(mAdvance)
, m_mRise(mRise)
{
}

LocationParticle*    RelativeLocationParticle::Clone() const
{
    return new RelativeLocationParticle(m_mAdvance,m_mRise);
}

LocationParticle&    RelativeLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
          this->m_mAdvance = ((const RelativeLocationParticle&)o).m_mAdvance;
          this->m_mRise    = ((const RelativeLocationParticle&)o).m_mRise;
    }

    return *this;
}

bool                 RelativeLocationParticle::operator==(const LocationParticle& o) const
{
    if(Type() != o.Type())
        return false;
    else
        return this->m_mAdvance == ((RelativeLocationParticle&)o).m_mAdvance
            && this->m_mRise    == ((RelativeLocationParticle&)o).m_mRise;
}

ATOM::Measure RelativeLocationParticle::Advance() const
{
    return m_mAdvance;
}

ATOM::Measure RelativeLocationParticle::Rise() const
{
    return m_mRise;
}


ATOM_PARTICLE_IMPL(Location,Point)
PointLocationParticle::PointLocationParticle(ATOM::NUMBER x, ATOM::NUMBER y)
: m_x(x)
, m_y(y)
{
}

LocationParticle*    PointLocationParticle::Clone() const
{
    return new PointLocationParticle(m_x,m_y);
}

LocationParticle&    PointLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
          this->m_x = ((const PointLocationParticle&)o).m_x;
          this->m_x = ((const PointLocationParticle&)o).m_y;
    }

    return *this;
}

bool                 PointLocationParticle::operator==(const LocationParticle& o) const
{
    if(Type() != o.Type())
        return false;
    else
        return this->m_x == ((PointLocationParticle&)o).m_x
            && this->m_y == ((PointLocationParticle&)o).m_y;
}

ATOM::NUMBER PointLocationParticle::X() const
{
    return this->m_x;

}

ATOM::NUMBER PointLocationParticle::Y() const
{
    return this->m_y;
}

ATOM_PARTICLE_IMPL(Location,LineBreak)
LineBreakLocationParticle::LineBreakLocationParticle()
{
}

LocationParticle*    LineBreakLocationParticle::Clone() const
{
    return new LineBreakLocationParticle();
}

LocationParticle&    LineBreakLocationParticle::operator= (const LocationParticle& /*o*/)
{
//    if(Type() == o.Type()) {
//    }

    return *this;
}

bool                 LineBreakLocationParticle::operator==(const LocationParticle& o) const
{
    return (Type() == o.Type());
}


ATOM_PARTICLE_IMPL(Location,Path)
PathLocationParticle::PathLocationParticle(/* path description*/)
{
}

LocationParticle* PathLocationParticle::Clone() const
{
    return new PathLocationParticle(/*...*/);
}

LocationParticle& PathLocationParticle::operator= (const LocationParticle& o)
{
    if(Type() == o.Type()) {
        // ...
    }

    return *this;
}

bool PathLocationParticle::operator==(const LocationParticle& /*o*/) const
{
    return false; // TODO: not implemented yet...
}







void Matrix::operator*= (const NUMBER* o)
{
    NUMBER m[9];

    m[elem_00] = m_pElements[elem_00]*o[elem_00]
               + m_pElements[elem_01]*o[elem_10]
               + m_pElements[elem_02]*o[elem_20];
    m[elem_01] = m_pElements[elem_00]*o[elem_01]
               + m_pElements[elem_01]*o[elem_11]
               + m_pElements[elem_02]*o[elem_21];
    m[elem_02] = m_pElements[elem_00]*o[elem_02]
               + m_pElements[elem_01]*o[elem_12]
               + m_pElements[elem_02]*o[elem_22];

    m[elem_10] = m_pElements[elem_10]*o[elem_00]
               + m_pElements[elem_11]*o[elem_10]
               + m_pElements[elem_12]*o[elem_20];
    m[elem_11] = m_pElements[elem_10]*o[elem_01]
               + m_pElements[elem_11]*o[elem_11]
               + m_pElements[elem_12]*o[elem_21];
    m[elem_12] = m_pElements[elem_10]*o[elem_02]
               + m_pElements[elem_11]*o[elem_12]
               + m_pElements[elem_12]*o[elem_22];

    m[elem_20] = m_pElements[elem_20]*o[elem_00]
               + m_pElements[elem_21]*o[elem_10]
               + m_pElements[elem_22]*o[elem_20];
    m[elem_21] = m_pElements[elem_20]*o[elem_01]
               + m_pElements[elem_21]*o[elem_11]
               + m_pElements[elem_22]*o[elem_21];
    m[elem_22] = m_pElements[elem_20]*o[elem_02]
               + m_pElements[elem_21]*o[elem_12]
               + m_pElements[elem_22]*o[elem_22];

   *this = m;
}


void Matrix::SetZero()
{
    m_pElements[0] = 0, m_pElements[1] = 0, m_pElements[2] = 0,
    m_pElements[3] = 0, m_pElements[4] = 0, m_pElements[5] = 0,
    m_pElements[6] = 0, m_pElements[7] = 0, m_pElements[8] = 0;
}
void Matrix::SetIdentity()
{
    m_pElements[0] = 1, m_pElements[1] = 0, m_pElements[2] = 0,
    m_pElements[3] = 0, m_pElements[4] = 1, m_pElements[5] = 0,
    m_pElements[6] = 0, m_pElements[7] = 0, m_pElements[8] = 1;
}

}} // namespace RichText::ATOM
