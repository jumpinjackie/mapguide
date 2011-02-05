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

#ifndef RICHTEXTENGINE_H_
#define RICHTEXTENGINE_H_

#include "atom.h"
#include "atom_element.h"
#include "Stylization.h"
#include "RendererStyles.h"
#include "RS_Font.h"

using namespace RichText::ATOM;

struct LinePos;
class  SE_Renderer;
class  RS_FontEngine;
class  RS_TextMetrics;
class  AtomRun;
class  AtomLine;
class  AtomBlock;
class  RichTextFormatState;

class AtomBaseComponent
{
public:
    AtomBaseComponent( RS_F_Point position, AtomBaseComponent* pParent );
    virtual ~AtomBaseComponent();

    // Virtuals which must be implemented by parent class
    // Note that parent class must calculate the extent
    virtual double      GetAscentPosition( bool yUp ) = 0;
    virtual double      GetDescentPosition( bool yUp ) = 0;
    virtual double      GetCaplinePosition( bool yUp ) = 0;
    virtual double      GetBaselinePosition( bool yUp ) = 0;
    virtual void        OutputData( RS_F_Point parentPosition, RS_TextMetrics* pTextMetrics ) = 0;

    // Methods to access data
    virtual RS_F_Point& Offset();
    virtual void        GetPosition( RS_F_Point& position );
    virtual RS_F_Point* GetExtent();
    virtual double      GetWidth();
    virtual void        GetPositionedExtent( RS_F_Point* positionedExtent );
    virtual RS_F_Point  GetUpperLeftCorner();
    virtual RS_F_Point  GetLowerRightCorner();
    virtual void        AddExtent( RS_F_Point* ext, bool yUp );
    virtual void        Translate( RS_F_Point translation );

protected:
    RS_F_Point          m_offset;
    RS_F_Point          m_extent[4];
    AtomBaseComponent*  m_pParent;
};

class AtomBaseComponentCollection : public AtomBaseComponent
{
public:
    // Construction
    AtomBaseComponentCollection( RS_F_Point position, AtomBaseComponent* pParent );
    virtual ~AtomBaseComponentCollection();

    void AddComponent( AtomBaseComponent* pComponent );
    void CalculateExtent( bool yUp );

    // AtomBaseComponent
    virtual double  GetAscentPosition( bool yUp );
    virtual double  GetDescentPosition( bool yUp );
    virtual double  GetCaplinePosition( bool yUp );
    virtual double  GetBaselinePosition( bool yUp );
    virtual void    OutputData( RS_F_Point parentPosition, RS_TextMetrics* pTextMetrics );

protected:
    // Ordered list of components in collection
    std::vector<AtomBaseComponent*> m_components;
};


class AtomRun : public AtomBaseComponent
{
public:
    // Construction
    AtomRun( RS_F_Point position, AtomLine* pParentLine, RichTextFormatState &formatState );
    virtual ~AtomRun();

    void SetTextRun( StRange runContent, unsigned int runInd, Particle* pFormatChanges );
    void Close( RS_FontEngine* pFontEngine, const RS_Font* pFont );
    double  GetAdvanceAlignment();

    // AtomBaseComponent
    double  GetAscentPosition( bool yUp );
    double  GetDescentPosition( bool yUp );
    double  GetCaplinePosition( bool yUp );
    double  GetBaselinePosition( bool yUp );
    void    OutputData( RS_F_Point parentPosition, RS_TextMetrics* pTextMetrics );

private:
    void CalculateExtent( RS_FontEngine* pFontEngine, const RS_Font* pFont );

    // The text run
    wchar_t*            m_textRun;
    size_t              m_textRunLen;
    unsigned int        m_textRunInd;
    std::vector<float>  m_charAdvances;
    Particle*           m_pFormatChanges;

    // Formatting values needed to calculate metrics and line post-processing
    double              m_fontAscent;
    double              m_fontDescent;
    double              m_fontCapline;
    double              m_actualHeight;
    double              m_advanceAlignment;  // Used to track state of advance alignment multiplier
};

class AtomLine : public AtomBaseComponentCollection
{
public:
    // Construction
    AtomLine( RS_F_Point position, AtomBlock *pParentBlock, RichTextFormatState &formatState, bool fixedLine );
    virtual ~AtomLine();

    double      Close( bool yUp );
    AtomBlock*  GetParentBlock();

private:
    double  AdjustBaseline( bool yUp );
    void    ApplyAdvanceAlignment();

    double  m_initialAscent;
    bool    m_fixedLine;
};


class AtomBlock : public AtomBaseComponentCollection
{
public:
    // Construction
    AtomBlock( RS_F_Point position, AtomLine* pParentLine, RichTextFormatState& formatState );
    virtual ~AtomBlock();

    void        Close( bool yUp );
    AtomLine*   GetParentLine();
    AtomLine*   GetLastLine();

private:
    void        ApplyJustification();

    RS_Justify  m_justification;
};

class RichTextFormatState
{
public:
    RS_TextDef              m_tmpTDef;

    NUMBER                  m_advanceAlignmentVal;
    double                  m_fontAscent;
    double                  m_fontDescent;
    double                  m_fontCapline;
    double                  m_actualHeight;
    double                  m_lineHeight;

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

    // Parser ISink implementation
    STYLIZATION_API Status Initialize(IEnvironment*) { m_parserSinkState = ISink::keInitialized; return Status::keContinue; }
    STYLIZATION_API Status TextRun(ITextRun*,IEnvironment*);
    STYLIZATION_API Status Abandon(IAbandonment*,IEnvironment*);
    STYLIZATION_API Status Terminate(IEnvironment*) { return Status::keOk; }
    STYLIZATION_API SinkStateType SinkState() { return m_parserSinkState; }
    STYLIZATION_API IGenerator* GetGenerator() { return NULL; }
    STYLIZATION_API ICapability* GetMarkupCapabilities(/*...*/) { return NULL; }

private:
    double GetHorizontalAlignmentOffset(RS_HAlignment hAlign, RS_F_Point* extent);
    double GetVerticalAlignmentOffset(RS_VAlignment vAlign, double textTop, double topCapline, double textBottom, double bottomBaseline );
    void   ApplyLocation( const ILocation* pLocation );
    double ConvertToScreenUnits( double val, Measure::UnitType unit );
    void   GetFontValues();
    void   OpenNewLine( bool fixedLine );
    void   CloseCurrentLine();
    void   OpenNewBlock();
    void   CloseCurrentBlock();

public:
    SE_Renderer*    m_pSERenderer;
    RS_FontEngine*  m_pFontEngine;

private:
    ////////////////////////////////////Parser ISink variables///////////////////////////////////////////////////////////

    SinkStateType                   m_parserSinkState;

    // Current format state
    int                             m_stateDepth;           // Used to manage the format state stack
    RichTextFormatState             m_formatState;          // The format state stack
    NUMBER                          m_curXform[9];

    // Current position
    bool                            m_yUp;                  // Axis orientation
    RS_F_Point                      m_curPos;
    RS_F_Point                      m_bookmarks[ kiBookmarkTableSize ];  // Position bookmarks

    // Block hierarchy
    AtomBlock*                      m_pHeadBlock;           // Outermost block
    AtomBlock*                      m_pCurrBlock;           // Current block
    AtomLine*                       m_pCurrLine;            // Current line
    unsigned int                    m_numRuns;

    ////////////////////////////////////Parser ISink variables///////////////////////////////////////////////////////////
};

#endif
