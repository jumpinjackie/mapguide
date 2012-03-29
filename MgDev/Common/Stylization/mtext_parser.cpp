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
#include "atom_element.h"
#include "mtext_parser.h"
#include <wchar.h> // for _wtoi

#ifndef _WIN32// for Linux builds only.
double _wtof(const wchar_t* _Str);
int    _wtoi(const wchar_t *_Str);
#endif

#define MTEXT_PARSER_NAME /*MSG0*/L"MTEXT"

// Reserve indices for specific purposes.
#define STACK_LEFT_BOOKMARK_INDEX  0
#define STACK_RIGHT_BOOKMARK_INDEX 1

/*

This is the minimum set of MTEXT opcodes that need to be supported,
and is derived from the Topobase requirements.

\A#;  Vertical Alignment
\C#;  AutoCAD color index,
\c#;  True color in decimal
\ffont[b#][i#][p#][cN]  TrueType font override
\H#;  Text height
\L  Underline on
\l  Underline off
\N  Column end
\O  Overline off
\o  Overline off
\Q#;  Obliquing Angle
\S[numer]sep[char][denom];  Stack
\T#;  Tracking factor
\\ (double backslash)  produces a backslash
{  Initiate MText override.
}  Terminate MTEXT override
\~  Non-breaking space
Carriage Return  Line end (soft break)
%%c or %%C  Diameter symbol

This parser also supports

\U+xxxx CIF notation
%< .. >% field notation (delegating the interpretation of the contents
to the IReferenceResolver)

General Format:
%<\EvalId FieldCode \f "Format" \href "Link">%

Example of use of field notation:
Drawing created by %<\AcVar author>% on %<\AcVar SaveDate \f "M/d/yyyy">%

*/

MTextParser::MTextParser(ATOM::IGenerator* pGenerator)
: m_pGenerator(pGenerator)
{
}

// Parse a markup string by creating an "environment" with all the
// ambient settings, then combine that with a string to parse, and
// give it to this method.
ATOM::Status MTextParser::Parse(const ATOM::StRange sMarkup,ATOM::IEnvironment* pEnv)
{
    // Nice stack-based implementation; keeps things simple.
    // gee, and it's thread-safe, too!
    MTextParseInstance Instance(sMarkup,pEnv);

    return Instance.Parse();
}


ATOM::IGenerator* MTextParser::GetGenerator()
{
    return m_pGenerator;
}




// Parse a markup string by creating an "environment" with all the
// ambient settings, then combine that with a string to parse, and
// give it to this method.
ATOM::Status MTextParseInstance::Parse()
{
    ATOM::Status eRet = ATOM::Status::keOk;

    eRet = m_pEnv->Sink()->Initialize(m_pEnv);
    if(eRet.Succeeded()) {
        //__try {
            m_sHere.Set(m_sMarkup.Start(),1);
            eRet = ParseContext(/* No outer TextRun context*/NULL);
        //}
        //__finally {
            m_pEnv->Sink()->Terminate(m_pEnv);
        //}
    }
    return eRet;
}


ATOM::Status MTextParseInstance::SendMetacharacter(TextRunElement& Run,const ATOM::StRange& sMeta)
{
    // Meta-characters... we're not keeping a working buffer, so we
    // break the run around these characters.
    ATOM::Status eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    Run.Contents() = sMeta;

    return SendTextRunNotification(Run);
}

// Only extends the Run.Length, unless reset.
void MTextParseInstance::UpdateContentsPointer(TextRunElement& Run,int iAdvance)
{
    if(Run.IsReset()) {
        Run.Contents().SetStart(m_sHere.Start());
        Run.Contents().SetLength(iAdvance);
    }
    else
        Run.Contents().AddLength(iAdvance);
}


// Scans up to the first occurance of character ch in the indicated string.
// Returns the string before that character (ie, not including it) while
// updating the source string's reference to begin after it.
// Kind of like a strtok() for StRange types.
/*ATOM::StRange MTextParseInstance::ScanFor(ATOM::CHARTYPE ch,ATOM::StRange& sString)
{
    ATOM::StRange sRet = sString;

    int i=0;
    while(sString[i] && sString[i] != ch)
        i++;

    // update the incoming string.
    sString.MoveStart(i + (sString[i]==ch? 1 : 0)); // +1 past the character.

    // and return out the parsed-off bit.
    sRet.SetLength(i);

    return sRet;
}*/


// Implements the \A#; command
ATOM::Status MTextParseInstance::Parse_A(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    if(!this->ParseForParameter(parm).Succeeded())
        return Abandon(ATOM::Status::keUnexpectedCharacter,parm);

    ATOM::NUMBER nAlign;
    switch(parm[0]) {
    default:
    case '0': nAlign = 0.0f; break;
    case '1': nAlign = 0.5f; break;
    case '2': nAlign = 1.0f; break;
    }

    Run.Style().AddDelta(ATOM::AdvanceAlignmentStyleParticle(ATOM::Measure(nAlign,ATOM::Measure::keUnitless,&parm)));

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}

// Implements the \C#; command.
ATOM::Status MTextParseInstance::Parse_C(TextRunElement& Run)
{
    m_sHere.Move(1);

    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nACI;
    if(!this->GetNumber(parm,nACI).Succeeded())
        return Abandon(ATOM::Status::keIncompleteString,parm);

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    int iACI = (int)nACI;
    // Out of bounds, or not an int?
    if(iACI < 0 || iACI > 255 || (ATOM::NUMBER)iACI != nACI)
        return Abandon(ATOM::Status::keInvalidArg,parm);

    Run.Style().AddDelta(ATOM::FillColorStyleParticle(ATOM::Color(sm_lAciColorTable[iACI])));

    return ATOM::Status::keOk;
}

// Implements the \c#; command.
ATOM::Status MTextParseInstance::Parse_c(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nColor;
    if(!GetNumber(parm,nColor).Succeeded())
        return Abandon(ATOM::Status::keUnexpectedCharacter,parm);

    // Advance past opcode; leaves us in the right place if Abandon is declined.
    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    long lColor = (long)nColor;
    if(lColor < 0 || lColor > 0xFFFFFF || (ATOM::NUMBER)lColor != nColor)
        return Abandon(ATOM::Status::keInvalidArg,parm);

    // We need to swap things from BGR to RGB
    lColor = ATOM::Color(lColor).LongABGR();

    Run.Style().AddDelta(ATOM::FillColorStyleParticle(ATOM::Color((lColor)|0xFF000000)));

    return ATOM::Status::keOk;
}

// Implements the \fname|b#|i#|p#|c#; command.
ATOM::Status MTextParseInstance::Parse_f(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    if(!ParseForParameter(parm).Succeeded())
        return Abandon(ATOM::Status::keIncompleteString,parm);

    ATOM::StRange s = parm.Split('|');
    Run.Style().AddDelta(ATOM::TypefaceStyleParticle(s));
    do {
        s = parm.Split('|');

        switch(s[0]) {
        case 'b':
            Run.Style().AddDelta(ATOM::FontWeightStyleParticle(s[1] == '1'? ATOM::FontWeight::keBold  :
                                                                            ATOM::FontWeight::keNormal));
            break;

        case 'i':
            Run.Style().AddDelta(ATOM::ItalicStyleParticle(s[1] == '1'));
            break;

        case 'c':
            Run.Style().AddDelta(ATOM::CharacterSetStyleParticle((ATOM::CharacterSet::Type)_wtoi(s.Start()+1)));
            break;

        case 'p':
            Run.Style().AddDelta(ATOM::PitchFamilyStyleParticle((ATOM::PitchFamily::Type)_wtoi(s.Start()+1)));
            break;

        default:
            return Abandon(ATOM::Status::keUnknownMarkup,parm);
        }
    }while(parm.Length() > 0);

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}

// Implements the \H#; and \H#x; command variations.
ATOM::Status MTextParseInstance::Parse_H(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nHeight;
    if(!GetNumber(parm,nHeight).Succeeded())
        return Abandon(ATOM::Status::keIncompleteString,parm);

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    double dHeight = (double)nHeight;
    // Out of bounds?
    if(dHeight <= 0.0 )
        return Abandon(ATOM::Status::keInvalidArg,parm);

    if(parm.Last() == 'x') {
        Run.Style().AddDelta(ATOM::CapSizeStyleParticle(ATOM::Measure(nHeight,ATOM::Measure::keProportion,&parm)));
    }
    else {
        Run.Style().AddDelta(ATOM::CapSizeStyleParticle(ATOM::Measure(nHeight,ATOM::Measure::keModel,&parm)));
    }

    return ATOM::Status::keOk;
}

// Implements the \L command.
ATOM::Status MTextParseInstance::Parse_L(TextRunElement& Run)
{
    Run.Style().AddDelta(ATOM::UnderlineStyleParticle(ATOM::TextLine::keSingle));

    // Parameterless opcode, so no need to advance

    return ATOM::Status::keOk;
}

// Implements the \l command.
ATOM::Status MTextParseInstance::Parse_l(TextRunElement& Run)
{
    Run.Style().AddDelta(ATOM::UnderlineStyleParticle(ATOM::TextLine::keNone));

    // Parameterless opcode, so no need to advance

    return ATOM::Status::keOk;
}

ATOM::Status MTextParseInstance::Parse_N(TextRunElement& Run)
{
    Run.Location().SetSemantics(ATOM::ILocation::keFlowColumn);
    Run.Location().AddOperation(ATOM::LineBreakLocationParticle());

    // Parameterless opcode, so no need to advance

    return ATOM::Status::keOk;
}

ATOM::Status MTextParseInstance::Parse_O(TextRunElement& Run)
{
    Run.Style().AddDelta(ATOM::OverlineStyleParticle(ATOM::TextLine::keSingle));

    // Parameterless opcode, so no need to advance

    return ATOM::Status::keOk;
}

ATOM::Status MTextParseInstance::Parse_o(TextRunElement& Run)
{
    Run.Style().AddDelta(ATOM::OverlineStyleParticle(ATOM::TextLine::keNone));

    // Parameterless opcode, so no need to advance

    return ATOM::Status::keOk;
}

ATOM::Status MTextParseInstance::Parse_Q(TextRunElement& Run)
{
    m_sHere.Move(1);

    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nAng;
    if(!GetNumber(parm,nAng).Succeeded())
       return Abandon(ATOM::Status::keIncompleteString,parm);

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    double dAng = (double)nAng;
    // Out of bounds?
    if(dAng < -85.0 || dAng > 85.0 )
        return Abandon(ATOM::Status::keInvalidArg,parm);

    if(nAng == 0.0)
        Run.Transform().RemoveSameTypeTransform(ATOM::SkewTransformParticle(0,0));
    else
        Run.Transform().ReplaceTransform(ATOM::SkewTransformParticle(ATOM::DegreeRadialMeasure(nAng),0));

    return ATOM::Status::keOk;
}


//                         over
// Produces a traditional ----- fraction.
//                        under
ATOM::Status MTextParseInstance::Parse_S_OverUnder(TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom)
{
    // This guy is left justified, regardless of what's going on outside.
    // (But let's remember what that (outside) justification is.)
    ATOM::JustificationStyleParticle* pOldJustification = (ATOM::JustificationStyleParticle*)(Run.Style().GetDescriptionParticle(ATOM::StyleParticle::keJustification));
    ATOM::Justification::Type eOldJustification = pOldJustification != NULL ?
                                                  pOldJustification->Value() :
                                                  ATOM::Justification::keLeft;

    if(eOldJustification != ATOM::Justification::keCentered)
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keCentered));

    // also get the current underline / overline settings
    ATOM::UnderlineStyleParticle* pOldUnderline = (ATOM::UnderlineStyleParticle*)(Run.Style().GetDescriptionParticle(ATOM::StyleParticle::keUnderline));
    ATOM::TextLine::Type eOldUnderline = (pOldUnderline != NULL)? pOldUnderline->Value() : ATOM::TextLine::keNone;

    ATOM::OverlineStyleParticle* pOldOverline = (ATOM::OverlineStyleParticle*)(Run.Style().GetDescriptionParticle(ATOM::StyleParticle::keOverline));
    ATOM::TextLine::Type eOldOverline = (pOldOverline != NULL)? pOldOverline->Value() : ATOM::TextLine::keNone;

    bool bHasNumer = sNumer.Length() > 0;
    bool bHasDenom = sDenom.Length() > 0;

    // Hack: read the BorderLineStyleParticle stuff below.
    bool bNumerLine = false;
    bool bDenomLine = false;

    //
    //  Process the "numerator"
    //
    if(bHasNumer) {
        // Full stack (not fake superscript) so add some additional info.
        if(bHasDenom) {
            Run.Location().AddSemantic(ATOM::ILocation::keInlineBlock);
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // We have to come back to this place... remember it.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));

            // Short-term hack, instead of relying on BorderLineStyleParticle.
            // See the OverlineStyleParticle in the corresponding code below for notes.
            if(sDenom.Length() < sNumer.Length() && eOldUnderline != ATOM::TextLine::keSingle) {
                Run.Style().AddDelta(ATOM::UnderlineStyleParticle(ATOM::TextLine::keSingle));
                bNumerLine = true;
            }
        }

        // Indicate that this is the "superscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSuperscript);

        Run.Contents() = sNumer;

        // This location stuff is subject to some fiddling (part 1)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        // Send the Numerator along
        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // Disable any underline from the short-term hack
        if(bNumerLine)
        {
            Run.Style().AddDelta(ATOM::UnderlineStyleParticle(eOldUnderline));
            bNumerLine = false;
        }

        // End of superscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSuperscript);
        // This location stuff is subject to some fiddling (part 2)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));
    }

    //
    //  Process the "denominator"
    //
    if(bHasDenom) {
        // Full stack (not fake subscript) so add some additional info.
        if(bHasNumer) {
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // Remember how far out we came, because we need to come back here.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX));
            // Now, back to the left edge in preparation for the denom.
            Run.Location().AddOperation(ATOM::ReturnToBookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));

            // This is what puts the line in between the two parts
            // Removed for now...
            //Run.Style().AddDelta(ATOM::TopBorderLineStyleParticle(ATOM::BorderLine::keSolid));
            // ... instead, we temporarily do this.  We assume, based on some use-case knowledge
            // of the stacked fraction, that these are likely to be numeric, and numbers are usually
            // fixed-width, even in variable-width fonts.
            if(sDenom.Length() >= sNumer.Length() && eOldOverline != ATOM::TextLine::keSingle) {
                Run.Style().AddDelta(ATOM::OverlineStyleParticle(ATOM::TextLine::keSingle));
                bDenomLine = true;
            }
        }

        // Indicate that this is the "subscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSubscript);

        Run.Contents() = sDenom;

        // This location stuff is subject to some fiddling (part 3)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));

        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // Disable any overline from the short-term hack
        if(bDenomLine)
        {
            Run.Style().AddDelta(ATOM::OverlineStyleParticle(eOldOverline));
            bDenomLine = false;
        }

        // End of subscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSubscript);
        // This location stuff is subject to some fiddling (part 4)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        if(bHasNumer) {
            // No longer in the "stack" region
            Run.Location().AddSemantic(ATOM::ILocation::keEndInlineBlock);
            // Go as far as the
            Run.Location().AddOperation(ATOM::ConditionalReturnToBookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX,
                                                                                      ATOM::ConditionalReturnToBookmarkLocationParticle::keFarthestAdvance ));
            // End the line
            // Removed, for now.  See above.
            //Run.Style().AddDelta(ATOM::TopBorderLineStyleParticle(ATOM::BorderLine::keNoBorder));
        }
    }

    if(eOldJustification != ATOM::Justification::keCentered)
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(eOldJustification));

    m_sHere.SetStart(sDenom.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}

//                                      1 /
//  Implements a "vulgar" fraction, ie,  /  and such.
//                                      / 2
ATOM::Status MTextParseInstance::Parse_S_Vulgar(TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom)
{
    bool bHasNumer = sNumer.Length() > 0;
    bool bHasDenom = sDenom.Length() > 0;

    //
    //  Process the "numerator"
    //
    if(bHasNumer) {
        // Full stack (not fake superscript) so add some additional info.
        if(bHasDenom) {
            Run.Location().AddSemantic(ATOM::ILocation::keInlineBlock);
        }

        // Indicate that this is the "superscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSuperscript);

        Run.Contents() = sNumer;

        // This location stuff is subject to some fiddling (part 1)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        // Send the Numerator along
        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of superscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSuperscript);
        // This location stuff is subject to some fiddling (part 2)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5f,ATOM::Measure::keEm,NULL) ));



        if(bHasDenom) {
            // A bit of "hand" kerning to tuck the slash under the numerator.
            Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(-0.4f,ATOM::Measure::keEm,NULL),
                                                                       ATOM::Measure() ));

            // The slash.
            Run.Contents() = L"/";

            // Send the slash along
            ATOM::Status eRet = SendTextRunNotification(Run);
            if(!eRet.Succeeded())
                return eRet;

            // Ditto: more hand kerning so the denominator starts under the slash.
            // (But this is against the slashe's em size.)
            Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(-0.4f,ATOM::Measure::keEm,NULL),
                                                                       ATOM::Measure() ));
        }
    }

    //
    //  Process the "denominator"
    //
    if(bHasDenom) {
        // Indicate that this is the "subscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSubscript);

        Run.Contents() = sDenom;

        // This location stuff is subject to some fiddling (part 3)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));

        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of subscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSubscript);
        // This location stuff is subject to some fiddling (part 4)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        if(bHasNumer) {
            // No longer in the "stack" region
            Run.Location().AddSemantic(ATOM::ILocation::keEndInlineBlock);

        }
    }

    m_sHere.SetStart(sDenom.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}


//
// Produces a tolerance (two left-justified over/and-under arrangement, sans fraction line)
//
ATOM::Status MTextParseInstance::Parse_S_Tolerance(TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom)
{
    // This guy is left justified, regardless of what's going on outside.
    // (But let's remember what that (outside) justification is.)
    ATOM::JustificationStyleParticle* pOldJustification = (ATOM::JustificationStyleParticle*)(Run.Style().GetDescriptionParticle(ATOM::StyleParticle::keJustification));
    ATOM::Justification::Type eOldJustification = pOldJustification != NULL ?
                                                  pOldJustification->Value() :
                                                  ATOM::Justification::keLeft;

    if(eOldJustification != ATOM::Justification::keLeft)
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keLeft));

    bool bHasNumer = sNumer.Length() > 0;
    bool bHasDenom = sDenom.Length() > 0;

    //
    //  Process the "numerator"
    //
    if(bHasNumer) {
        // Full stack (not fake superscript) so add some additional info.
        if(bHasDenom) {
            Run.Location().AddSemantic(ATOM::ILocation::keInlineBlock);
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // We have to come back to this place... remember it.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));
        }

        // Indicate that this is the "superscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSuperscript);

        Run.Contents() = sNumer;

        // This location stuff is subject to some fiddling (part 1)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        // Send the Numerator along
        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of superscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSuperscript);
        // This location stuff is subject to some fiddling (part 2)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));
    }

    //
    //  Process the "denominator"
    //
    if(bHasDenom) {
        // Full stack (not fake subscript) so add some additional info.
        if(bHasNumer) {
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // Remember how far out we came, because we need to come back here.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX));
            // Now, back to the left edge in preparation for the denom.
            Run.Location().AddOperation(ATOM::ReturnToBookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));
        }

        // Indicate that this is the "subscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSubscript);

        Run.Contents() = sDenom;

        // This location stuff is subject to some fiddling (part 3)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));

        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of subscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSubscript);
        // This location stuff is subject to some fiddling (part 4)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        if(bHasNumer) {
            // No longer in the "stack" region
            Run.Location().AddSemantic(ATOM::ILocation::keEndInlineBlock);
            // Go as far as the
            Run.Location().AddOperation(ATOM::ConditionalReturnToBookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX,
                                                                                      ATOM::ConditionalReturnToBookmarkLocationParticle::keFarthestAdvance ));
        }
    }

    if(eOldJustification != ATOM::Justification::keLeft)
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(eOldJustification));

    m_sHere.SetStart(sDenom.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}


// TO DO? Factor out from below?
ATOM::Status MTextParseInstance::Parse_S_Decimal_Part(TextRunElement& /*Run*/, ATOM::StRange /*sWhole*/, ATOM::StRange /*sDecimal*/)
{
    return ATOM::Status::keNotImplemented;
}

//
//  This produces a decimal-aligned stack.     +99.09
//                                            +101.10
//
ATOM::Status MTextParseInstance::Parse_S_Decimal(TextRunElement& Run,ATOM::StRange sNumerDeci,ATOM::StRange sDenomDeci,ATOM::CHARTYPE chDecimal)
{
    // We attack the problem by dividing the numerator and denominator into
    // whole and decimal parts, respectively.  These four pieces will be presented
    // in a 2x2 table, where the whole parts are right justified and the decimal
    // are left justified.  In most other respects, this follows ordinary tolerance stacks.

    // We remember what that (outside) justification is.
    ATOM::JustificationStyleParticle* pOldJustification = (ATOM::JustificationStyleParticle*)(Run.Style().GetDescriptionParticle(ATOM::StyleParticle::keJustification));
    ATOM::Justification::Type eOldJustification = pOldJustification != NULL ?
                                                  pOldJustification->Value() :
                                                  ATOM::Justification::keLeft;

    bool bHasNumer = sNumerDeci.Length() > 0;
    bool bHasDenom = sDenomDeci.Length() > 0;

    //
    //  Process the "numerator"
    //
    if(bHasNumer) {
        // Full stack (not fake superscript) so add some additional info.
        if(bHasDenom) {
            Run.Location().AddSemantic(ATOM::ILocation::keTable);
            Run.Location().AddSemantic(ATOM::ILocation::keInlineBlock);
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // We have to come back to this place... remember it.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));

            // Whole number part is right-justified
            Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keRight));
        }

        // Indicate that this is the "superscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSuperscript);

        ATOM::StRange sNumerWhole = sNumerDeci.Split(chDecimal); // sNumerDeci reduced to what follows decimal point.
        sNumerDeci.MoveStart(-1); // back up, to include the point.

        Run.Contents() = sNumerWhole;

        // This location stuff is subject to some fiddling (part 1)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        // Send the Numerator Whole along
        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        if(bHasDenom) {
            // Okay, we move on to the next item.
            Run.Location().AddSemantic(ATOM::ILocation::keCell);
        }

        // The decimal part is left-justified
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keLeft));

        Run.Contents() = sNumerDeci;

        // Send the Numerator decimal along
        eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of superscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSuperscript);
        // This location stuff is subject to some fiddling (part 2)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));
    }

    //
    //  Process the "denominator"
    //
    if(bHasDenom) {
        // Full stack (not fake subscript) so add some additional info.
        if(bHasNumer) {
            Run.Location().AddSemantic(ATOM::ILocation::keRow);
            Run.Location().AddSemantic(ATOM::ILocation::keCell);

            // Remember how far out we came, because we need to come back here.
            Run.Location().AddOperation(ATOM::BookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX));
            // Now, back to the left edge in preparation for the denom.
            Run.Location().AddOperation(ATOM::ReturnToBookmarkLocationParticle(STACK_LEFT_BOOKMARK_INDEX));
            // Whole number part is right-justified
            Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keRight));
        }

        // Indicate that this is the "subscript" part of the stack
        Run.Location().AddSemantic(ATOM::ILocation::keSubscript);

        ATOM::StRange sDenomWhole = sDenomDeci.Split(chDecimal); // sDenomDeci reduced ...
        sDenomDeci.MoveStart(-1); // back up, to include the point.

        Run.Contents() = sDenomWhole;

        // This location stuff is subject to some fiddling (part 3)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(-0.5,ATOM::Measure::keEm,NULL) ));

        // Send the Denominator Whole along
        ATOM::Status eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        if(bHasDenom) {
            // Okay, we move on to the next item.
            Run.Location().AddSemantic(ATOM::ILocation::keCell);
        }

        // The decimal part is left-justified
        Run.Style().AddDelta(ATOM::JustificationStyleParticle(ATOM::Justification::keLeft));

        Run.Contents() = sDenomDeci;

        // Send the Numerator decimal along
        eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        //
        // Prepare for what follows
        //

        // End of subscript
        Run.Location().AddSemantic(ATOM::ILocation::keEndSubscript);

        // This location stuff is subject to some fiddling (part 4)
        Run.Location().AddOperation(ATOM::RelativeLocationParticle(ATOM::Measure(),
                                                                   ATOM::Measure(0.5,ATOM::Measure::keEm,NULL) ));

        if(bHasNumer) {
            // No longer in the "stack" region
            Run.Location().AddSemantic(ATOM::ILocation::keEndInlineBlock);
            Run.Location().AddSemantic(ATOM::ILocation::keEndTable);
            // Go as far as the right edge, unless we're farther.
            Run.Location().AddOperation(ATOM::ConditionalReturnToBookmarkLocationParticle(STACK_RIGHT_BOOKMARK_INDEX,
                                                                                      ATOM::ConditionalReturnToBookmarkLocationParticle::keFarthestAdvance ));
        }
    }

    Run.Style().AddDelta(ATOM::JustificationStyleParticle(eOldJustification));

    m_sHere.SetStart(sDenomDeci.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}


// Implements the \Snumer/denom; command. (stack)
// Formally, this may be:                                 1
//   \Snumer/denom;  -- traditional over/under fraction: ---
//   \Snumer#denom;  -- vulgar fraction  1/2              2
//   \Snumer^denom;  -- tolerance, numbers left-aligned
//   \Snumer~.denom; -- decimal-aligned stack, where . represents the decimal char
ATOM::Status MTextParseInstance::Parse_S(TextRunElement& Run)
{
    m_sHere.Move(1);

    ATOM::StRange parm = m_sHere;
    if(!ParseForParameter(parm).Succeeded())
        return Abandon(ATOM::Status::keIncompleteString,parm);

    // Okay, we know we've got a reasonably-well formed stack.
    // Let's flush what's before it.
    ATOM::Status eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    // Mark a recursive block surrounding the stack.

    // Now, with a clean slate, let's move forward:
    ATOM::StRange numer;
    const ATOM::CHARTYPE* pSep = NULL;
    ATOM::StRange denom;

    for(int i=0; i<parm.Length(); ++i)
    {
        switch(parm[i]) {
            case '/':
            case '#':
            case '^':
                numer.Set(parm.Start(),i);
                pSep = parm.Start()+i;
                denom = parm.Part(i+1);
                break;

            case '~':
                numer.Set(parm.Start(),i);
                pSep = parm.Start()+i;
                denom = parm.Part(i+2);
                break;
        }
        if(pSep != NULL)
            break;
    }

    switch(*pSep) {
        case '/': return Parse_S_OverUnder(Run,numer,denom);
        case '#': return Parse_S_Vulgar   (Run,numer,denom);
        case '^': return Parse_S_Tolerance(Run,numer,denom);
        case '~': return Parse_S_Decimal  (Run,numer,denom,pSep[1]);
        default: return Abandon(ATOM::Status::keInternalError,ATOM::StRange(pSep,2));
    }
}


ATOM::Status MTextParseInstance::Parse_T(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nTrack;
    if(!GetNumber(parm,nTrack).Succeeded())
       return Abandon(ATOM::Status::keIncompleteString,parm);

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    double dTrack = (double)nTrack;
    // Out of bounds?
    if(dTrack < 0.75 || dTrack > 4.0 )
        return Abandon(ATOM::Status::keInvalidArg,parm);

    Run.Style().AddDelta(ATOM::TrackingAugmentStyleParticle(ATOM::Measure(nTrack,ATOM::Measure::keProportion,&parm)));

    return ATOM::Status::keOk;
}

int MTextParseInstance::HexChar(ATOM::CHARTYPE ch)
{
    switch(ch) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return ch - '0';
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return ch - 'a' + 10;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return ch - 'A' + 10;
    }

    return -1;
}

// Implements the \U+xxxx command. (Unicode codepoint)
ATOM::Status MTextParseInstance::Parse_U(TextRunElement& Run)
{
    m_sHere.Move(1);

    ATOM::StRange parm(m_sHere.Start(),5);

    // Future: implement a variant based on single-octet character type.
    // (presumably UTF-8 encoding)
#pragma warning(push)
#pragma warning(disable: 4127)
    if(sizeof(ATOM::CHARTYPE) == 1)
        return Abandon(ATOM::Status::keNotSupported,parm);
#pragma warning(pop)

    if(parm[0] != '+')
        return Abandon(ATOM::Status::keUnexpectedCharacter,parm);

    int hd,iCodePoint=0;
    for(int i=1; i<5; ++i)
    {
        hd = HexChar(parm[i]);
        if(hd < 0)
            return Abandon(ATOM::Status::keInvalidArg,parm);
        iCodePoint <<= 4;
        iCodePoint += hd;
    }
    ATOM::CHARTYPE sz[2];
    sz[0] = (ATOM::CHARTYPE)iCodePoint;
    sz[1] = 0;

    // Flush what's not been sent yet
    ATOM::Status eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    // Send the unicode character
    Run.Contents() = sz;
    eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    // Now, get ready to continue parsing.
    m_sHere.SetStart(parm.End());

    return ATOM::Status::keOk;
}

ATOM::Status MTextParseInstance::Parse_W(TextRunElement& Run)
{
    m_sHere.Move(1);
    ATOM::StRange parm = m_sHere;
    ATOM::NUMBER nWid;
    if(!GetNumber(parm,nWid).Succeeded())
        return Abandon(ATOM::Status::keIncompleteString,parm);

    if(nWid == 1.0)
        Run.Transform().RemoveSameTypeTransform(ATOM::ScaleTransformParticle(nWid,1.0));
    else
        Run.Transform().ReplaceTransform(ATOM::ScaleTransformParticle(nWid,1.0));

    m_sHere.SetStart(parm.Beyond()); // account for the trailing semicolon

    return ATOM::Status::keOk;
}

// Processes %< ... >% insertion, or complains if failed.
ATOM::Status MTextParseInstance::ParseFieldInsertion(TextRunElement& Run)
{
    ATOM::StRange sField = m_sHere;

    // Read forward looking for a semicolon, which is the
    // marker for end of parameter.
    for ( ; ; ) {
        // Make sure it's a complete reference; if we're over the buffer's EOS
        // then we've run out of characters and thus need to abandon.
        if(!sField.Last())
            return Abandon(ATOM::Status::keUnmatchedConstruct,m_sHere.Part(0,2)/*ATOM::StRange(m_sHere.Start(),2)*/);

        // Okay, do we have the last two characters?
        if(sField.Last() == '%' && sField.Last(2) == '>')
            break;

        // No news; let's expand our look by one.
        sField.AddLength(1);
    }

    // We now have sField covering the full %< ... >% markup.
    // Let's break it down...

    // First, we need to send the unprocessed text run preceding the field:
    ATOM::Status eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    // Delete the markup from around the field.
    sField.MoveStart(2);  // Take two off the front for %<
    sField.AddLength(-2); // and  two off the end for >%

    // Now, ask the Environment to expand this field.
    ATOM::StRange sResolution;
    ATOM::Status eStatus = m_pEnv->References()->Resolve(MTEXT_PARSER_NAME,
                                                         sField,
                                                         sResolution,m_pEnv);

    // If the resolver figured it out, let's put what it figured out.
    // Otherwise, let's pass the field through unchanged.
    if(eStatus.Result() == ATOM::Status::keUnchanged || !eStatus.Succeeded())
        Run.Style().AddDelta(ATOM::ReferenceExpansionStyleParticle(ATOM::ReferenceExpansion::keSource));   // the original is there
    else
        Run.Style().AddDelta(ATOM::ReferenceExpansionStyleParticle(ATOM::ReferenceExpansion::keExpanded)); // it's been swapped.

    // Let's push through the reference, expanded or not.
    Run.Contents() = eStatus.Succeeded()? sResolution : sField;
    eRet = SendTextRunNotification(Run);
    if(!eRet.Succeeded())
        return eRet;

    // Finally, let's reset for normal operation.
    Run.Style().AddDelta(ATOM::ReferenceExpansionStyleParticle(ATOM::ReferenceExpansion::keNotReference));
    m_sHere.Set(sField.Beyond(1),1); // Get past the >% in the markup.

    return ATOM::Status::keOk;
}


// Handles all markup that isn't prefixed with a backslash
// Notably: nesting constructs { and }
//          newline character (immediate line break)
//          %% metacharacters
//          %< ... >% expansion notation
ATOM::Status MTextParseInstance::Parse_NonBackslash(TextRunElement& Run)
{
    ATOM::Status eRet;
    if(m_sHere[0] == '{') {
        eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        // Enter nested context...
        m_sHere.Move(1);
        eRet = ParseContext(&Run);
        if(!eRet.Succeeded())
            return eRet;
    }
    else if(m_sHere[0] == '}') {
        // Take care of unfinished business inside of this context
        eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        if(Run.Structure()->Depth() == 0)
            return Abandon(ATOM::Status::keUnexpectedCharacter,m_sHere);

        // Special secret handshake to tell the outside
        // world that we're Okay, but we're exiting a nested
        // construct.
        return ATOM::Status::keDone;
    }
    else if(m_sHere[0] == '\n') {
        eRet = SendTextRunNotification(Run);
        if(!eRet.Succeeded())
            return eRet;

        Run.Location().SetSemantics(ATOM::ILocation::keLine);
        Run.Location().AddOperation(ATOM::LineBreakLocationParticle());
    }
    else if(m_sHere[0] == '%' && *m_sHere.Beyond() == '%') {  // %%C %%D or %%P metachars?
        switch(m_sHere.Start()[2]) {
            case 'C':  // Diameter
            case 'c':
                eRet = SendMetacharacter(Run,/*MSG0*/L"\x00D8"); // Diameter symbol
                if(!eRet.Succeeded())
                    return eRet;
                m_sHere.Move(2);
                break;

            case 'D':  // Degree
            case 'd':
                eRet = SendMetacharacter(Run,/*MSG0*/L"\x00b0"); // Degree symbol
                if(!eRet.Succeeded())
                    return eRet;
                m_sHere.Move(2);
                break;

            case 'P': // Plus/Minus
            case 'p':
                eRet = SendMetacharacter(Run,/*MSG0*/L"\x00b1"); // Plus/Minus symbol
                if(!eRet.Succeeded())
                    return eRet;
                m_sHere.Move(2);
                break;

            default: // nothing... just pass through the % sign.
                UpdateContentsPointer(Run);
        }
    }
    else if(m_sHere[0] == '%' && *m_sHere.Beyond() == '<') {  // %< ... >%?
        eRet = ParseFieldInsertion(Run);
        if(!eRet.Succeeded())
            return eRet;
    }
    else {
        UpdateContentsPointer(Run);
    }

    return ATOM::Status::keOk;
}


// Main workhorse.  Reentrant to handle nesting constructs { and }
ATOM::Status MTextParseInstance::ParseContext(TextRunElement* pOuter)
{
    ATOM::Status eRet = ATOM::Status::keOk;

    TextRunElement Run;

    if(pOuter != NULL)
        Run.InitFrom(pOuter);
    else {
        Run.InitFrom(m_pEnv);
        // Now, remove stuff from the style that might be confusing/contradictory.
        Run.Style().RemoveFromDescription(ATOM::StyleParticle::keSize);
    }

    const ATOM::CHARTYPE* pEntryPosition = m_sHere.Start();  // for diagnostics, if necessary.

    while(m_sHere[0]) {
        if(m_sHere[0] == '\\') { // Transition to Markup
            eRet = SendTextRunNotification(Run);
            if(!eRet.Succeeded())
                return eRet;

            // Advance past the slash to the opcode.
            m_sHere.Move(1);

            switch(m_sHere[0]) {
            case '\0':
                // End of string in the middle of a backslash sequence?  That ain't good.
                return Abandon(ATOM::Status::keIncompleteString,ATOM::StRange(m_sHere.Start()-1,1));

            case 'A':  // \A#; -- Vertical alignment. # = 0, 1, 2
                eRet = Parse_A(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'C':  // \C##;  -- ACI color
                eRet = Parse_C(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'c':  // \c###; -- RGB color (in decimal) (R + G<<8 + B<<16)
                eRet = Parse_c(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'f':  //  \fArial|b0|i0|p34|c0; -- Font change: typeface,bold,italic,pitchfam,charset
                eRet = Parse_f(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'H':  // either \H###; or \H###x; the latter being a relative scale: \H2x; => twice size.
                eRet = Parse_H(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'L': // \L (no extra syntax) -- begin Underline.
                eRet = Parse_L(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'l': // \l (no extra syntax) -- end Underline.
                eRet = Parse_l(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'N': // \N (no extra syntax) -- Next Column
                eRet = Parse_N(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'O': // \O (no extra syntax) -- begin Overline.
                eRet = Parse_O(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'o': // \o (no extra syntax) -- end Overline.
                eRet = Parse_o(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'P': // \P (no extra syntax) -- end Paragraph.
                Run.Location().SetSemantics(ATOM::ILocation::keParagraph);
                // Go to the next line.
                Run.Location().AddOperation(ATOM::LineBreakLocationParticle());

                break;


            case 'Q': // \Q##; -- Set obliquing angle
                eRet = Parse_Q(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'S': // \Snum/denom;  (where / could be ^ or %, too)  -- Stack fraction.
                eRet = Parse_S(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'T': // \T##; -- Tracking factor.
                eRet = Parse_T(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'U': // \U+xxxx -- Unicode codepoint.
                eRet = Parse_U(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case 'W': // \W##; -- Width factor: horizontally stretch text by multiplier given.
                eRet = Parse_W(Run);
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case '~': // \~ -- Non-breaking space
                eRet = SendMetacharacter(Run,/*MSG0*/L"\x00a0"); // non-breaking space
                if(!eRet.Succeeded())
                    return eRet;
                break;

            case '\\':
            case '{':
            case '}':
                // The escaped character, \, {, or }, is what we
                // send (that is, we omit the esaping \ in the
                // string.
                eRet = SendMetacharacter(Run,m_sHere);
                if(!eRet.Succeeded())
                    return eRet;

                break;
            }
        }
        else {
            eRet = Parse_NonBackslash(Run);

            // Our queue to exit from a nested context:
            if(eRet == ATOM::Status::keDone)
                return ATOM::Status::keOk;

            if(!eRet.Succeeded())
                return eRet;
        }
        // Advance to the next character.
        m_sHere.Move(1);
    }

    // Process whatever's left over...
    eRet = SendTextRunNotification(Run);

    // Have we prematurely reached the end of string?
    if(Run.Structure()->Depth() != 0)
        return Abandon(ATOM::Status::keUnmatchedConstruct,ATOM::StRange(pEntryPosition-1,1));

    return eRet;

}



ATOM::Status MTextParseInstance::ParseForParameter(ATOM::StRange& sParam)
{
    // Read forward looking for a semicolon, which is the
    // marker for end of parameter.
    while(sParam.Last() && sParam.Last() != ';') {
        // Looking for runaway command: do we find another
        // backslash inside?
        if(sParam.Last() == '\\') {
            sParam.AddLength(1);
            // It's only a problem if the following character
            // isn't also a backslash (that is, escaped backslash)
            if(sParam.Last(1) != '\\') {
                sParam.AddLength(-1);
                break;
            }
        }
        sParam.AddLength(1);
    }

    // if we're out of the loop and we're not over
    // a semicolon, then back off to not include that
    // unexpected character, and indicate that we think
    // the parameter isn't there.
    if(sParam.Last() != ';') {
        sParam.AddLength(-1);
        return ATOM::Status::keUnexpectedCharacter;
    }

    // Come to think of it, we shouldn't include the semicolon, either.
    sParam.AddLength(-1);
    return ATOM::Status::keOk;

}

// Gets an ATOM::NUMBER from the parameter string indicated.
ATOM::Status MTextParseInstance::GetNumber(ATOM::StRange& sNumber,ATOM::NUMBER& nNumber)
{
    if(ParseForParameter(sNumber).Succeeded()) {
        nNumber = (ATOM::NUMBER) _wtof(sNumber.Start());
        return ATOM::Status::keOk;
    }
    return ATOM::Status::keNotPresent;
}

// Does the dirty work of talking TextRun to the sink.
ATOM::Status MTextParseInstance::SendTextRunNotification(TextRunElement& Run)
{
    if(!Run.IsReset()) {
        ATOM::Status eRet = m_pEnv->Sink()->TextRun(&Run,m_pEnv);
        if(!eRet.Succeeded())
            eRet = Abandon(eRet.Result(),Run.Contents());
        Run.Reset();
        return eRet;
    }
    return ATOM::Status::keOk;
}


// Does the handiwork of dispatching an Abandon notification.
ATOM::Status MTextParseInstance::Abandon(ATOM::Status::StatusType eReason,const ATOM::StRange sPos)
{
    AbandonmentElement a(eReason);
    a.SetMarkup(m_sMarkup);

    // Let's backtrack to find the start of the line (or buffer)
    const ATOM::CHARTYPE* pLineStart = sPos.Start();
    while(pLineStart > m_sMarkup.Start()) {
        if(*pLineStart == '\n') {
            pLineStart++; // get back on our side of the line.
            break;
        }
        /* Reinstate when/if \P support is requested.
        else if(*pLineStart == '\\' && pLineStart[1] == 'P') {
            pLineStart+=2;
            break;
        }
        */
        pLineStart--;
    }

    ATOM::StRange sContext(pLineStart,sPos.End());
    // And go forward to find end of line (or buffer)
    while(sContext.Last() != '\0' && sContext.Last() != '\n')
        sContext.AddLength(1);
    sContext.AddLength(-1);

    a.SetContext(sContext);
    a.SetPosition(sPos);
    return m_pEnv->Sink()->Abandon(&a,m_pEnv);
}




int MTextParseInstance::RgbToAci(ATOM::Color rgb)
{
    long l = rgb.LongARGB();
    if(!(l & ATOM_COLOR_A_BITS)) // no alpha?
        l |= ATOM_COLOR_A_BITS; // force alpha to maximum.

    for(int i=0; i<sizeof(sm_lAciColorTable)/sizeof(long); ++i)
    {
        if(l == sm_lAciColorTable[i])
            return i;
    }
    return -1; // not found.
}

// Just minimizes cartesian distance (squared) to
int MTextParseInstance::RgbToNearestAci(ATOM::Color rgb)
{
    double dDistSquaredToNearest = 1e308; //huge;
    int iIndexNearest = -1;

    for(int i=0; i<sizeof(sm_lAciColorTable)/sizeof(long); ++i)
    {
        long lAci = sm_lAciColorTable[i];
        long lRgb = rgb.LongARGB();

        // B
        double dDist  = ((lAci & 0xFF)-(lRgb & 0xFF))*((lAci & 0xFF)-(lRgb & 0xFF));

        // G
        lAci >>= 8; lRgb >>=8;
        dDist += ((lAci & 0xFF)-(lRgb & 0xFF))*((lAci & 0xFF)-(lRgb & 0xFF));

        // R
        lAci >>= 8; lRgb >>=8;
        dDist += ((lAci & 0xFF)-(lRgb & 0xFF))*((lAci & 0xFF)-(lRgb & 0xFF));

        // A
        lAci >>= 8; lRgb >>=8;
        dDist += ((lAci & 0xFF)-(lRgb & 0xFF))*((lAci & 0xFF)-(lRgb & 0xFF));

        if(dDist < dDistSquaredToNearest) {
            dDistSquaredToNearest = dDist;
            iIndexNearest = i;
            // Hmm, exact match; don't get any closer than this.
            if(dDist == 0)
                break;
        }
    }

    return iIndexNearest; // not found.
}


// generated using data from http://bitsy.sub-atomic.com/~moses/acadcolors.html
long MTextParseInstance::sm_lAciColorTable[256] = {
  //aaRRGGBB  <- note that it's RGB, not BGR.
  0xff000000, // 0
  0xffFF0000, // 1
  0xffFFFF00, // 2
  0xff00FF00, // 3
  0xff00FFFF, // 4
  0xff0000FF, // 5
  0xffFF00FF, // 6
  0xffFFFFFF, // 7
  0xff414141, // 8
  0xff808080, // 9
  0xffFF0000, // 10
  0xffFFAAAA, // 11
  0xffBD0000, // 12
  0xffBD7E7E, // 13
  0xff810000, // 14
  0xff815656, // 15
  0xff680000, // 16
  0xff684545, // 17
  0xff4F0000, // 18
  0xff4F3535, // 19
  0xffFF3F00, // 20
  0xffFFBFAA, // 21
  0xffBD2E00, // 22
  0xffBD8D7E, // 23
  0xff811F00, // 24
  0xff816056, // 25
  0xff681900, // 26
  0xff684E45, // 27
  0xff4F1300, // 28
  0xff4F3B35, // 29
  0xffFF7F00, // 30
  0xffFFD4AA, // 31
  0xffBD5E00, // 32
  0xffBD9D7E, // 33
  0xff814000, // 34
  0xff816B56, // 35
  0xff683400, // 36
  0xff685645, // 37
  0xff4F2700, // 38
  0xff4F4235, // 39
  0xffFFBF00, // 40
  0xffFFEAAA, // 41
  0xffBD8D00, // 42
  0xffBDAD7E, // 43
  0xff816000, // 44
  0xff817656, // 45
  0xff684E00, // 46
  0xff685F45, // 47
  0xff4F3B00, // 48
  0xff4F4935, // 49
  0xffFFFF00, // 50
  0xffFFFFAA, // 51
  0xffBDBD00, // 52
  0xffBDBD7E, // 53
  0xff818100, // 54
  0xff818156, // 55
  0xff686800, // 56
  0xff686845, // 57
  0xff4F4F00, // 58
  0xff4F4F35, // 59
  0xffBFFF00, // 60
  0xffEAFFAA, // 61
  0xff8DBD00, // 62
  0xffADBD7E, // 63
  0xff608100, // 64
  0xff768156, // 65
  0xff4E6800, // 66
  0xff5F6845, // 67
  0xff3B4F00, // 68
  0xff494F35, // 69
  0xff7FFF00, // 70
  0xffD4FFAA, // 71
  0xff5EBD00, // 72
  0xff9DBD7E, // 73
  0xff408100, // 74
  0xff6B8156, // 75
  0xff346800, // 76
  0xff566845, // 77
  0xff274F00, // 78
  0xff424F35, // 79
  0xff3FFF00, // 80
  0xffBFFFAA, // 81
  0xff2EBD00, // 82
  0xff8DBD7E, // 83
  0xff1F8100, // 84
  0xff608156, // 85
  0xff196800, // 86
  0xff4E6845, // 87
  0xff134F00, // 88
  0xff3B4F35, // 89
  0xff00FF00, // 90
  0xffAAFFAA, // 91
  0xff00BD00, // 92
  0xff7EBD7E, // 93
  0xff008100, // 94
  0xff568156, // 95
  0xff006800, // 96
  0xff456845, // 97
  0xff004F00, // 98
  0xff354F35, // 99
  0xff00FF3F, // 100
  0xffAAFFBF, // 101
  0xff00BD2E, // 102
  0xff7EBD8D, // 103
  0xff00811F, // 104
  0xff568160, // 105
  0xff006819, // 106
  0xff45684E, // 107
  0xff004F13, // 108
  0xff354F3B, // 109
  0xff00FF7F, // 110
  0xffAAFFD4, // 111
  0xff00BD5E, // 112
  0xff7EBD9D, // 113
  0xff008140, // 114
  0xff56816B, // 115
  0xff006834, // 116
  0xff456856, // 117
  0xff004F27, // 118
  0xff354F42, // 119
  0xff00FFBF, // 120
  0xffAAFFEA, // 121
  0xff00BD8D, // 122
  0xff7EBDAD, // 123
  0xff008160, // 124
  0xff568176, // 125
  0xff00684E, // 126
  0xff45685F, // 127
  0xff004F3B, // 128
  0xff354F49, // 129
  0xff00FFFF, // 130
  0xffAAFFFF, // 131
  0xff00BDBD, // 132
  0xff7EBDBD, // 133
  0xff008181, // 134
  0xff568181, // 135
  0xff006868, // 136
  0xff456868, // 137
  0xff004F4F, // 138
  0xff354F4F, // 139
  0xff00BFFF, // 140
  0xffAAEAFF, // 141
  0xff008DBD, // 142
  0xff7EADBD, // 143
  0xff006081, // 144
  0xff567681, // 145
  0xff004E68, // 146
  0xff455F68, // 147
  0xff003B4F, // 148
  0xff35494F, // 149
  0xff007FFF, // 150
  0xffAAD4FF, // 151
  0xff005EBD, // 152
  0xff7E9DBD, // 153
  0xff004081, // 154
  0xff566B81, // 155
  0xff003468, // 156
  0xff455668, // 157
  0xff00274F, // 158
  0xff35424F, // 159
  0xff003FFF, // 160
  0xffAABFFF, // 161
  0xff002EBD, // 162
  0xff7E8DBD, // 163
  0xff001F81, // 164
  0xff566081, // 165
  0xff001968, // 166
  0xff454E68, // 167
  0xff00134F, // 168
  0xff353B4F, // 169
  0xff0000FF, // 170
  0xffAAAAFF, // 171
  0xff0000BD, // 172
  0xff7E7EBD, // 173
  0xff000081, // 174
  0xff565681, // 175
  0xff000068, // 176
  0xff454568, // 177
  0xff00004F, // 178
  0xff35354F, // 179
  0xff3F00FF, // 180
  0xffBFAAFF, // 181
  0xff2E00BD, // 182
  0xff8D7EBD, // 183
  0xff1F0081, // 184
  0xff605681, // 185
  0xff190068, // 186
  0xff4E4568, // 187
  0xff13004F, // 188
  0xff3B354F, // 189
  0xff7F00FF, // 190
  0xffD4AAFF, // 191
  0xff5E00BD, // 192
  0xff9D7EBD, // 193
  0xff400081, // 194
  0xff6B5681, // 195
  0xff340068, // 196
  0xff564568, // 197
  0xff27004F, // 198
  0xff42354F, // 199
  0xffBF00FF, // 200
  0xffEAAAFF, // 201
  0xff8D00BD, // 202
  0xffAD7EBD, // 203
  0xff600081, // 204
  0xff765681, // 205
  0xff4E0068, // 206
  0xff5F4568, // 207
  0xff3B004F, // 208
  0xff49354F, // 209
  0xffFF00FF, // 210
  0xffFFAAFF, // 211
  0xffBD00BD, // 212
  0xffBD7EBD, // 213
  0xff810081, // 214
  0xff815681, // 215
  0xff680068, // 216
  0xff684568, // 217
  0xff4F004F, // 218
  0xff4F354F, // 219
  0xffFF00BF, // 220
  0xffFFAAEA, // 221
  0xffBD008D, // 222
  0xffBD7EAD, // 223
  0xff810060, // 224
  0xff815676, // 225
  0xff68004E, // 226
  0xff68455F, // 227
  0xff4F003B, // 228
  0xff4F3549, // 229
  0xffFF007F, // 230
  0xffFFAAD4, // 231
  0xffBD005E, // 232
  0xffBD7E9D, // 233
  0xff810040, // 234
  0xff81566B, // 235
  0xff680034, // 236
  0xff684556, // 237
  0xff4F0027, // 238
  0xff4F3542, // 239
  0xffFF003F, // 240
  0xffFFAABF, // 241
  0xffBD002E, // 242
  0xffBD7E8D, // 243
  0xff81001F, // 244
  0xff815660, // 245
  0xff680019, // 246
  0xff68454E, // 247
  0xff4F0013, // 248
  0xff4F353B, // 249
  0xff333333, // 250
  0xff505050, // 251
  0xff696969, // 252
  0xff828282, // 253
  0xffBEBEBE, // 254
  0xffFFFFFF, // 255
};


/**********************************************************************
 *
 *  PARSER GENERATOR and its singleton instance.
 *
 **********************************************************************/

MTextGenerator::MTextGenerator()
{
    ATOM::BigBang()->Register(this);
}

MTextGenerator::~MTextGenerator()
{
    ATOM::BigBang()->Unregister(this);
}


// The name of the markup this parser represents
// such as "SVG" or "RTF" or ...
const ATOM::StRange MTextGenerator::Name() const
{
    return MTEXT_PARSER_NAME; // this is an internal string, not subject to localization.
}

// Documentation of the parser/generator (for
// version reporting, etc.)  A human-readable string.
const ATOM::StRange MTextGenerator::Description() const
{
    return L"MTEXT ATOM::IParser v1.0";
}

// Creates an instance to a new Parser
ATOM::Status MTextGenerator::Create(ATOM::IParser** ppNewParser)
{
    if(ppNewParser == NULL)
        return ATOM::Status::keInvalidArg;

    *ppNewParser = new MTextParser(this);

    return ATOM::Status::keOk;
}

// Takes a pointer to an existing parser and destroys it.
ATOM::Status MTextGenerator::Destroy(ATOM::IParser* pOldParser)
{
    if(pOldParser == NULL)
        return ATOM::Status::keInvalidArg;

    // TO DO ... validate that it's one of ours?

    delete(pOldParser);
    return ATOM::Status::keOk;
}

bool MTextGenerator::HasSink() const
{
    return false;
}

// Creates an instance to a new Parser
ATOM::Status MTextGenerator::Create(ATOM::ISink** ppNewSink)
{
    if(ppNewSink == NULL)
        return ATOM::Status::keInvalidArg;

    // *ppNewSink = new MTextSink(this);
    *ppNewSink = NULL;

    return ATOM::Status::keNotImplemented;
}

// Takes a pointer to an existing parser and destroys it.
ATOM::Status MTextGenerator::Destroy(ATOM::ISink* pOldSink)
{
    if (pOldSink == NULL)
        return ATOM::Status::keInvalidArg;

    // TO DO ... validate that it's one of ours?

    delete pOldSink;
    return ATOM::Status::keOk;
}


// Instance the generator, which does
// all the self-registration with the ATOM::IUniverse.
MTextGenerator LongLiveMText;
// well, until it's dead.
