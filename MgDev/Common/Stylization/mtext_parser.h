//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _mtext_parser_h
#define _mtext_parser_h

#include "atom_element.h"

class MTextParseInstance
{
public:
    MTextParseInstance(const ATOM::StRange sMarkup,ATOM::IEnvironment* pEnv)
        : m_sMarkup(sMarkup)
        , m_pEnv(pEnv)
    {
    }

    ATOM::Status Parse();

private:
    // Constructor-provided values
    const ATOM::StRange m_sMarkup;
    ATOM::IEnvironment* m_pEnv;

    // Internally-used values.
    ATOM::StRange  m_sHere;    // Current location of parsing.

    static long sm_lAciColorTable[256];

private:
    // Methods used internally
    void UpdateContentsPointer(TextRunElement& Run,int iAdvance = 1);

    ATOM::Status ParseContext(/*ATOM::StRange& sStart,*/TextRunElement* pOuter);

    ATOM::Status Parse_A(TextRunElement& Run);
    ATOM::Status Parse_C(TextRunElement& Run);
    ATOM::Status Parse_c(TextRunElement& Run);
    ATOM::Status Parse_f(TextRunElement& Run);
    ATOM::Status Parse_H(TextRunElement& Run);
    ATOM::Status Parse_L(TextRunElement& Run);
    ATOM::Status Parse_l(TextRunElement& Run);
    ATOM::Status Parse_N(TextRunElement& Run);
    ATOM::Status Parse_O(TextRunElement& Run);
    ATOM::Status Parse_o(TextRunElement& Run);
    ATOM::Status Parse_P(TextRunElement& Run);
    ATOM::Status Parse_Q(TextRunElement& Run);
    ATOM::Status Parse_S(TextRunElement& Run);
    ATOM::Status Parse_T(TextRunElement& Run);
    ATOM::Status Parse_U(TextRunElement& Run);
    ATOM::Status Parse_W(TextRunElement& Run);

    ATOM::Status Parse_S_OverUnder(TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom);
    ATOM::Status Parse_S_Vulgar   (TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom);
    ATOM::Status Parse_S_Tolerance(TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom);
    ATOM::Status Parse_S_Decimal  (TextRunElement& Run,ATOM::StRange sNumer,ATOM::StRange sDenom,ATOM::CHARTYPE chDecimal);
    ATOM::Status Parse_S_Decimal_Part(TextRunElement& Run,ATOM::StRange sWhole,ATOM::StRange sDecimal);

    ATOM::Status ParseFieldInsertion(TextRunElement& Run);
    ATOM::Status Parse_NonBackslash(TextRunElement& Run);

    ATOM::Status GetNumber(ATOM::StRange& sString, ATOM::NUMBER& nNumber);

    ATOM::Status ParseForParameter(ATOM::StRange& sParam);

    ATOM::Status SendMetacharacter(TextRunElement& Run,const ATOM::StRange& sMeta);

    // scans for a character within the string indicated.
    //ATOM::StRange ScanFor(ATOM::CHARTYPE ch,ATOM::StRange& sString);

    // Does the handiwork of dispatching a TextRun notification.
    ATOM::Status SendTextRunNotification(TextRunElement& Run);

    // Does the handiwork of dispatching an Abandon notification.
    ATOM::Status Abandon(ATOM::Status::StatusType eReason,
                         const ATOM::StRange sPos);

    int HexChar(ATOM::CHARTYPE ch);

};


class MTextParser: public ATOM::IParser
{
public: // Implementations of the ATOM::IParser interface

    // Parse a markup string by creating an "environment" with all the
    // ambient settings, then combine that with a string to parse, and
    // give it to this method.
    //
    // Since MTEXT is a nested grammar, this just does boiler-plate administrative
    // work, and defers the actual parsing to ParseContext.
    ATOM::Status Parse(const ATOM::StRange sMarkup,ATOM::IEnvironment* pEnv);
};



// The MTEXT Parser just needs to instance one of these as
// a singleton in order to be automatically self registering
// and unregistering.
class MTextParserGenerator: public ATOM::IParserGenerator
{
public:
    // Self-registering ctor and dtor.
    MTextParserGenerator();
    ~MTextParserGenerator();

public:
    // The name of the markup this parser represents
    // such as "SVG" or "RTF" or ...
    const ATOM::StRange Name() const;

    // Documentation of the parser/generator (for 
    // version reporting, etc.)  A human-readable string.
    const ATOM::StRange Description() const;

    // Creates an instance to a new Parser
    ATOM::Status Create(ATOM::IParser**);

    // Takes a pointer to an existing parser and destroys it.
    ATOM::Status Destroy(ATOM::IParser*);

    bool HasSink() const;

    // Creates an instance to a new Parser
    ATOM::Status Create(ATOM::ISink**);

    // Takes a pointer to an existing parser and destroys it.
    ATOM::Status Destroy(ATOM::ISink*);
};

#endif//_mtext_parser_h


