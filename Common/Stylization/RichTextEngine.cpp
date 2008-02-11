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

#include "stdafx.h"
#include "RichTextEngine.h"
#include "SE_Renderer.h"
#include "RS_FontEngine.h"
#include "RS_Font.h"
#include <math.h>

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
}


void RichTextEngine::InitEngine( RS_TextDef* pTDef )
{
    // Check for renderer && font engine
    if ( !m_pSERenderer || !m_pFontEngine || !pTDef )
        return;

    // Initialize format state
    this->m_stateDepth = 0;
    this->m_formatState.m_tmpTDef = *pTDef;
    this->m_formatState.m_trackingVal = 1.0;
    this->m_formatState.m_advanceAlignmentVal = 0.0;
    this->m_formatState.m_pNext = NULL;

    // Initialize position vals
    this->m_yUp = this->m_pSERenderer->YPointsUp();
    this->m_curX = 0.0;
    this->m_curY = 0.0;

    // Initialize font vals
    GetFontValues();

    // Initialize line metrics
    if ( this->m_yUp )
        this->m_topCapline = this->m_fontCapline;
    else
        this->m_topCapline = -this->m_fontCapline;
    this->m_bottomBaseline = this->m_curY;
    this->m_numLines = 0;
    this->m_lineStarts.clear();

    // Initialize bookmark table
    for ( int i = 0; i < kiBookmarkTableSize; i++ )
    {
        this->m_bookmarks[i].xPos = 0.0;
        this->m_bookmarks[i].yPos = 0.0;
    }

    // Initialize text run vectors
    this->m_numRuns = 0;
    this->m_line_pos.clear();
    this->m_line_breaks.clear();
    this->m_format_changes.clear();

    // Get ready for first line
    InitLine( true );
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
    for ( int i = 0; i < 9; i++ )
        xform[i] = this->m_curXform[i];
}

void RichTextEngine::GetRichTextFormatState( RichTextFormatState* pState )
{
    *pState = this->m_formatState;
    pState->m_pNext = NULL;
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

        parserSucceeded = pMText->Parse(s.c_str(),&env).Succeeded();
        pGenerator->Destroy(pMText);

        if ( parserSucceeded && this->m_numRuns > 0 )
        {
            // Fill out the RS_TextMetrics class
            pTextMetrics->line_pos.reserve( this->m_numRuns );
            pTextMetrics->line_pos.resize( this->m_numRuns );
            pTextMetrics->line_breaks.reserve( this->m_numRuns );
            pTextMetrics->line_breaks.resize( this->m_numRuns );
            pTextMetrics->format_changes.reserve( this->m_numRuns );
            pTextMetrics->format_changes.resize( this->m_numRuns );
            for ( unsigned int i = 0; i < this->m_numRuns; i++ )
            {
                pTextMetrics->line_pos[i] = this->m_line_pos[i];
                pTextMetrics->line_breaks[i] = this->m_line_breaks[i];
                pTextMetrics->format_changes[i] = this->m_format_changes[i];
            }

            // Translate extents to their positions
            LinePos* pLinePos = &(pTextMetrics->line_pos[0]);
            for ( unsigned int i = 0; i < this->m_numRuns; i++, pLinePos++ )
            {
                for ( int j = 0; j < 4; j++ )
                {
                    pLinePos->ext[j].x += pLinePos->hOffset;
                    pLinePos->ext[j].y += pLinePos->vOffset;
                }
            }

            // Make horizontal alignment adjustments and advance alignment adjustments
            RS_F_Point lineExt[4];
            this->m_formatState = initialState;
            GetFontValues();
            unsigned int startRun = this->m_lineStarts[0];
            unsigned int stopRun;
            for ( unsigned int i = 0; i < this->m_numLines; i++ )
            {
                // Get indices of the runs which comprise the line
                stopRun = i < this->m_numLines - 1 ? this->m_lineStarts[i+1] - 1 : this->m_numRuns - 1;

                // Get line extent and max height
                pLinePos = &(pTextMetrics->line_pos[ startRun ]);
                RS_Bounds lineBounds(pLinePos->ext[0].x, pLinePos->ext[0].y, pLinePos->ext[2].x, pLinePos->ext[2].y);
                double maxHeight = fabs(pLinePos->ext[3].y);
                double runHeight;
                for ( unsigned int j = startRun + 1; j <= stopRun; j++, pLinePos++ )
                {
                    lineBounds.add_point( pLinePos->ext[0] );
                    lineBounds.add_point( pLinePos->ext[2] );
                    runHeight = fabs(pLinePos->ext[3].y);
                    maxHeight = runHeight > maxHeight ? runHeight : maxHeight;
                }
                RS_F_Point lineExt[4];
                lineBounds.get_points( lineExt );

                // Apply alignments as specified by formatting changes
                double hAlignOffset = GetHorizontalAlignmentOffset( this->m_formatState.m_tmpTDef.halign(), lineExt );
                pLinePos = &(pTextMetrics->line_pos[ startRun ]);
                for ( unsigned int j = startRun; j <= stopRun; j++, pLinePos++ )
                {
                    // Get vertical adjustment
                    double vAdjustment = 0.0;
                    this->ApplyFormatChanges( pTextMetrics->format_changes[j] );
                    if ( this->m_formatState.m_advanceAlignmentVal != 0.0 )
                    {
                        runHeight = fabs(pLinePos->ext[3].y);
                        vAdjustment = (maxHeight - runHeight) * this->m_formatState.m_advanceAlignmentVal;
                    }
                    if ( this->m_yUp )
                    {
                        pLinePos->hOffset += hAlignOffset;
                        pLinePos->vOffset += vAdjustment;
                        for ( int k = 0; k < 4; k++ )
                        {
                            pLinePos->ext[k].x += hAlignOffset;
                            pLinePos->ext[k].y += vAdjustment;
                        }
                    }
                    else
                    {
                        pLinePos->hOffset += hAlignOffset;
                        pLinePos->vOffset -= vAdjustment;
                        for ( int k = 0; k < 4; k++ )
                        {
                            pLinePos->ext[k].x += hAlignOffset;
                            pLinePos->ext[k].y -= vAdjustment;
                        }
                    }
                }
                GetFontValues(); // Make sure font values are current

                // Next Line
                startRun = stopRun + 1;
            }

            // Get bounds of text
            RS_Bounds textBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
            for ( unsigned int i = 0; i < this->m_numRuns; i++)
            {
                textBounds.add_point( pTextMetrics->line_pos[i].ext[0] );
                textBounds.add_point( pTextMetrics->line_pos[i].ext[2] );
            }

            // Get vertical alignment adjustment
            // Note that we pick up vertical alignment from the tmpTextDef which updated with calls to ApplyFormatChanges.
            // This way we use the final setting of vertical alignment which will reflect any local changes in the
            // formatted string.
            double textTop = this->m_yUp ? textBounds.maxy : textBounds.miny;
            double textBottom = this->m_yUp ? textBounds.miny : textBounds.maxy;
            double vAlignOffset = GetVerticalAlignmentOffset(this->m_formatState.m_tmpTDef.valign(), textTop, this->m_topCapline, textBottom, this->m_bottomBaseline );

            // Apply vertical alignment offset
            pLinePos = &(pTextMetrics->line_pos[ 0 ]);
            for ( unsigned int i = 0; i < this->m_numRuns; i++, pLinePos++ )
            {
                pLinePos->vOffset += vAlignOffset;
                for ( int j = 0; j < 4; j++ )
                    pLinePos->ext[j].y += vAlignOffset;
            }
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
    this->m_actualHeight = this->m_pFontEngine->MetersToPixels( fontDef.units(), fontDef.height());
    double scale = this->m_actualHeight / pFont->m_units_per_EM;
    this->m_fontAscent = fabs( pFont->m_ascender * scale );
    this->m_fontDescent = fabs( pFont->m_descender * scale );
    double fontHeight = fabs( pFont->m_height * scale );
    this->m_lineHeight = this->m_formatState.m_tmpTDef.linespace() * fontHeight;
    this->m_fontCapline = fabs( pFont->m_capheight * scale );
}

double RichTextEngine::ConvertToPixels( double val, Measure::UnitType unit )
{
    double ret;
    switch ( unit )
    {
    case Measure::keUnitless: // Unitless; just a number
        ret = 0.0;  // Value has no meaning in our context
        break;
    case Measure::keModel:    // Units of the surrounding model space
        ret = this->m_pFontEngine->MetersToPixels( RS_Units_Model, val);
        break;
    case Measure::kePixels:   // Physical device units
        ret = val;
        break;
    case Measure::kePoints:   // 1/72 of an inch; Twips = Points * 20, Pica = 12 Points
        val = (val * 72.0) * .0254;
        ret = this->m_pFontEngine->MetersToPixels( RS_Units_Device, val );
        break;
    case Measure::keEm:       // an "em" (1.0 times height of current font.)
        {
            // This is calculated as ( val * pFont->m_units_per_EM ) * ( this->m_actualHeight / pFont->m_units_per_EM )
            // which reduces to the following:
            ret = val * this->m_actualHeight;
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

void RichTextEngine::InitLine( bool fixed )
{
    this->m_numLines++;
    if ( this->m_numRuns > 0 )
        this->m_lineStarts.push_back( this->m_numRuns - 1 );
    else
        this->m_lineStarts.push_back( 0 );
    this->m_fixedLine = fixed;

    if ( this->m_yUp )
    {
        this->m_lineMaxAscentPos = this->m_curY + this->m_fontAscent;
        this->m_lineMinDescentPos = this->m_curY - this->m_fontDescent;
        double newCapline = this->m_curY + this->m_fontCapline;
        if ( newCapline > this->m_topCapline )
            this->m_topCapline = newCapline;
        if ( this->m_curY < this->m_bottomBaseline )
            this->m_bottomBaseline = this->m_curY;
    }
    else
    {
        this->m_lineMaxAscentPos = this->m_curY - this->m_fontAscent;
        this->m_lineMinDescentPos = this->m_curY + this->m_fontDescent;
        double newCapline = this->m_curY - this->m_fontCapline;
        if ( newCapline < this->m_topCapline )
            this->m_topCapline = newCapline;
        if ( this->m_curY > this->m_bottomBaseline )
            this->m_bottomBaseline = this->m_curY;
    }
}

void RichTextEngine::TrackLineMetrics()
{
    if ( this->m_yUp )
    {
        // Keep track of max ascent position
        double newAscentPos = this->m_curY + this->m_fontAscent;
        if ( newAscentPos > this->m_lineMaxAscentPos )
        {
            // If this baseline is fixed, then just record the new max ascent
            if ( this->m_fixedLine )
                this->m_lineMaxAscentPos = newAscentPos;
            else
            {
                // This baseline is not fixed and must be adjusted down.
                // The max ascent will remain unchanged.
                double delta = newAscentPos - this->m_lineMaxAscentPos;
                for ( unsigned int i = this->m_lineStarts[ this->m_numLines - 1 ]; i < this->m_numRuns - 1; i++ )
                    this->m_line_pos[i].vOffset -= delta;
                this->m_curY -= delta;
            }
        }

        // Keep track of min descent position
        double newDescentPos = this->m_curY - this->m_fontDescent;
        if ( newDescentPos < this->m_lineMinDescentPos )
            this->m_lineMinDescentPos = newDescentPos;
    }
    else
    {
        // Keep track of max ascent position
        double newAscentPos = this->m_curY - this->m_fontAscent;
        if ( newAscentPos < this->m_lineMaxAscentPos )
        {
            // If this baseline is fixed, then just record the new max ascent
            if ( this->m_fixedLine )
                this->m_lineMaxAscentPos = newAscentPos;
            else
            {
                // This baseline is not fixed and must be adjusted down.
                // The max ascent will remain unchanged.
                double delta = this->m_lineMaxAscentPos - newAscentPos;
                for ( unsigned int i = this->m_lineStarts[ this->m_numLines - 1 ]; i < this->m_numRuns - 1; i++ )
                    this->m_line_pos[i].vOffset += delta;
                this->m_curY += delta;
            }
        }

        // Keep track of min descent position
        double newDescentPos = this->m_curY + this->m_fontDescent;
        if ( newDescentPos > this->m_lineMinDescentPos )
            this->m_lineMinDescentPos = newDescentPos;
    }
}

void RichTextEngine::ApplyLocationOperations( const ILocation* pLocation )
{
    if ( !pLocation )
        return;

    // Update location
    // ILocation::SemanticType semantics = pLocation->Semantics();
    const LocationParticle* pOperations = pLocation->Operations();
    if ( !pOperations )
        TrackLineMetrics();
    else
    while ( pOperations )
    {
        LocationParticle::LocationParticleType opType = pOperations->Type();

        switch ( opType )
        {
        case LocationParticle::keLineBreak:
            {
                double leading = this->m_lineHeight - (this->m_fontAscent + this->m_fontDescent);
                this->m_curX = 0.0;
                if ( this->m_yUp )
                    this->m_curY = this->m_lineMinDescentPos - leading - this->m_fontAscent;
                else
                    this->m_curY = this->m_lineMinDescentPos + leading + this->m_fontAscent;

                InitLine( false );
            }
            break;
        case LocationParticle::keBookmark:
            {
                TrackLineMetrics();

                // Bookmark our location
                const BookmarkLocationParticle* pBookmarkParticle = static_cast<const BookmarkLocationParticle*>(pOperations);
                int index = pBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                {
                    this->m_bookmarks[ index ].xPos = this->m_curX;
                    this->m_bookmarks[ index ].yPos = this->m_curY;
                }
            }
            break;
        case LocationParticle::keReturnToBookmark:
            {
                // Go to bookmark
                const ReturnToBookmarkLocationParticle* pReturnToBookmarkParticle = static_cast<const ReturnToBookmarkLocationParticle*>(pOperations);
                int index = pReturnToBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                {
                    this->m_curX = this->m_bookmarks[ index ].xPos;
                    this->m_curY = this->m_bookmarks[ index ].yPos;
                }

                TrackLineMetrics();
            }
            break;
        case LocationParticle::keConditionalReturnToBookmark:
            {
                const ConditionalReturnToBookmarkLocationParticle* pCondReturnToBookmarkParticle = static_cast<const ConditionalReturnToBookmarkLocationParticle*>(pOperations);
                int index = pCondReturnToBookmarkParticle->Index();
                if ( index < kiBookmarkTableSize )
                {
                    if ( pCondReturnToBookmarkParticle->Condition() == ConditionalReturnToBookmarkLocationParticle::keFarthestAdvance )
                        this->m_curX = this->m_bookmarks[ index ].xPos > this->m_curX ? this->m_bookmarks[ index ].xPos : this->m_curX;
                    else // ConditionalReturnToBookmarkLocationParticle::keLeastAdvance
                        this->m_curX = this->m_bookmarks[ index ].xPos < this->m_curX ? this->m_bookmarks[ index ].xPos : this->m_curX;
                    this->m_curY = this->m_bookmarks[ index ].yPos;
                }

                TrackLineMetrics();
            }
            break;
        case LocationParticle::keRelative:
            {
                // Get advance and rise
                const RelativeLocationParticle* pRelParticle = static_cast<const RelativeLocationParticle*>(pOperations);
                Measure advanceMeasure = pRelParticle->Advance();
                Measure riseMeasure = pRelParticle->Rise();

                // Adjust current position
                this->m_curX =
                    advanceMeasure.Units() == Measure::keProportion ?
                    this->m_curX *= advanceMeasure.Number() :
                    this->m_curX += ConvertToPixels( advanceMeasure.Number(), advanceMeasure.Units() );
                if ( riseMeasure.Units() == Measure::keProportion )
                    this->m_curY *= riseMeasure.Number();
                else
                if ( this->m_yUp )
                    this->m_curY += ConvertToPixels( riseMeasure.Number(), riseMeasure.Units() );
                else
                    this->m_curY -= ConvertToPixels( riseMeasure.Number(), riseMeasure.Units() );

                TrackLineMetrics();
            }
            break;
        case LocationParticle::kePoint:
            {
                const PointLocationParticle* pPointParticle = static_cast<const PointLocationParticle*>(pOperations);
                this->m_curX = (double) pPointParticle->X();
                this->m_curY = (double) pPointParticle->Y();

                InitLine( true );
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

    } while ( pOperations );
}

Status RichTextEngine::TextRun(ITextRun* pTextRun,IEnvironment*)
{
    // Calculate the run ind
    this->m_numRuns++;

    // Store runContent
    StRange content = pTextRun->Contents();
    size_t len = content.Length();
    wchar_t* contentCopy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
    wcsncpy(contentCopy, content.Start(), len);
    contentCopy[len] = L'\0';
    this->m_line_breaks.push_back( contentCopy );

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
    this->m_format_changes.push_back( pFormatChanges );

    // Apply formatting changes
    ApplyFormatChanges( pFormatChanges );

    // Apply location operations
    ApplyLocationOperations( pTextRun->Location() );

    // Calculate the extent and set the position of text run
    // Note that the RS_TextMetrics class expects that the extent is translated to the position.  Here
    // we simply calculate the extent.  The position translation will be done in the ParseFormattedString
    // method once all text runs have been processed and their positions are set.
    if ( this->m_formatState.m_trackingVal != 1.0 )
    {
        // We need to render each char of the text run separately so that tracking can be applied.
        // Eventually, for the sake of efficiency, we should investigate ways to push this down to the
        // individual renderers.

        // Create a text run for each char.
        this->m_line_breaks.pop_back();
        unsigned int firstRun = this->m_numRuns - 1;
        this->m_numRuns += ( content.Length() - 1);
        this->m_line_pos.resize( this->m_numRuns );
        LinePos* pLinePos = &this->m_line_pos[ firstRun ];
        const RS_Font* pFont = this->m_pFontEngine->GetRenderingFont( this->m_formatState.m_tmpTDef );
        for ( int i = 0; i < content.Length(); i++, pLinePos++ )
        {
            // First create the text run
            wchar_t* charCopy = (wchar_t*)alloca(2 * sizeof(wchar_t));
            charCopy[0] = content[i];
            charCopy[1] = L'\0';
            this->m_line_breaks.push_back( charCopy );

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
                this->m_format_changes.push_back( pCharTransforms );
            }

            // Set the char position and measure it
            pLinePos->hOffset = this->m_curX;
            pLinePos->vOffset = this->m_curY;
            this->m_pFontEngine->MeasureString( charCopy, this->m_actualHeight, pFont, 0.0, pLinePos->ext, NULL);

            // Apply tracking value
            double charWidth = this->m_line_pos[ firstRun + i ].ext[1].x;
            if ( this->m_formatState.m_trackingVal > 1.0 )
            {
                // If tracking is greater than 1, we need to stretch the extent so that underline/overline will
                // draw for the full extent of the char.  If less that 1, the overline/underline will overwrite
                // a bit but we want the last char to have a complete overline/underline.
                pLinePos->ext[1].x *= this->m_formatState.m_trackingVal;
                pLinePos->ext[2].x = pLinePos->ext[1].x;
            }

            // Advance to the end of this char while allowing for tracking value.
            this->m_curX = ( charWidth * this->m_formatState.m_trackingVal ) + this->m_curX;
        }
    }
    else
    {
        this->m_line_pos.resize( this->m_numRuns );
        this->m_line_pos[ this->m_numRuns - 1 ].hOffset = this->m_curX;
        this->m_line_pos[ this->m_numRuns - 1 ].vOffset = this->m_curY;
        const RS_Font* pFont = this->m_pFontEngine->GetRenderingFont( this->m_formatState.m_tmpTDef );
        this->m_pFontEngine->MeasureString( contentCopy, this->m_actualHeight, pFont, 0.0, this->m_line_pos[ this->m_numRuns - 1 ].ext, NULL);

        // Advance to the end of the text run
        this->m_curX = this->m_line_pos[ this->m_numRuns - 1 ].ext[1].x + this->m_curX;
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

            case StyleParticle::kePitchFamily:
            case StyleParticle::keCharacterSet:
                // Not used by our font manager.
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
                            double currHeight = this->m_actualHeight * m_pSERenderer->GetDrawingScale();
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
                        this->m_formatState.m_trackingVal = trackingVal.Number();
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

            // Unsupported style attributes - will be done if time permits
            case StyleParticle::keLineHeight:
            case StyleParticle::keAltTypefaces:
            case StyleParticle::keSize:
            case StyleParticle::keStrikethrough:
            case StyleParticle::keCaseShift:
            case StyleParticle::keStrokeWeight:
            case StyleParticle::keStrokeBehind:
            case StyleParticle::keJustification:
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
