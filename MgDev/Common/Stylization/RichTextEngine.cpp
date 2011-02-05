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
#include "RichTextEngine.h"
#include "SE_Renderer.h"
#include "RS_FontEngine.h"
#include <math.h>
#include <assert.h>

using namespace RichText::ATOM;

//////////////////////////////////////////////////////////////////////////////
RichTextEngine::RichTextEngine( SE_Renderer* pSERenderer, RS_FontEngine* pFontEngine, RS_TextDef* pTDef )
{
    m_pSERenderer = pSERenderer;
    m_pFontEngine = pFontEngine;
    this->InitEngine( pTDef );
}


//////////////////////////////////////////////////////////////////////////////
RichTextEngine::~RichTextEngine()
{
    while ( this->m_formatState.m_pNext != NULL )
    {
        RichTextFormatState* pNext = this->m_formatState.m_pNext;
        this->m_formatState.m_pNext = pNext->m_pNext;
        delete pNext;
    }

    delete this->m_pHeadBlock;
    delete this->m_pCurrLine;
}


void RichTextEngine::InitEngine( RS_TextDef* pTDef )
{
    // Check for renderer && font engine
    if ( !m_pSERenderer || !m_pFontEngine || !pTDef )
        return;

    // Initialize format state
    this->m_stateDepth = 0;
    this->m_formatState.m_tmpTDef = *pTDef;
    this->m_formatState.m_advanceAlignmentVal = 0.0;
    this->m_formatState.m_pNext = NULL;

    // Initialize position vals
    this->m_yUp = this->m_pSERenderer->YPointsUp();
    this->m_curPos.x = 0.0;
    this->m_curPos.y = 0.0;

    // Initialize font vals
    GetFontValues();

    // Create initial block
    this->m_pHeadBlock  = new AtomBlock( this->m_curPos, NULL, this->m_formatState );
    this->m_pCurrBlock = this->m_pHeadBlock;
    this->OpenNewLine( true );
    this->m_numRuns = 0;

    // Initialize bookmark table
    for ( int i=0; i<kiBookmarkTableSize; ++i )
    {
        this->m_bookmarks[i].x = 0.0;
        this->m_bookmarks[i].y = 0.0;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Methods used to get current state variables.
//
void RichTextEngine::GetTextDef( RS_TextDef* pTDef )
{
    *pTDef = this->m_formatState.m_tmpTDef;
}

void RichTextEngine::GetTransform( NUMBER xform[9] )
{
    for ( int i=0; i<9; ++i )
        xform[i] = this->m_curXform[i];
}

//////////////////////////////////////////////////////////////////////////////
// Support for Rich Text formatting
// This is generic rich text which uses any specific markup language such as
// MText, RTF, SVG, etc.  Note that MText is the only supported markup
// language for Rev 1.
bool RichTextEngine::Parse( const RS_String& s, RS_TextMetrics* pTextMetrics )
{
    bool parserSucceeded = false;

    try
    {
        // Check for renderer && font engine
        if ( !m_pSERenderer || !m_pFontEngine )
            return false;

        // Store Initial format state
        RichTextFormatState initialState = this->m_formatState;

        // Get parser
        IUniverse* pUniverse = BigBang();
        IGenerator* pGenerator = pUniverse->GetGenerator( this->m_formatState.m_tmpTDef.markup().c_str() );
        if(!pGenerator)
            return false;
        IParser* pMText;
        if(!pGenerator->Create(&pMText).Succeeded())
            return false;
        this->m_parserSinkState = ISink::keWaiting;

        //MyReferenceResolver refres;
        EmptyStyleTable est;
        EnvironmentElement env(this,&est,Color(0,0,0));
        //env.SetResolver(&refres);

        // Set up the ambient style description.  This is the "default" style stuff.
        RS_FontDef& fontDef = this->m_formatState.m_tmpTDef.font();
        env.UpdateAmbientStyle( TypefaceStyleParticle(  fontDef.name().c_str() ) );
        RS_Color& clr = this->m_formatState.m_tmpTDef.textcolor();
        env.UpdateAmbientStyle( StrokeColorStyleParticle( Color( clr.red(), clr.green(), clr.blue(), clr.alpha() ) ) );
        if ( ( fontDef.style() & RS_FontStyle_Bold ) == 0 )
            env.UpdateAmbientStyle(FontWeightStyleParticle(FontWeight::keNormal));
        else
            env.UpdateAmbientStyle(FontWeightStyleParticle(FontWeight::keBold));
        if ( ( fontDef.style() & RS_FontStyle_Italic ) == 0 )
            env.UpdateAmbientStyle( ItalicStyleParticle(false) );
        else
            env.UpdateAmbientStyle( ItalicStyleParticle(true) );
        if ( ( fontDef.style() & RS_FontStyle_Underline ) == 0 )
            env.UpdateAmbientStyle( UnderlineStyleParticle( TextLine::keNone ) );
        else
            env.UpdateAmbientStyle( UnderlineStyleParticle( TextLine::keSingle ) );
        if ( ( fontDef.style() & RS_FontStyle_Overline ) == 0 )
            env.UpdateAmbientStyle( OverlineStyleParticle( TextLine::keNone ) );
        else
            env.UpdateAmbientStyle( OverlineStyleParticle( TextLine::keSingle ) );
        env.UpdateAmbientStyle( StrikethroughStyleParticle( TextLine::keNone ) );
        env.UpdateAmbientStyle( SizeStyleParticle( Measure( 12, Measure::kePoints,NULL ) ) );
        VerticalAlignment::Type atomVAlign = VerticalAlignment::keMid;
        switch ( this->m_formatState.m_tmpTDef.valign() )
        {
            case RS_VAlignment_Cap:
                // Cap is not specifically supported - approximate with Ascender.
            case RS_VAlignment_Ascent:
                atomVAlign = VerticalAlignment::keAscender;
                break;
            case RS_VAlignment_Base:
                atomVAlign = VerticalAlignment::keBaseline ;
                break;
            case RS_VAlignment_Descent:
                atomVAlign = VerticalAlignment::keDescender;
                break;
            case RS_VAlignment_Half:
            default:
                atomVAlign = VerticalAlignment::keMid;
                break;
        }
        env.UpdateAmbientStyle( VerticalAlignmentStyleParticle( atomVAlign ) );
        HorizontalAlignment::Type atomHAlign = HorizontalAlignment::keMiddle;
        switch (this->m_formatState.m_tmpTDef.halign() )
        {
            case RS_HAlignment_Left:
                atomHAlign = HorizontalAlignment::keLeft;
                break;
            case RS_HAlignment_Right:
                atomHAlign = HorizontalAlignment::keRight;
                break;
            case RS_HAlignment_Center:
            default:
                atomHAlign = HorizontalAlignment::keMiddle;
                break;
        }
        env.UpdateAmbientStyle( HorizontalAlignmentStyleParticle( atomHAlign ) );
        Justification::Type atomJustification = Justification::keLeft;
        switch (this->m_formatState.m_tmpTDef.justify() )
        {
            case RS_Justify_Justify:
                atomJustification = Justification::keJustified;
                break;
            case RS_Justify_Right:
                atomJustification = Justification::keRight;
                break;
            case RS_Justify_Center:
                atomJustification = Justification::keCentered;
                break;
            case RS_Justify_Left:
            default:
                atomJustification = Justification::keLeft;
                break;
        }
        env.UpdateAmbientStyle( JustificationStyleParticle( atomJustification ) );
        // TODO: Track spacing

        parserSucceeded = pMText->Parse(s.c_str(),&env).Succeeded();
        pGenerator->Destroy(pMText);

        // Close any open blocks
        while ( this->m_pCurrBlock )
            this->CloseCurrentBlock();

        if ( parserSucceeded && this->m_numRuns > 0 && this->m_pHeadBlock )
        {
            // Get adjustments for horizontal and vertical alignments
            // Note that we pick up alignment values from the tmpTextDef which was updated with calls to ApplyFormatChanges.
            // This way we use the final alignment settings which will reflect any local changes in the
            // formatted string.
            RS_F_Point positionedExtent[4];
            RS_F_Point alignmentAdjustment;
            this->m_pHeadBlock->GetPositionedExtent( positionedExtent );
            alignmentAdjustment.x = this->GetHorizontalAlignmentOffset(
                this->m_formatState.m_tmpTDef.halign(),
                positionedExtent );
            alignmentAdjustment.y = this->GetVerticalAlignmentOffset(
                this->m_formatState.m_tmpTDef.valign(),
                this->m_pHeadBlock->GetUpperLeftCorner().y,
                this->m_pHeadBlock->GetCaplinePosition( this->m_yUp ),
                this->m_pHeadBlock->GetLowerRightCorner().y,
                this->m_pHeadBlock->GetBaselinePosition( this->m_yUp ) );

            // Fill out the RS_TextMetrics class
            pTextMetrics->line_pos.reserve( this->m_numRuns );
            pTextMetrics->line_pos.resize( this->m_numRuns );
            pTextMetrics->line_breaks.reserve( this->m_numRuns );
            pTextMetrics->line_breaks.resize( this->m_numRuns );
            pTextMetrics->format_changes.reserve( this->m_numRuns );
            pTextMetrics->format_changes.resize( this->m_numRuns );
            this->m_pHeadBlock->OutputData( alignmentAdjustment, pTextMetrics );
        }
        else
        if ( parserSucceeded && this->m_numRuns == 0 )
        {
            pTextMetrics->text = L"";
        }
    }
    catch ( ... )
    {
        parserSucceeded = false;
    }

    return parserSucceeded;
}

Status RichTextEngine::Abandon(IAbandonment* pAbandon,IEnvironment*)
{
    Status retStatus;

    switch ( pAbandon->Reason().Result() )
    {
    case Status::keInvalidArg:
    case Status::keNotImplemented:
    case Status::keNotSupported:
        m_parserSinkState = ISink::keWaiting;
        retStatus = Status::keContinue;
        break;
    case Status::keNoMemory:
    case Status::keNoResource:
        m_parserSinkState = ISink::keWaiting;
        retStatus = Status::keDone;
        break;
    default:
        m_parserSinkState = ISink::keAbandoned;
        retStatus = Status::keAbandoned;
        break;
    }

    return retStatus;
}

void RichTextEngine::GetFontValues()
{
    RS_FontDef& fontDef = this->m_formatState.m_tmpTDef.font();
    RS_Font* pFont = (RS_Font*) this->m_pFontEngine->GetRenderingFont( this->m_formatState.m_tmpTDef );
    this->m_formatState.m_actualHeight = this->m_pFontEngine->MetersToScreenUnits( fontDef.units(), fontDef.height());
    double scale = this->m_formatState.m_actualHeight / pFont->m_units_per_EM;
    this->m_formatState.m_fontAscent = fabs( pFont->m_ascender * scale );
    this->m_formatState.m_fontDescent = fabs( pFont->m_descender * scale );
    double fontHeight = fabs( pFont->m_height * scale );
    this->m_formatState.m_lineHeight = this->m_formatState.m_tmpTDef.linespace() * fontHeight;
    this->m_formatState.m_fontCapline = fabs( pFont->m_capheight * scale );
}

double RichTextEngine::ConvertToScreenUnits( double val, Measure::UnitType unit )
{
    double ret;
    switch ( unit )
    {
    case Measure::keUnitless: // Unitless; just a number
        ret = 0.0;  // Value has no meaning in our context
        break;
    case Measure::keModel:    // Units of the surrounding model space
        ret = this->m_pFontEngine->MetersToScreenUnits( RS_Units_Model, val );
        break;
    case Measure::keDevice:   // Physical device units
        ret = val;
        break;
    case Measure::kePoints:   // 1/72 of an inch; Twips = Points * 20, Pica = 12 Points
        val = (val / 72.0) * 0.0254;
        ret = this->m_pFontEngine->MetersToScreenUnits( RS_Units_Device, val );
        break;
    case Measure::keEm:       // an "em" (1.0 times height of current font.)
        {
            // This is calculated as ( val * pFont->m_units_per_EM ) * ( this->m_actualHeight / pFont->m_units_per_EM )
            // which reduces to the following:
            ret = val * this->m_formatState.m_actualHeight;
        }
        break;
    case Measure::keEx:        // The x-height (height of lower-case letter)
        // Not supported by RS_Font
    case Measure::keProportion:// Scale, 1 = normal (100%), 2 = twice normal, etc.
        // Should be handled differently
    default:
        ret = 0.0;  // Value has no meaning in our context
        break;
    }

    return ret;
}

void RichTextEngine::ApplyLocation( const ILocation* pLocation )
{
    if ( !pLocation )
        return;

    // Examine semantics
    ILocation::SemanticType semantics = pLocation->Semantics();
    bool openingNewBlock = (semantics & ILocation::keInlineBlock) != 0;
    bool openingNewLine = ((semantics & ILocation::keRow) | (semantics & ILocation::keLine)) != 0;
    if ( (semantics & ILocation::keEndInlineBlock) != 0 )
        this->CloseCurrentBlock();

    // Update the block's current position
    const LocationParticle* pOperations = pLocation->Operations();
    while ( pOperations )
    {
        LocationParticle::LocationParticleType opType = pOperations->Type();

        switch ( opType )
        {
        case LocationParticle::keLineBreak:
            {
                this->CloseCurrentLine();

                double leading = this->m_formatState.m_lineHeight - (this->m_formatState.m_fontAscent + this->m_formatState.m_fontDescent);
                AtomLine* pPreviousLine = this->m_pCurrBlock->GetLastLine();
                double previousLineMinDescentPos;
                if ( pPreviousLine )
                    previousLineMinDescentPos = pPreviousLine->GetDescentPosition( this->m_yUp );
                else
                {
                    if ( this->m_yUp )
                        previousLineMinDescentPos = this->m_curPos.y - this->m_formatState.m_fontDescent;
                    else
                        previousLineMinDescentPos = this->m_curPos.y + this->m_formatState.m_fontDescent;
                }
                this->m_curPos.x = 0.0;
                if ( this->m_yUp )
                    this->m_curPos.y = previousLineMinDescentPos - leading - this->m_formatState.m_fontAscent;
                else
                    this->m_curPos.y = previousLineMinDescentPos + leading + this->m_formatState.m_fontAscent;

                this->OpenNewLine( false );
            }
            break;
        case LocationParticle::keBookmark:
            {
                // Bookmark our location
                const BookmarkLocationParticle* pBookmarkParticle = static_cast<const BookmarkLocationParticle*>(pOperations);
                int index = pBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                    this->m_bookmarks[ index ] = this->m_curPos;
            }
            break;
        case LocationParticle::keReturnToBookmark:
            {
                // Go to bookmark
                const ReturnToBookmarkLocationParticle* pReturnToBookmarkParticle = static_cast<const ReturnToBookmarkLocationParticle*>(pOperations);
                int index = pReturnToBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                    this->m_curPos = this->m_bookmarks[ index ];
            }
            break;
        case LocationParticle::keConditionalReturnToBookmark:
            {
                const ConditionalReturnToBookmarkLocationParticle* pCondReturnToBookmarkParticle = static_cast<const ConditionalReturnToBookmarkLocationParticle*>(pOperations);
                int index = pCondReturnToBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                {
                    if ( pCondReturnToBookmarkParticle->Condition() == ConditionalReturnToBookmarkLocationParticle::keFarthestAdvance )
                        this->m_curPos.x = this->m_bookmarks[ index ].x > this->m_curPos.x ? this->m_bookmarks[ index ].x : this->m_curPos.x;
                    else // ConditionalReturnToBookmarkLocationParticle::keLeastAdvance
                        this->m_curPos.x = this->m_bookmarks[ index ].x < this->m_curPos.x ? this->m_bookmarks[ index ].x : this->m_curPos.x;
                    this->m_curPos.y = this->m_bookmarks[ index ].y;
                }
            }
            break;
        case LocationParticle::keRelative:
            {
                // Get advance and rise
                const RelativeLocationParticle* pRelParticle = static_cast<const RelativeLocationParticle*>(pOperations);
                Measure advanceMeasure = pRelParticle->Advance();
                Measure riseMeasure = pRelParticle->Rise();

                // Adjust current position
                this->m_curPos.x =
                    advanceMeasure.Units() == Measure::keProportion ?
                    this->m_curPos.x *= advanceMeasure.Number() :
                    this->m_curPos.x += ConvertToScreenUnits( advanceMeasure.Number(), advanceMeasure.Units() );
                if ( riseMeasure.Units() == Measure::keProportion )
                    this->m_curPos.y *= riseMeasure.Number();
                else
                if ( this->m_yUp )
                    this->m_curPos.y += ConvertToScreenUnits( riseMeasure.Number(), riseMeasure.Units() );
                else
                    this->m_curPos.y -= ConvertToScreenUnits( riseMeasure.Number(), riseMeasure.Units() );
            }
            break;
        case LocationParticle::kePoint:
            {
                const PointLocationParticle* pPointParticle = static_cast<const PointLocationParticle*>(pOperations);
                this->m_curPos.x = (double) pPointParticle->X();
                this->m_curPos.y = (double) pPointParticle->Y();
            }
            break;
        case LocationParticle::kePath:
            {
                // Not sure what this means
            }
            break;
        default:
            break;
        }

        pOperations = pOperations->Next();

    };

    // Finish semantics
    if ( openingNewBlock )
        this->OpenNewBlock();
    else
    if ( openingNewLine )
    {
        this->CloseCurrentLine();
        this->OpenNewLine( true );
    }
}

void RichTextEngine::OpenNewLine( bool fixedLine )
{
    this->m_pCurrLine = new AtomLine( this->m_curPos, this->m_pCurrBlock, this->m_formatState, fixedLine );
}

void RichTextEngine::CloseCurrentLine()
{
    if ( !this->m_pCurrLine )
        return;

    if ( this->m_pCurrLine->GetParentBlock() != this->m_pCurrBlock )
        assert( false ); // This shouldn't happen

    // Close the current line and account for any baseline adjustment
    this->m_curPos.y += this->m_pCurrLine->Close( this->m_yUp );

    // Add current line to the current block
    this->m_pCurrBlock->AddComponent( this->m_pCurrLine );
    this->m_pCurrLine = NULL;
}

void RichTextEngine::OpenNewBlock()
{
    if ( !this->m_pCurrLine )
        return;

    // Create a new block and its first line
    this->m_pCurrBlock = new AtomBlock( this->m_curPos, this->m_pCurrLine, this->m_formatState );
    this->OpenNewLine( true );
}

void RichTextEngine::CloseCurrentBlock()
{
    if ( !this->m_pCurrBlock )
        return;

    this->CloseCurrentLine();

    // Close the block and add to its parent line, if we have one.
    this->m_pCurrBlock->Close( this->m_yUp );
    this->m_pCurrLine = this->m_pCurrBlock->GetParentLine();
    if ( this->m_pCurrLine )
    {
        this->m_pCurrLine->AddComponent( m_pCurrBlock );
        this->m_pCurrBlock = this->m_pCurrLine->GetParentBlock();
    }
    else
        this->m_pCurrBlock = NULL;
}

Status RichTextEngine::TextRun(ITextRun* pTextRun,IEnvironment*)
{
    // Compile list of format changes
    Particle* pFormatChanges = NULL;
    int runDepth = pTextRun->Structure()->Depth();
    if ( runDepth != this->m_stateDepth )
    {
        FormatStatePushPopParticle* pPushPopParticle = new FormatStatePushPopParticle();
        pPushPopParticle->SetNext( NULL );
        if ( runDepth > this->m_stateDepth )
            pPushPopParticle->SetPush( true );
        else
            pPushPopParticle->SetPush( false );  // This is a pop

        pFormatChanges = (Particle*) pPushPopParticle;
        this->m_stateDepth = runDepth;
    }
    const StyleParticle* pStyleChanges = pTextRun->Style()->Deltas();
    while ( pStyleChanges )
    {
        Particle::AddToList( pFormatChanges, *pStyleChanges );
        pStyleChanges = pStyleChanges->Next();
    }
    const TransformParticle* pTransforms = pTextRun->Transform()->Description();
    while ( pTransforms )
    {
        if ( !pTransforms->IsIdentity() )
            Particle::AddToList( pFormatChanges, *pTransforms );
        pTransforms = pTransforms->Next();
    }

    // Apply format changes to current state
    this->ApplyFormatChanges( pFormatChanges );

    // Apply location changes to current position
    this->ApplyLocation( pTextRun->Location() );

    // Make sure we have a current line
    if ( !this->m_pCurrLine )
    {
        // This should not happent
        assert( false );
        delete pFormatChanges;
        return Status::keContinue;
    }

    // Add text run to current line
    if ( this->m_formatState.m_tmpTDef.trackSpacing() == 1.0 )
    {
        AtomRun* pRun = new AtomRun( this->m_curPos, this->m_pCurrLine, this->m_formatState );
        pRun->SetTextRun( pTextRun->Contents(), this->m_numRuns, pFormatChanges );
        const RS_Font* pFont = this->m_pFontEngine->GetRenderingFont( this->m_formatState.m_tmpTDef );
        pRun->Close( this->m_pFontEngine, pFont );
        this->m_pCurrLine->AddComponent( pRun );
        this->m_numRuns++;
        this->m_curPos.x += pRun->GetWidth();
    }
    else
    {
        // Create a text run for each char.
        const RS_Font* pFont = this->m_pFontEngine->GetRenderingFont( this->m_formatState.m_tmpTDef );
        double trackingVal = this->m_formatState.m_tmpTDef.trackSpacing();
        StRange entireContents = pTextRun->Contents();
        StRange charContent;
        wchar_t charCopy[2];
        charCopy[1] = L'\0';
        charContent.Set( &charCopy[0], 1 );
        for ( int i=0; i<entireContents.Length(); ++i )
        {
            // The first char already has its list of style particles and transform particles. However, since
            // Transform particles do not persist from one run to the next, we need to repeat the transform
            // particles for each char.  Note that each char needs its own copy so that, when RS_TextMetrics
            // cleans itself up, it doesn't delete the same list several times.
            if ( i > 0 )
            {
                const TransformParticle* pTransforms = pTextRun->Transform()->Description();
                TransformParticle* pCharTransforms = NULL;
                TransformParticle* pNextCharTransform = NULL;
                while ( pTransforms )
                {
                    if ( pNextCharTransform == NULL )
                    {
                        pCharTransforms = pTransforms->Clone();
                        pNextCharTransform = pCharTransforms;
                    }
                    else
                    {
                        pNextCharTransform->Append( pTransforms->Clone() );
                        pNextCharTransform = ( TransformParticle* ) pNextCharTransform->Next();
                    }
                    pTransforms = pTransforms->Next();
                }
                pFormatChanges = pCharTransforms;
            }

            // Create the run
            charCopy[0] = entireContents[i];
            AtomRun* pRun = new AtomRun( this->m_curPos, this->m_pCurrLine, this->m_formatState );
            pRun->SetTextRun( charContent, this->m_numRuns, pFormatChanges );
            pRun->Close( this->m_pFontEngine, pFont );
            this->m_pCurrLine->AddComponent( pRun );
            this->m_numRuns++;

            // Apply tracking value
            double charWidth = pRun->GetWidth();
            if ( trackingVal > 1.0 )
            {
                // If tracking is greater than 1, we need to stretch the extent so that underline/overline will
                // draw for the full extent of the char.  If less that 1, the overline/underline will overwrite
                // a bit but we want the last char to have a complete overline/underline.
                RS_F_Point* ext = pRun->GetExtent();
                ext[1].x *= trackingVal;
                ext[2].x = ext[1].x;
            }

            // Advance to the end of this char while allowing for tracking value.
            this->m_curPos.x += ( charWidth * trackingVal );
        }
    }

    return Status::keContinue;
}


//////////////////////////////////////////////////////////////////////////////
// Computes the Y offset that must be applied to the unrotated text to
// obtain the specified vertical alignment.  Positive Y points down.
double RichTextEngine::GetVerticalAlignmentOffset(RS_VAlignment vAlign, double textTop, double topCapline, double textBottom, double bottomBaseline )
{
    double offsetY = 0.0;

    switch (vAlign)
    {
    case RS_VAlignment_Descent:
        offsetY = -textBottom;
        break;

    case RS_VAlignment_Base:
        offsetY = -bottomBaseline;
        break;

    case RS_VAlignment_Half:
        offsetY = -0.5 * ( topCapline + bottomBaseline );
        break;

    case RS_VAlignment_Cap:
        offsetY = -topCapline;
        break;

    case RS_VAlignment_Ascent:
        offsetY = -textTop;
        break;
    }

    return offsetY;
}


//////////////////////////////////////////////////////////////////////////////
// Computes the X offset that must be applied to the unrotated text to
// obtain the specified horizontal alignment.
double RichTextEngine::GetHorizontalAlignmentOffset(RS_HAlignment hAlign, RS_F_Point* extent)
{
    double offsetX = 0.0;

    switch (hAlign)
    {
    case RS_HAlignment_Left:
        offsetX = -extent[0].x;
        break;

    case RS_HAlignment_Center:
        offsetX = -0.5*(extent[0].x + extent[1].x);
        break;

    case RS_HAlignment_Right:
        offsetX = -extent[1].x;
        break;
    }

    return offsetX;
}

void RichTextEngine::ApplyFormatChanges( const Particle* pFormatChanges )
{
    if ( !this->m_pFontEngine || !this->m_pSERenderer )
        return;

    RS_FontDef& fontDef = this->m_formatState.m_tmpTDef.font();

    const Particle* pParticle = pFormatChanges;
    const StyleParticle* pStyleParticle;
    const TransformParticle* pTransformParticle;
    const FormatStatePushPopParticle* pPushPopParticle;
    Matrix curXformMatrix( this->m_curXform  );
    curXformMatrix.SetIdentity();
    while ( pParticle )
    {
        pStyleParticle = dynamic_cast<const StyleParticle*>(pParticle);
        pTransformParticle = dynamic_cast<const TransformParticle*>(pParticle);
        pPushPopParticle = dynamic_cast<const FormatStatePushPopParticle*>(pParticle);

        if ( pStyleParticle ) switch ( pStyleParticle->Type() )
        {
            case StyleParticle::keFillColor:
                // Text color
                {
                    const StrokeColorStyleParticle* pStrokeColorParticle = static_cast<const StrokeColorStyleParticle*>( pStyleParticle );
                    const Color val = pStrokeColorParticle->Value();
                    this->m_formatState.m_tmpTDef.textcolor() = RS_Color( val.R(), val.G(), val.B(), val.A() );
                }
                break;


            case StyleParticle::keTypeface:
                // Typeface used, eg "Times New Roman"
                {
                    const TypefaceStyleParticle* pTypefaceParticle = static_cast<const TypefaceStyleParticle*>( pStyleParticle );
                    StRange typeface = pTypefaceParticle->Value();
                    fontDef.name() = RS_String( typeface.Start(), typeface.Length() );
                }
                break;

            case StyleParticle::keCharacterSet:
                // Character Set
                {
                    // We know that the Atom CharacterSet::Type enum is defined the same as RS_CharacterSetType,
                    // so we can simply typecast the value.
                    const CharacterSetStyleParticle* pCharacterSetParticle = static_cast<const CharacterSetStyleParticle*>( pStyleParticle );
                    fontDef.charset() = (RS_CharacterSetType) pCharacterSetParticle->Value();
                }
                break;

            case StyleParticle::keCapSize:
                // Text Height
                {
                    const CapSizeStyleParticle* pCapSizeParticle = static_cast<const CapSizeStyleParticle*>( pStyleParticle );
                    Measure capSize = pCapSizeParticle->Value();
                    switch ( capSize.Units() )
                    {
                    case Measure::keProportion:
                        fontDef.height() *= capSize.Number();
                        break;
                    case Measure::keModel:
                        {
                            double currHeight = this->m_formatState.m_actualHeight * m_pSERenderer->GetDrawingScale();
                            fontDef.height() *= ( capSize.Number() / currHeight );
                        }
                        break;
                    default:
                        // Not supported
                        break;
                    }
                }
                break;

            case StyleParticle::keItalic:
                {
                    const ItalicStyleParticle* pItalicParticle = static_cast<const ItalicStyleParticle*>( pStyleParticle );
                    int& style = (int&) fontDef.style();
                    if ( pItalicParticle->Value() )
                        // Turn italic on
                        style |= RS_FontStyle_Italic;
                    else
                        // Turn italic off
                        style &= ~RS_FontStyle_Italic;
                }
                break;

            case StyleParticle::keUnderline:
                {
                    const UnderlineStyleParticle* pUnderlineParticle = static_cast<const UnderlineStyleParticle*>( pStyleParticle );
                    TextLine::Type underline = pUnderlineParticle->Value();
                    int& style = (int&) fontDef.style();
                    if ( underline != TextLine::keNone )
                        // Turn underline on
                        style |= RS_FontStyle_Underline;
                    else
                        // Turn underline off
                        style &= ~RS_FontStyle_Underline;
                }
                break;

            case StyleParticle::keOverline:
                {
                    const OverlineStyleParticle* pOverlineParticle = static_cast<const OverlineStyleParticle*>( pStyleParticle );
                    TextLine::Type overline = pOverlineParticle->Value();
                    int& style = (int&) fontDef.style();
                    if ( overline != TextLine::keNone )
                        // Turn overline on
                        style |= RS_FontStyle_Overline;
                    else
                        // Turn overline off
                        style &= ~RS_FontStyle_Overline;
                }
                break;

            case StyleParticle::keFontWeight:
                // Normal, Bold, ...     See enum.
                {
                    const FontWeightStyleParticle* pFontWeightParticle = static_cast<const FontWeightStyleParticle*>( pStyleParticle );
                    FontWeight::Type weight = pFontWeightParticle->Value();
                    int& style = (int&) fontDef.style();
                    if ( weight > FontWeight::keNormal )
                        // Turn bold on
                        style |= RS_FontStyle_Bold;
                    else
                        // Turn bold off
                        style &= ~RS_FontStyle_Bold;
                }
                break;

            case StyleParticle::keVerticalAlignment:
                // Describes vertical relationship of text to insertion point.
                {
                    const VerticalAlignmentStyleParticle* pVerticalAlignmentParticle = static_cast<const VerticalAlignmentStyleParticle*>( pStyleParticle );
                    VerticalAlignment::Type vAlign = pVerticalAlignmentParticle->Value();
                    switch ( vAlign )
                    {
                        case VerticalAlignment::keAscender:
                            this->m_formatState.m_tmpTDef.valign() = RS_VAlignment_Ascent;
                            break;
                        case VerticalAlignment::keXHeight:
                            // Not supported
                            break;
                        case VerticalAlignment::keBaseline:
                            this->m_formatState.m_tmpTDef.valign() = RS_VAlignment_Base;
                            break;
                        case VerticalAlignment::keDescender:
                            this->m_formatState.m_tmpTDef.valign() = RS_VAlignment_Descent;
                            break;
                        case VerticalAlignment::keMid:
                        default:
                            this->m_formatState.m_tmpTDef.valign() = RS_VAlignment_Half;
                            break;
                    }
                }
                break;

            case StyleParticle::keHorizontalAlignment:
                // Describes horizontal relationship of text to insertion point.
                {
                    const HorizontalAlignmentStyleParticle* pHorizontalAlignmentParticle = static_cast<const HorizontalAlignmentStyleParticle*>( pStyleParticle );
                    HorizontalAlignment::Type hAlign = pHorizontalAlignmentParticle->Value();
                    switch ( hAlign )
                    {
                        case HorizontalAlignment::keLeft:
                            this->m_formatState.m_tmpTDef.halign() = RS_HAlignment_Left;
                            break;
                        case HorizontalAlignment::keRight:
                            this->m_formatState.m_tmpTDef.halign() = RS_HAlignment_Right;
                            break;
                        case HorizontalAlignment::keMiddle:
                        default:
                            this->m_formatState.m_tmpTDef.halign() = RS_HAlignment_Center;
                            break;
                    }
                }
                break;

            case StyleParticle::keTrackingAugment:
                // Describes Tracking factor
                {
                    const TrackingAugmentStyleParticle* pTrackingAugmentParticle = static_cast<const TrackingAugmentStyleParticle*>( pStyleParticle );
                    Measure trackingVal = pTrackingAugmentParticle->Value();
                    if ( trackingVal.Units() == Measure::keProportion )
                        this->m_formatState.m_tmpTDef.trackSpacing() = trackingVal.Number();
                }
                break;

            case StyleParticle::keAdvanceAlignment:
                // Describes Vertical Alignment
                {
                    const AdvanceAlignmentStyleParticle* pAdvanceAlignmentParticle = static_cast<const AdvanceAlignmentStyleParticle*>( pStyleParticle );
                    Measure advanceAlignmentVal = pAdvanceAlignmentParticle->Value();
                    if ( advanceAlignmentVal.Units() == Measure::keUnitless )
                        this->m_formatState.m_advanceAlignmentVal = advanceAlignmentVal.Number();
                }
                break;

            case StyleParticle::keJustification:
                {
                    const JustificationStyleParticle* pJustificationParticle = static_cast<const JustificationStyleParticle*>( pStyleParticle );
                    Justification::Type justification = pJustificationParticle->Value();
                    switch ( justification )
                    {
                        case Justification::keRight:
                            this->m_formatState.m_tmpTDef.justify() = RS_Justify_Right;
                            break;
                        case Justification::keCentered:
                            this->m_formatState.m_tmpTDef.justify() = RS_Justify_Center;
                            break;
                        case Justification::keJustified:
                            this->m_formatState.m_tmpTDef.justify() = RS_Justify_Justify;
                            break;
                        case Justification::keLeft:
                        default:
                            this->m_formatState.m_tmpTDef.justify() = RS_Justify_Left;
                            break;
                    }
                }
                break;

            // Unsupported style attributes - will be done if time permits
            case StyleParticle::kePitchFamily:
            case StyleParticle::keLineHeight:
            case StyleParticle::keAltTypefaces:
            case StyleParticle::keSize:
            case StyleParticle::keStrikethrough:
            case StyleParticle::keCaseShift:
            case StyleParticle::keStrokeWeight:
            case StyleParticle::keStrokeBehind:
            case StyleParticle::keBeforePara:
            case StyleParticle::keAfterPara:
            case StyleParticle::keReferenceExpansion:
            case StyleParticle::keStrokeColor:
            default:
                break;
        }

        else
        if ( pTransformParticle ) switch ( pTransformParticle->Type() )
        {
            case TransformParticle::keSkew:
                {
                    //NUMBER n[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
                    //Matrix localMatrix( n );
                    //pTransformParticle->SetMatrix( localMatrix );
                    //curXformMatrix *= localMatrix;

                    // Presently, the GD and DWF renderers do not permit us to apply a transform to the font.
                    // The skew matrix is used for obliquing in mtext.  So, in this case we will "approximate"
                    // obliquing with italics.  All of this is handled in RS_FontEngine::GetRenderingFont; so, here we
                    // use a hack to force obliquing.  Since we are just substituting italics for obliquing, we
                    // just need to set a value for obliquing which is not equal to 1.0.  This is a total hack
                    // and should be redone for next release.
                    this->m_formatState.m_tmpTDef.obliqueAngle() = 1.5;
                }
                break;

            case TransformParticle::keTranslation:
            case TransformParticle::keScale:
            case TransformParticle::keRotation:
            case TransformParticle::keArbitrary:
            case TransformParticle::keNone:
            default:
                // Not supported
                break;
        }
        else
        if ( pPushPopParticle )
        {
            if ( pPushPopParticle->Push() )
            {
                // Push down a copy
                RichTextFormatState* pNewState = new RichTextFormatState();
                *pNewState = this->m_formatState;
                this->m_formatState.m_pNext = pNewState;
            }
            else
            {
                if ( this->m_formatState.m_pNext != NULL )
                {
                    // Pop a copy and restore it
                    RichTextFormatState* pOldState = this->m_formatState.m_pNext;
                    this->m_formatState = *pOldState;
                    delete pOldState;
                }
            }
        }

        pParticle = pParticle->Next();
    }

    // Update font values
    GetFontValues();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting classes for RichTextEngine

//////////////////////////////////////////////////////////////////////////////
// AtomBaseComponent class

AtomBaseComponent::AtomBaseComponent( RS_F_Point position, AtomBaseComponent* pParent )
{
    this->m_pParent = pParent;

    if ( pParent )
    {
        RS_F_Point parentPosition;
        pParent->GetPosition( parentPosition );
        this->m_offset.x = position.x - parentPosition.x;
        this->m_offset.y = position.y - parentPosition.y;
    }
    else
        this->m_offset = position;

    this->m_extent[0].x = 0.0;
    this->m_extent[1].x = 0.0;
    this->m_extent[2].x = 0.0;
    this->m_extent[3].x = 0.0;
    this->m_extent[0].y = 0.0;
    this->m_extent[1].y = 0.0;
    this->m_extent[2].y = 0.0;
    this->m_extent[3].y = 0.0;
}

AtomBaseComponent::~AtomBaseComponent()
{
}

RS_F_Point& AtomBaseComponent::Offset()
{
    return this->m_offset;
}

void AtomBaseComponent::GetPosition( RS_F_Point& position )
{
    if ( this->m_pParent )
    {
        this->m_pParent->GetPosition( position );
        position.x += this->m_offset.x;
        position.y += this->m_offset.y;
    }
    else
        position = this->m_offset;
}

RS_F_Point* AtomBaseComponent::GetExtent()
{
    return this->m_extent;
}

double AtomBaseComponent::GetWidth()
{
    return ( this->m_extent[1].x - this->m_extent[0].x );
}

void AtomBaseComponent::GetPositionedExtent( RS_F_Point* positionedExtent )
{
    for ( int i=0; i<4; ++i )
    {
        positionedExtent[i].x = this->m_offset.x + this->m_extent[i].x;
        positionedExtent[i].y = this->m_offset.y + this->m_extent[i].y;
    }
}

RS_F_Point AtomBaseComponent::GetUpperLeftCorner()
{
    RS_F_Point upperLeftCorner = this->m_extent[3];
    upperLeftCorner.x += this->m_offset.x;
    upperLeftCorner.y += this->m_offset.y;
    return upperLeftCorner;
}

RS_F_Point AtomBaseComponent::GetLowerRightCorner()
{
    RS_F_Point lowerRightCorner = this->m_extent[1];
    lowerRightCorner.x += this->m_offset.x;
    lowerRightCorner.y += this->m_offset.y;
    return lowerRightCorner;
}

void AtomBaseComponent::Translate( RS_F_Point translation )
{
    this->m_offset.x += translation.x;
    this->m_offset.y += translation.y;
}

void AtomBaseComponent::AddExtent( RS_F_Point* ext, bool yUp )
{
    if ( ext[0].x < this->m_extent[0].x )
    {
        this->m_extent[0].x = ext[0].x;
        this->m_extent[3].x = ext[0].x;
    }
    if ( ext[1].x > this->m_extent[1].x )
    {
        this->m_extent[1].x = ext[1].x;
        this->m_extent[2].x = ext[1].x;
    }
    if ( yUp )
    {
        if ( ext[0].y < this->m_extent[0].y )
        {
            this->m_extent[0].y = ext[0].y;
            this->m_extent[1].y = ext[0].y;
        }
        if ( ext[2].y > this->m_extent[2].y )
        {
            this->m_extent[2].y = ext[2].y;
            this->m_extent[3].y = ext[2].y;
        }
    }
    else
    {
        if ( ext[0].y > this->m_extent[0].y )
        {
            this->m_extent[0].y = ext[0].y;
            this->m_extent[1].y = ext[0].y;
        }
        if ( ext[2].y < this->m_extent[2].y )
        {
            this->m_extent[2].y = ext[2].y;
            this->m_extent[3].y = ext[2].y;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// AtomBaseComponentCollection class

AtomBaseComponentCollection::AtomBaseComponentCollection( RS_F_Point position, AtomBaseComponent* pParent ) : AtomBaseComponent( position, pParent )
{
    this->m_components.clear();
}

AtomBaseComponentCollection::~AtomBaseComponentCollection()
{
    size_t numComponents = this->m_components.size();
    for ( size_t i=0; i<numComponents; ++i )
        delete this->m_components[i];
}

void AtomBaseComponentCollection::AddComponent( AtomBaseComponent* pComponent )
{
    if ( pComponent )
        this->m_components.push_back( pComponent );
}

double AtomBaseComponentCollection::GetAscentPosition( bool yUp )
{
    size_t numComponents = this->m_components.size();
    if ( numComponents == 0 )
        return this->m_offset.y;

    double ascentPos;
    double maxAscentPos = this->m_offset.y + this->m_components[0]->GetAscentPosition( yUp );
    for ( size_t i=1; i<numComponents; ++i )
    {
        ascentPos = this->m_offset.y + this->m_components[i]->GetAscentPosition( yUp );
        if ( yUp )
            maxAscentPos = ascentPos > maxAscentPos ? ascentPos : maxAscentPos;
        else
            maxAscentPos = ascentPos < maxAscentPos ? ascentPos : maxAscentPos;
    }

    return maxAscentPos;
}

double AtomBaseComponentCollection::GetDescentPosition( bool yUp )
{
    size_t numComponents = this->m_components.size();
    if ( numComponents == 0 )
        return this->m_offset.y;

    double descentPos;
    double minDescentPos = this->m_offset.y + this->m_components[0]->GetDescentPosition( yUp );
    for ( size_t i=1; i<numComponents; ++i )
    {
        descentPos = this->m_offset.y + this->m_components[i]->GetDescentPosition( yUp );
        if ( yUp )
            minDescentPos = descentPos < minDescentPos ? descentPos : minDescentPos;
        else
            minDescentPos = descentPos > minDescentPos ? descentPos : minDescentPos;
    }

    return minDescentPos;
}

double AtomBaseComponentCollection::GetCaplinePosition( bool yUp )
{
    size_t numComponents = this->m_components.size();
    if ( numComponents == 0 )
        return this->m_offset.y;

    double caplinePos;
    double maxCaplinePos = this->m_offset.y + this->m_components[0]->GetCaplinePosition( yUp );
    for ( size_t i=1; i<numComponents; ++i )
    {
        caplinePos = this->m_offset.y + this->m_components[i]->GetCaplinePosition( yUp );
        if ( yUp )
            maxCaplinePos = caplinePos > maxCaplinePos ? caplinePos : maxCaplinePos;
        else
            maxCaplinePos = caplinePos < maxCaplinePos ? caplinePos : maxCaplinePos;
    }

    return maxCaplinePos;
}

double AtomBaseComponentCollection::GetBaselinePosition( bool yUp )
{
    size_t numComponents = this->m_components.size();
    if ( numComponents == 0 )
        return this->m_offset.y;

    double baselinePos;
    double minBaselinePos = this->m_offset.y + this->m_components[0]->GetBaselinePosition( yUp );
    for ( size_t i=1; i<numComponents; ++i )
    {
        baselinePos = this->m_offset.y + this->m_components[i]->GetBaselinePosition( yUp );
        if ( yUp )
            minBaselinePos = baselinePos < minBaselinePos ? baselinePos : minBaselinePos;
        else
            minBaselinePos = baselinePos > minBaselinePos ? baselinePos : minBaselinePos;
    }

    return minBaselinePos;
}

void AtomBaseComponentCollection::CalculateExtent( bool yUp )
{
    size_t numComponents = this->m_components.size();
    if ( numComponents == 0 )
        return;

    // Add the extents of the line's components
    RS_F_Point positionedExt[4];
    AtomBaseComponent* pComponent = this->m_components[0];
    pComponent->GetPositionedExtent( this->m_extent );
    for ( size_t i=1; i<numComponents; ++i )
    {
        pComponent = this->m_components[i];
        pComponent->GetPositionedExtent( positionedExt );
        this->AddExtent( positionedExt, yUp );
    }
}

void AtomBaseComponentCollection::OutputData( RS_F_Point parentPosition, RS_TextMetrics* pTextMetrics )
{
    this->Translate( parentPosition );

    size_t numComponents = this->m_components.size();
    for ( size_t i=0; i<numComponents; ++i )
    {
        this->m_components[i]->OutputData( this->m_offset, pTextMetrics );
        delete this->m_components[i];
        this->m_components[i] = NULL;
    }
}



//////////////////////////////////////////////////////////////////////////////
// AtomRun class

AtomRun::AtomRun( RS_F_Point position, AtomLine* pParentLine, RichTextFormatState& formatState ) : AtomBaseComponent( position, pParentLine )
{
    // A run must have a parent
    assert( pParentLine );

    this->m_textRun = NULL;
    this->m_textRunLen = 0;
    this->m_textRunInd = 0;
    this->m_charAdvances.clear();
    this->m_pFormatChanges = NULL;

    this->m_advanceAlignment = formatState.m_advanceAlignmentVal;
    this->m_fontAscent = formatState.m_fontAscent;
    this->m_fontCapline = formatState.m_fontCapline;
    this->m_fontDescent = formatState.m_fontDescent;
    this->m_actualHeight = formatState.m_actualHeight;
}

AtomRun::~AtomRun()
{
    // We only need to clean up here if OutputData was not called.
    free(this->m_textRun);
    if ( this->m_pFormatChanges )
    {
        Particle* pParticle = this->m_pFormatChanges;
        this->m_pFormatChanges = (Particle*) this->m_pFormatChanges->Next();
        delete pParticle;
    }
}

void AtomRun::SetTextRun( StRange runContent, unsigned int runInd, Particle* pFormatChanges )
{
    this->m_textRunLen = runContent.Length();
    this->m_textRunInd = runInd;
    this->m_pFormatChanges = pFormatChanges;
    this->m_textRun = (wchar_t*)malloc((this->m_textRunLen + 1) * sizeof(wchar_t));
    wcsncpy(this->m_textRun, runContent.Start(), this->m_textRunLen);
    this->m_textRun[this->m_textRunLen] = L'\0';
}

void AtomRun::Close( RS_FontEngine* pFontEngine, const RS_Font* pFont )
{
    this->CalculateExtent( pFontEngine, pFont );
}

double AtomRun::GetAdvanceAlignment()
{
    return this->m_advanceAlignment;
}

void AtomRun::CalculateExtent( RS_FontEngine* pFontEngine, const RS_Font* pFont )
{
    if ( !pFontEngine || !pFont )
        return;

    pFontEngine->MeasureString( this->m_textRun, this->m_actualHeight, pFont, 0.0, this->m_extent, NULL);
}

double AtomRun::GetAscentPosition( bool yUp )
{
    if ( yUp )
        return this->m_offset.y + this->m_fontAscent;
    else
        return this->m_offset.y - this->m_fontAscent;
}

double AtomRun::GetDescentPosition( bool yUp )
{
    if ( yUp )
        return this->m_offset.y - this->m_fontDescent;
    else
        return this->m_offset.y + this->m_fontDescent;
}


double AtomRun::GetCaplinePosition( bool yUp )
{
    if ( yUp )
        return ( this->m_offset.y + this->m_fontCapline );
    else
        return ( this->m_offset.y - this->m_fontCapline );
}

double AtomRun::GetBaselinePosition( bool /*yUp*/ )
{
    return this->m_offset.y;
}

void AtomRun::OutputData( RS_F_Point parentPosition, RS_TextMetrics* pTextMetrics )
{
    if ( !pTextMetrics )
        return;

    // Translate to absolute position
    this->Translate( parentPosition );

    // Store data in TextMetrics
    this->GetPositionedExtent( pTextMetrics->line_pos[ this->m_textRunInd ].ext );
    pTextMetrics->line_pos[ this->m_textRunInd ].hOffset = this->m_offset.x;
    pTextMetrics->line_pos[ this->m_textRunInd ].vOffset = this->m_offset.y;
    pTextMetrics->line_breaks[ this->m_textRunInd ] = this->m_textRun;
    pTextMetrics->format_changes[ this->m_textRunInd ] = this->m_pFormatChanges;

    // Need to free the text run string
    free(this->m_textRun);

    this->m_textRun = NULL;
    this->m_textRunLen = 0;
    this->m_pFormatChanges = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// AtomLine class

AtomLine::AtomLine( RS_F_Point position, AtomBlock* pParentBlock, RichTextFormatState& formatState, bool fixedLine ) : AtomBaseComponentCollection( position, pParentBlock )
{
    // A line must have a parent
    assert( pParentBlock );

    this->m_initialAscent = formatState.m_fontAscent;
    this->m_fixedLine = fixedLine;
}

AtomLine::~AtomLine()
{
}

void AtomLine::ApplyAdvanceAlignment()
{
    size_t numComponents = this->m_components.size();
    AtomRun* pRun;
    double runTop, vAdjustment, advanceAlignment;
    double lineTop = this->GetUpperLeftCorner().y;
    for ( size_t i=0; i<numComponents; ++i )
    {
        pRun = dynamic_cast<AtomRun*>(this->m_components[i]);
        advanceAlignment = pRun ? pRun->GetAdvanceAlignment() : 0.0;
        if ( advanceAlignment != 0.0 )
        {
            runTop= pRun->GetUpperLeftCorner().y;
            vAdjustment = (lineTop - runTop) * advanceAlignment;
            pRun->Offset().y = pRun->Offset().y + vAdjustment;
        }
    }
}

double AtomLine::AdjustBaseline( bool yUp )
{
    if ( this->m_components.empty() || this->m_fixedLine )
        return 0.0;

    double vAdjustment = 0.0;
    double lineAscentPos = fabs( this->GetAscentPosition( yUp ) );
    double initialLineAscentPos = yUp ?
        fabs( this->m_offset.y + this->m_initialAscent ):
        fabs( this->m_offset.y - this->m_initialAscent );

    if ( lineAscentPos > initialLineAscentPos )
    {
        if ( yUp )
            vAdjustment = initialLineAscentPos - lineAscentPos;
        else
            vAdjustment = lineAscentPos - initialLineAscentPos;
    }

    this->m_offset.y += vAdjustment;
    return vAdjustment;
}

double AtomLine::Close( bool yUp )
{
    double baselineAdjustment = this->AdjustBaseline( yUp );
    this->CalculateExtent( yUp );
    this->ApplyAdvanceAlignment();
    return baselineAdjustment;
}

AtomBlock* AtomLine::GetParentBlock()
{
    return (AtomBlock*) this->m_pParent;
}


//////////////////////////////////////////////////////////////////////////////
// AtomBlock class

AtomBlock::AtomBlock( RS_F_Point position, AtomLine* pParentLine, RichTextFormatState& formatState ) : AtomBaseComponentCollection( position, pParentLine )
{
    this->m_justification = formatState.m_tmpTDef.justify();
}

AtomBlock::~AtomBlock()
{
}


void AtomBlock::Close( bool yUp )
{
    this->CalculateExtent( yUp );
    this->ApplyJustification();
}

AtomLine* AtomBlock::GetParentLine()
{
    return (AtomLine*) this->m_pParent;
}

AtomLine* AtomBlock::GetLastLine()
{
    if ( !this->m_components.empty() )
        return (AtomLine*) this->m_components[ this->m_components.size() - 1 ];
    else
        return NULL;
}

void AtomBlock::ApplyJustification()
{
    switch ( this->m_justification )
    {
    case RS_Justify_Left:
        break;  // We are already left-justified

    case RS_Justify_Center:
        {
            size_t numLines = this->m_components.size();
            AtomBaseComponent* pLine;
            for ( size_t i=0; i<numLines; ++i )
            {
                pLine = this->m_components[i];
                pLine->Offset().x += 0.5 * ( this->GetWidth() - pLine->GetWidth() );
            }
        }
        break;

    case RS_Justify_Right:
        {
            size_t numLines = this->m_components.size();
            AtomBaseComponent* pLine;
            for ( size_t i=0; i<numLines; ++i )
            {
                pLine = this->m_components[i];
                pLine->Offset().x += ( this->GetWidth() - pLine->GetWidth() );
            }
        }
        break;

    case RS_Justify_Justify:
        break; // Not supported
    }
}
