//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef RICHTEXTENGINE_H_
#define RICHTEXTENGINE_H_

#include "atom.h"
#include "atom_element.h"
#include "Stylization.h"
#include "RendererStyles.h"

using namespace RichText::ATOM;

struct LinePos;
class  SE_Renderer;
class  RS_FontEngine;
class  RS_TextMetrics;


struct AtomBookmark
{
    double xPos;
    double yPos;
};


class RichTextFormatState
{
public:
    RS_TextDef              m_tmpTDef;
    NUMBER                  m_trackingVal;          // Used to track state of tracking multiplier
    NUMBER                  m_advanceAlignmentVal;  // Used to track state of advance alignment multiplier

    RichTextFormatState*    m_pNext;   // Used to maintain a stack of states
};


class FormatStatePushPopParticle : public Particle
{
public:
    void SetPush( bool push )
    {
        this->m_push = push;
    }

    const bool Push() const
    {
        return this->m_push;
    }

    FormatStatePushPopParticle* Clone() const
    {
        FormatStatePushPopParticle* pClone = new FormatStatePushPopParticle();
        pClone->SetPush( this->m_push );
        pClone->SetNext( NULL );
        return pClone;
    }
private:
    bool m_push;
};


class RichTextEngine : public ISink
{
public:
    STYLIZATION_API RichTextEngine( SE_Renderer* pSERenderer, RS_FontEngine* pFontEngine, RS_TextDef* pTDef );
    STYLIZATION_API ~RichTextEngine();

    STYLIZATION_API bool Parse( const RS_String& s, RS_TextMetrics* pTextMetrics );
    STYLIZATION_API void ApplyFormatChanges( const Particle* pFormatChanges );
    STYLIZATION_API void InitEngine( RS_TextDef* pTDef );
    STYLIZATION_API void GetTextDef( RS_TextDef* pTDef );
    STYLIZATION_API void GetTransform( NUMBER xform[9] );
    STYLIZATION_API void GetRichTextFormatState( RichTextFormatState* pState );

public:
    // Parser ISink implementation
    STYLIZATION_API Status Initialize(IEnvironment*) { m_parserSinkState = ISink::keInitialized; return Status::keContinue; }
    STYLIZATION_API Status TextRun(ITextRun*,IEnvironment*);
    STYLIZATION_API Status Abandon(IAbandonment*,IEnvironment*);
    STYLIZATION_API Status Terminate(IEnvironment*) { return Status::keOk; }
    STYLIZATION_API SinkStateType SinkState() { return m_parserSinkState; }
    STYLIZATION_API IGenerator* GetGenerator() { return NULL; }
    STYLIZATION_API ICapability* GetMarkupCapabilities(/*...*/) { return NULL; }

public:

private:
    double GetHorizontalAlignmentOffset(RS_HAlignment hAlign, RS_F_Point* extent);
    double GetVerticalAlignmentOffset(RS_VAlignment vAlign, double textTop, double topCapline, double textBottom, double bottomBaseline );

    void ApplyLocationOperations( const ILocation* pLocation );
    double ConvertToPixels( double val, Measure::UnitType unit );
    void InitLine( bool fixed );
    void TrackLineMetrics();
    void GetFontValues();

public:
    SE_Renderer*    m_pSERenderer;
    RS_FontEngine*  m_pFontEngine;

private:
    ////////////////////////////////////Parser ISink variables///////////////////////////////////////////////////////////

    SinkStateType                   m_parserSinkState;

    // Current format values
    int                             m_stateDepth;           // Used to manage the format state stack
    RichTextFormatState             m_formatState;          // The format state stack

    // Current position
    bool                            m_yUp;                  // Axis orientation
    double                          m_curX, m_curY;
    NUMBER                          m_curXform[9];

    // Text runs
    unsigned int                    m_numRuns;
    std::vector<LinePos>            m_line_pos;             // Their positions and extents
    std::vector<RS_String>          m_line_breaks;          // Text runs
    std::vector<const Particle*>    m_format_changes;       // Formatting for each run

    // Line Metrics
    bool                            m_fixedLine;            // Needed to manage baseline
    double                          m_lineMinDescentPos;    // Needed to manage newlines
    double                          m_lineMaxAscentPos;     // Needed to manage baseline
    double                          m_topCapline;           // Needed for vertical alignment
    double                          m_bottomBaseline;       // Needed for vertical alignment
    unsigned int                    m_numLines;
    std::vector<unsigned int>       m_lineStarts;           // Needed for horizontal alignment

    // Current font values
    double                          m_fontAscent;
    double                          m_fontDescent;
    double                          m_fontCapline;
    double                          m_actualHeight;
    double                          m_lineHeight;

    // Position bookmarks
    AtomBookmark                    m_bookmarks[ kiBookmarkTableSize ];

    ////////////////////////////////////Parser ISink variables///////////////////////////////////////////////////////////
};

#endif
