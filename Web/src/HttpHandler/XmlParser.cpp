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

#include "OgcFramework.h"
#include "XmlParser.h"

////////////////////////////////////////////////////////////////////////////////

// Pushes the cursor to the next tag/nontag boundary
// (or the end of the string.)
// Returns the number of characters processed.
// (When added to pszInput, that's one short of the
// terminating character.)
int MgXmlNode::Advance(CPSZ pszInput) const
{
    // End of string?  Nowhere to advance
    if(*pszInput == '\0')
        return 0;

    // Ignoring the character we're over, start
    // scanning until we find a boundary (EOS, or element)
    int iPos = 1;
    for (;;)
    {
        CHARTYPE ch = pszInput[iPos];
        if(ch == '\0' || ch == '<' || ch== '>')
            break;

        iPos++;
    }

    return iPos;
}


// Pushes the cursor over a name, which can consist
// of anything but certain terminating characters.
// Returns the number of characters processed.
int MgXmlNode::AdvanceOverName(CPSZ pszInput) const
{
    int iPos = 0;
    for (;;)
    {
        CHARTYPE ch = pszInput[iPos];
        if(ch == '\0' || ch == '>' || ch == '=' || ch == '/' || ch == '?' || iswspace(ch))
            break;

        iPos++;
    }

    return iPos;
}


CPSZ MgXmlNode::AdvanceOverWhitespace(CPSZ psz)
{
    for (;;)
    {
        // Advance past white space
        if(iswspace(*psz))
            psz++;
        else
            break;
    }
    return psz;
}


int MgXmlNode::AdvanceOverQuote(CPSZ psz)
{
    CHARTYPE chQuote = *psz;
    int i=1;// don't count that opening quote.
    for (;;)
    {
        // Some safe-guarding.  Never should find these, really.
        // the > should be escaped, the latter is the sign of corruption.
        if(psz[i] == '>' || psz[0] == '\0')
            break;

        // Look for that matching quote
        if(psz[i] != chQuote)
            i++;
        else
            break;
    }
    return i;
}


// Just returns the raw contents of a node, as determined by
// each derivation's analysis.
STRING MgXmlNode::Contents() const
{
    return STRING(m_pszStart,m_iLen);
}

////////////////////////////////////////////////////////////////////////////////

MgXmlInvalid::MgXmlInvalid()
{
    m_pszStart = _("");
    m_iLen = 0;
}

////////////////////////////////////////////////////////////////////////////////


MgXmlDoctypeNode::MgXmlDoctypeNode(CPSZ pszString,xsize_t& iStartStop)
: m_bSystem(false)
, m_bPublic(false)
, m_pszDtd(NULL)
, m_iDtdLen(0)
, m_pszInternalSubset(NULL)
, m_iInternalSubsetLen(0)
{
    // Mark off the start of the <!DOCTYPE ...> declaration
    m_pszStart = pszString + iStartStop;

    // Now, find the <!DOCTYPE rootelement ... >
    m_pszRootElement    = AdvanceOverWhitespace(m_pszStart + /* len("<!DOCTYPE") */ 9);
    m_iRootElementLen   = AdvanceOverName(m_pszRootElement);

    // Look what's next: it can be SYSTEM, PUBLIC, or [...]
    CPSZ psz = AdvanceOverWhitespace(m_pszRootElement + m_iRootElementLen);
    if(     psz[0]=='S' && psz[1]=='Y' && psz[2]=='S' && psz[3]=='T' && psz[4]=='E' && psz[5]=='M') {
        // SYSTEM means we just expect a quoted string.
        m_bSystem = true;
        m_pszPublicIdentifier = NULL;
        m_iPublicIdentifierLen = 0;
        psz = AdvanceOverWhitespace(psz+6);
    }
    else if(psz[0]=='P' && psz[1]=='U' && psz[2]=='B' && psz[3]=='L' && psz[4]=='I' && psz[5]=='C') {
        // PUBLIC means it's followed with a quoted identifier
        m_bPublic = true;

        // Now find the public identifier, a quoted string
        psz = AdvanceOverWhitespace(psz+6);
        if(psz[0]=='"' || psz[0]== '\'') {
            m_iPublicIdentifierLen = AdvanceOverQuote(psz)-1;
            m_pszPublicIdentifier  = psz+1;

            psz = AdvanceOverWhitespace(psz + m_iPublicIdentifierLen + 2);
        }
    }

    // Now, if either PUBLIC or SYSTEM is encountered, we expect a string
    if((m_bPublic || m_bSystem) && (psz[0]=='"' || psz[0]== '\'')) {
        m_iDtdLen = AdvanceOverQuote(psz)-1;
        m_pszDtd = psz+1;

        psz = AdvanceOverWhitespace(psz + m_iDtdLen + 2);
    }

    // Now, regardless of what came before, we might have an internal subset,
    // a local bunch of DTD-like declarations that trump or augment what's declared
    // at the location indicated in the m_pszDtd string.
    if(psz[0]=='[') {
        m_pszInternalSubset = psz+1;
        psz = AdvanceToDoctypeEnd(m_pszInternalSubset);
        m_iInternalSubsetLen = (xsize_t) (psz - m_pszInternalSubset);
    }

    // Now, some administrative overhead: we need to find the end of
    // the DOCTYPE, make a note of the overall length, as well as tell
    // the parser how much we've consumed.
    psz = AdvanceOverWhitespace(psz);
    m_iLen = (xsize_t) (psz - m_pszStart);
    iStartStop += m_iLen;
    m_iLen++; // save a bunch of +1's and -1's elsewhere; include the ending '>'
}

//
STRING MgXmlDoctypeNode::RootElement()
{
    return STRING(m_pszRootElement,m_iRootElementLen);
}

bool MgXmlDoctypeNode::IsSystem()
{
    return m_bSystem;
}

bool MgXmlDoctypeNode::IsPublic(REFSTRING sIdentifier)
{
    if(m_bPublic) {
        sIdentifier = m_pszPublicIdentifier != NULL ? STRING(m_pszPublicIdentifier,m_iPublicIdentifierLen) : _("");// NOXLATE
        return true;
    }
    return false;
}

STRING MgXmlDoctypeNode::DTD()
{
    return m_pszDtd != NULL? STRING(m_pszDtd,m_iDtdLen) : _("");// NOXLATE
}

STRING MgXmlDoctypeNode::InternalSubset()
{
    return m_pszInternalSubset != NULL ? STRING(m_pszInternalSubset,m_iInternalSubsetLen) : _("");// NOXLATE
}

// Since we're being cheap and not parsing the InternalSubset, we just look for a close
// bracket that's not in a string or comment
CPSZ MgXmlDoctypeNode::AdvanceToDoctypeEnd(CPSZ psz)
{
    bool bInQuote = false; // Are we in a quoted mode (inside < ... >)?
    for(;;psz++) {
        switch(*psz) {
        case '\0':
            // This is bad, let's get out.
            return psz;

        case ']':
            // If we're not in some sort of a quote,
            // this is the end, get out!
            if(!bInQuote)
                return psz;
            break;
        case '<':
            // We'll treat any pseudo-XML as a quote.
            // This covers a multitude of sins, like
            // <!ENTITY>, <!ATTLIST> ..., PI's, and comments
            if(!bInQuote) {
                bInQuote = true;
            }
            break;

        case '>':
            if(bInQuote) {
                bInQuote = false;
            }

        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Represents a CDATA node, such as  <![CDATA[ ... ]]>
MgXmlCdataNode::MgXmlCdataNode(CPSZ pszString,xsize_t& iStartStop)
{
    // Mark off the start of the <![CDATA[ ...]]> declaration
    m_pszStart = pszString + iStartStop;

    CPSZ psz = AdvanceToCDataEnd(m_pszStart + /*len("<![CDATA[")*/ 9);
    m_iLen = (xsize_t) (psz - m_pszStart);
    iStartStop += m_iLen;
    m_iLen++;
}

STRING MgXmlCdataNode::Data()
{
    return STRING(m_pszStart+9,m_iLen -(9+/* len("]]>") */3));
}

CPSZ MgXmlCdataNode::AdvanceToCDataEnd(CPSZ psz)
{
    for(;;psz++) {
        if(psz[0] == '0')
            return psz; // Trouble!!!
        else if(psz[0] == ']' && psz[1] == ']' && psz[2] == '>')
            return psz+2; // point to that final '>'
    }
}

////////////////////////////////////////////////////////////////////////////////



// Iterates to the next attribute in the collection,
// or returns false if there is no other attribute.
bool MgXmlAttribute::Next()
{
    m_pszName = AdvanceOverWhitespace(m_pszStart);

    // End of the line?  Let's bail, since there's no next.
    if(*m_pszName == '>' || *m_pszName == '\0')
        return false;

    // We're now over a name; get its length
    m_iNameLen = AdvanceOverName(m_pszName);

    // Now we're at the equal sign, or some whitespace before it.
    m_pszValue = AdvanceOverWhitespace(m_pszName + m_iNameLen);
    // What?  No equal sign?  Not valid, so no more attributes.
    if(*m_pszValue != '=')
        return false;
    // Skip that equal sign.
    m_pszValue++;

    // Past the equal sign, there can be some space, or not.
    m_pszValue = AdvanceOverWhitespace(m_pszValue);
    // But after that, we expect to find either of the two legal quotes
    if(*m_pszValue != /*NOXLATE*/'"' && *m_pszValue != /*NOXLATE*/'\'')
        return false;

    m_iValueLen = AdvanceOverQuote(m_pszValue);

    m_pszValue++; // Advance past that open quote.

    // Ready for the next Next.  Next?
    m_pszStart = m_pszValue + m_iValueLen;

    // Now, we discounted the open quote above, gotta account
    // for that in the length, too.
    m_iValueLen--;

    return true;
}


STRING MgXmlAttribute::Name() const
{
    return STRING(m_pszName,m_iNameLen);
}


STRING MgXmlAttribute::Value() const
{
    return STRING(m_pszValue,m_iValueLen);
}


////////////////////////////////////////////////////////////////////////////////


bool MgXmlAttributedNode::HasAttributes()
{
    return m_pszStart[m_iEndOfName] != '>' && m_pszStart[m_iEndOfName] != '/';
}

MgXmlAttribute& MgXmlAttributedNode::Attributes()
{
    m_Attributes.m_pszStart = m_pszStart + m_iEndOfName;
    m_Attributes.m_iLen = m_iLen - m_iEndOfName - EndDecorationLen();
    return m_Attributes;
}


bool MgXmlAttributedNode::GetAttribute(CPSZ pszName,STRING& sValue)
{
    if(!HasAttributes())
        return false;

    MgXmlAttribute& Attr = Attributes();
    while(Attr.Next()) {
        STRING sName = Attr.Name();
        if(sName == pszName) {
            sValue = Attr.Value();
            return true;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////

MgXmlProcessingInstruction::MgXmlProcessingInstruction(CPSZ pszString,xsize_t& iStartStop)
{
    m_pszStart   = pszString + iStartStop;
    m_iEndOfName = AdvanceOverName(m_pszStart+2)+2;
    m_iLen       = m_iEndOfName;
    if(HasAttributes())
        m_iLen += Advance(m_pszStart + m_iEndOfName);

    iStartStop += m_iLen;

    m_iLen++;
}

STRING MgXmlProcessingInstruction::Name() const
{
    CPSZ pszStart = m_pszStart + 2; // past the opening brace and question mark
    return STRING(pszStart,m_iEndOfName - 2);
}

bool MgXmlProcessingInstruction::IsXml() const
{
    return m_pszStart[2] == 'x'
        && m_pszStart[3] == 'm'
        && m_pszStart[4] == 'l'
        && m_iEndOfName == 5;  // end of name occurs at 5th position: <?xml_
}

int MgXmlProcessingInstruction::EndDecorationLen()
{
    return 2; // the "?>" is not considered attribute
}


////////////////////////////////////////////////////////////////////////////////


MgXmlBeginElement::MgXmlBeginElement(CPSZ pszString,xsize_t& iStartStop)
{
    m_pszStart   = pszString + iStartStop;
    m_iEndOfName = AdvanceOverName(m_pszStart);
    m_iLen       = m_iEndOfName;
    // Attributes? Let's include them, too.
    if(HasAttributes())
        m_iLen  += Advance(m_pszStart + m_iEndOfName);
    else if(m_pszStart[m_iEndOfName] == '/') // slash immediately after end of name?
        m_iLen++; // count that slash in the length.

    iStartStop += m_iLen;

    m_iLen++; // advance to include the close angle bracket.
}


STRING MgXmlBeginElement::Name() const
{
    CPSZ pszStart = m_pszStart + 1; // past the opening brace
    return STRING(pszStart,m_iEndOfName - 1);
}


// Is it a <begin ... /> element? (don't expect matching </end>)
bool MgXmlBeginElement::IsEmpty() const
{
    // [m_iLen]   = char past >
    // [m_iLen-1] = >
    // [m_iLen-2] = char before >
    return m_pszStart[m_iLen-2] == '/';
}

int MgXmlBeginElement::EndDecorationLen()
{
    return 1;  // the ">" is not considered part of the attribute
}

////////////////////////////////////////////////////////////////////////////////


MgXmlEndElement::MgXmlEndElement(CPSZ pszString,xsize_t& iStartStop)
{
    m_pszStart = pszString + iStartStop;
    m_iLen     = Advance(m_pszStart);
    iStartStop += m_iLen;

    m_iLen++; // advance to include the close angle bracket.
}

STRING MgXmlEndElement::Name() const
{
    CPSZ pszStart = m_pszStart + 2; // past the opening brace and slash
    int iLen = AdvanceOverName(pszStart);
    return STRING(pszStart,iLen);
}


////////////////////////////////////////////////////////////////////////////////


MgXmlTextElement::MgXmlTextElement(CPSZ pszString,xsize_t& iStartStop)
{
    m_pszStart = pszString + iStartStop;
    m_iLen     = Advance(m_pszStart);
    iStartStop += m_iLen ? m_iLen - 1 : 0;
}


bool MgXmlTextElement::IsWhitespace() const
{
    for(xsize_t i=0;i<m_iLen;i++) {
        CPSZ psz = m_pszStart+i; psz;
        if(!iswspace(m_pszStart[i]))
            return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////

MgXmlComment::MgXmlComment(CPSZ pszString,xsize_t& iStartStop)
{
    m_pszStart = pszString + iStartStop;
    CPSZ psz = AdvanceToCommentEnd(m_pszStart + /*len("<!--")*/ 4);
    m_iLen = (xsize_t) (psz - m_pszStart);
    iStartStop += m_iLen;
    m_iLen++; // advance to include the close angle bracket.
}

STRING MgXmlComment::Text() const
{
    CPSZ pszStart = m_pszStart + 4;    // ignore the <!-- (four chars)
    return STRING(pszStart,m_iLen - 7);  // ignore the --> (three)
}

CPSZ MgXmlComment::AdvanceToCommentEnd(CPSZ psz)
{
    for(;;psz++) {
        if(psz[0] == '0')
            return psz; // Trouble!!!
        else if(psz[0] == '-' && psz[1] == '-' && psz[2] == '>')
            return psz+2; // point to that final '>'
    }
}

////////////////////////////////////////////////////////////////////////////////


// Const version. We make a copy of the string.
MgXmlParser::MgXmlParser(CPSZ pszString)
: m_sString(szdup(pszString))
, m_iPos(UNINITIALIZED_XSIZE_T)
, m_pCurrent(new MgXmlInvalid())
, m_dwOptions(keNone)                  // The default is that you get all
{
    m_iLen = (int) szlen(m_sString);
}

// Non-const version.  We take ownership of the string.
MgXmlParser::MgXmlParser(VPSZ pszString)
: m_sString(pszString)
, m_iPos(UNINITIALIZED_XSIZE_T)
, m_pCurrent(new MgXmlInvalid())
, m_dwOptions(keNone)                  // The default is that you get all
{
    m_iLen = (int) szlen(m_sString);
}

MgXmlParser::~MgXmlParser()
{
    if (m_sString != NULL)
        free((void*)m_sString);

    delete m_pCurrent;
}

// Resets the enumerator.
void MgXmlParser::Reset()
{
    delete m_pCurrent;
    m_pCurrent = new MgXmlInvalid();
    m_iPos = UNINITIALIZED_XSIZE_T;
}

bool MgXmlParser::AtEnd() const
{
    return (m_iPos >= m_iLen - 1);
}

bool MgXmlParser::More() const
{
    return !AtEnd() || m_pCurrent->Type() != keUnknownXml;
}

bool MgXmlParser::Next()
{
    // We're able to skip certain nodes under predetermined
    // conditions; we do this in-situ...
    bool bSkipping;
    do {
        if(AtEnd()) {
            delete m_pCurrent;
            m_pCurrent = new MgXmlInvalid();
            return false;
        }

        m_iPos++;

        // ... but the default is to not skip.
        bSkipping = false;

        // TODO: maybe undo the string pointer.
        CPSZ pszDebug = ((CPSZ)m_sString)+m_iPos; pszDebug;

        // Clean up what was there before.
        delete m_pCurrent;
        m_pCurrent = NULL; // Note: not new MgXmlInvalid(); pointer will be set soon.

        // Examine what is under the "cursor" in our scan forward.
        if(m_sString[m_iPos] == '<') {
            switch(m_sString[m_iPos+1]) {
            case '/':
                m_pCurrent = new MgXmlEndElement(m_sString,m_iPos);
                break;

            case '?':
                m_pCurrent = new MgXmlProcessingInstruction(m_sString,m_iPos);
                if(m_dwOptions & keSkipProcessingInstructions)
                    bSkipping = true;
                break;

            case '!':
                if(IsDoctype(m_iPos+2))
                    m_pCurrent = new MgXmlDoctypeNode(m_sString,m_iPos);
                else if(IsCdata(m_iPos+2))
                    m_pCurrent = new MgXmlCdataNode(m_sString,m_iPos);
                else {
                    m_pCurrent = new MgXmlComment(m_sString,m_iPos);
                    if(m_dwOptions & keSkipComments)
                        bSkipping = true;
                }
                break;

            default:
                m_pCurrent = new MgXmlBeginElement(m_sString,m_iPos);
            }
        }
        else {
            m_pCurrent = new MgXmlTextElement(m_sString,m_iPos);
            if((m_dwOptions & keSkipWhitespace) && ((MgXmlTextElement*)m_pCurrent)->IsWhitespace())
                bSkipping = true;
        }
    } while(bSkipping);

    return true;
}

bool MgXmlParser::IsDoctype(xsize_t iPos)
{
    return m_sString[iPos++] == 'D'
        && m_sString[iPos++] == 'O'
        && m_sString[iPos++] == 'C'
        && m_sString[iPos++] == 'T'
        && m_sString[iPos++] == 'Y'
        && m_sString[iPos++] == 'P'
        && m_sString[iPos  ] == 'E';
}

bool MgXmlParser::IsCdata(xsize_t iPos)
{
    return m_sString[iPos++] == '['
        && m_sString[iPos++] == 'C'
        && m_sString[iPos++] == 'D'
        && m_sString[iPos++] == 'A'
        && m_sString[iPos++] == 'T'
        && m_sString[iPos++] == 'A'
        && m_sString[iPos  ] == '[';
}

MgXmlNode& MgXmlParser::Current()
{
    return *m_pCurrent;
}





//////////////////////////////////

STRING MgXmlNamespaceManager::ksDefaultNsPrefix = _("*");


// Private internal class
class MgXmlNamespaceScope: public MgUtilDictionary
{
public:
    MgXmlNamespaceScope(CPSZ pszScopeName,MgXmlNamespaceScope* pNextScope)
      : MgUtilDictionary(pNextScope)
    {
        m_sScopeName = pszScopeName;
    }

    STRING ScopeName() const
    {
        return m_sScopeName;
    }

protected:
    bool CompareSame(CPSZ pszName,CPSZ pszCandidate);

private:
    STRING m_sScopeName;
};




MgXmlNamespaceManager::MgXmlNamespaceManager()
: m_pTopScope(NULL)
{
}


MgXmlNamespaceManager::~MgXmlNamespaceManager()
{
    // Sometimes we're left with one or more namespace scopes due to mismatched
    // calls to TrackBeginElement/TrackEndElement.  We need to delete these to
    // reclaim memory.
    while (m_pTopScope != NULL)
    {
        MgXmlNamespaceScope* pOldTop = m_pTopScope;
        m_pTopScope = (MgXmlNamespaceScope*)m_pTopScope->NextScope();
        delete pOldTop;
    }
}


bool MgXmlNamespaceManager::TrackBeginElement(MgXmlBeginElement& oBegin)
{
    m_pTopScope = new MgXmlNamespaceScope(oBegin.Name().c_str(),m_pTopScope);
    if(!m_pTopScope)
        return false; // Catastrophic: malloc failed, unable to continue.

    if(oBegin.HasAttributes()) {
        MgXmlAttribute& oAttr = oBegin.Attributes();
        while(oAttr.Next()) {
            STRING sName = oAttr.Name();
            // Declaring the default namespace?
            if(sName == _("xmlns"))
                AddNamespace(ksDefaultNsPrefix,oAttr.Value());
            // How about a specific namespace?
            else if(IsXmlNs(sName.c_str()))
                AddNamespace(sName.substr(/*skip "xmlns:" */6),oAttr.Value());
        }
    }

    return true;
}


bool MgXmlNamespaceManager::AddNamespace(CREFSTRING sPrefix,CREFSTRING sNamespace)
{
    if(m_pTopScope != NULL)
        return m_pTopScope->AddDefinition(sPrefix,sNamespace);
    else
        return false;
}


bool MgXmlNamespaceManager::TrackEndElement  (MgXmlEndElement&   oEnd)
{
    if(m_pTopScope == NULL)
        return false; // We don't expect that: end w/o matching begin?

    if(oEnd.Name() != m_pTopScope->ScopeName())
        return false;  // We don't expect that: </end> not the same as <begin>

    // Remove that top scope.
    MgXmlNamespaceScope* pOldTop = m_pTopScope;
    m_pTopScope = (MgXmlNamespaceScope*)m_pTopScope->NextScope();
    delete pOldTop;

    return true;
}


STRING MgXmlNamespaceManager::NamespaceFrom(STRING sPrefix)
{
    CPSZ pszNamespace = m_pTopScope? m_pTopScope->operator [](sPrefix.c_str()) : NULL;
    // Return an empty namespace if nothing is found.  That's
    // what we have to do since STRINGS can't be NULL themselves.
    return STRING(pszNamespace? pszNamespace : _(""));
}


STRING MgXmlNamespaceManager::PrefixFrom(STRING sNamespace)
{
    MgUtilDictionary* pScope = m_pTopScope;
    while(pScope != NULL) {
        // Traverse this scope in inverse lookup fashion
        // (look for value, not name)
        int iCount=pScope->Count();
        for(int i=0;i<iCount;i++) {
            if(sNamespace == (CPSZ)pScope->Value(i))
                return pScope->Name(i);
        }
        // Not in this scope... next?
        pScope = pScope->NextScope();
    }
    return _("");
}


bool MgXmlNamespaceManager::HasNamespace(STRING sNamespace)
{
    STRING sPrefix = PrefixFrom(sNamespace);
    return sPrefix.length() > 0;
}




STRING MgXmlNamespaceManager::QualifiedName(STRING sName)
{
    STRING::size_type iSep = sName.find(_(":"));
    if(iSep == STRING::npos) {
        // Unprefixed name; might be in default namespace,
        // or it might just be vanilla.
        STRING sNamespace = NamespaceFrom(ksDefaultNsPrefix);
        if(sNamespace.length() > 0)
            return sNamespace+_(":")+sName;
        else
            return sName; // not much to do on a name w/o namespace ;-)
    }
    else {
        STRING sPrefix = sName.substr(0,iSep);
        STRING sNamespace = NamespaceFrom(sPrefix);
        return sNamespace + sName.substr(iSep);
    }
}


STRING MgXmlNamespaceManager::QualifiedName(MgXmlBeginElement& oBegin)
{
    return QualifiedName(oBegin.Name());
}


STRING MgXmlNamespaceManager::QualifiedName(MgXmlAttribute& oAttr)
{
    return QualifiedName(oAttr.Name());
}

STRING MgXmlNamespaceManager::QualifiedName(MgXmlEndElement& oEnd)
{
    return QualifiedName(oEnd.Name());
}

bool MgXmlNamespaceManager::IsXmlNs(CPSZ pszAttrName)
{
    return pszAttrName[0] == 'x'
        && pszAttrName[1] == 'm'
        && pszAttrName[2] == 'l'
        && pszAttrName[3] == 'n'
        && pszAttrName[4] == 's'
        && pszAttrName[5] == ':';
}

int MgXmlNamespaceManager::TotalCount()
{
    int iCount = 0;
    MgUtilDictionary* pScope = m_pTopScope;
    while(pScope != NULL) {
        iCount += pScope->Count();
        pScope = pScope->NextScope();
    }
    return iCount;
}


//
bool MgXmlNamespaceManager::Namespace(int iIndex,REFSTRING sNamespace)
{
    if(iIndex >= 0) {
        MgUtilDictionary* pScope = m_pTopScope;
        while(pScope != NULL) {
            int iCount = pScope->Count();
            if(iIndex < iCount) {
                sNamespace = (CPSZ)pScope->Value(iIndex);
                return true;
            }

            // Else, keep going.
            iIndex -= iCount;
            pScope = pScope->NextScope();
        }
    }
    // index too high if we got here.
    return false;
}


bool MgXmlNamespaceManager::Prefix(int iIndex,REFSTRING sPrefix)
{
    if(iIndex >= 0) {
        MgUtilDictionary* pScope = m_pTopScope;
        while(pScope != NULL) {
            int iCount = pScope->Count();
            if(iIndex < iCount) {
                sPrefix = pScope->Name(iIndex);
                return true;
            }

            // Else, keep going.
            iIndex -= iCount;
            pScope = pScope->NextScope();
        }
    }
    // index too high if we got here.
    return false;
}

bool MgXmlNamespaceManager::FindPrefix(CREFSTRING sPrefix,int& iIndex)
{
    MgUtilDictionary* pScope = m_pTopScope;
    int iLocalIndex = 0;
    while(pScope != NULL) {
        int iCount = pScope->Count();
        for(int i=0;i<iCount;i++) {
            if(sPrefix == pScope->Name(i)) {
                iIndex = iLocalIndex + i;
                return true;
            }
        }
        // Not in this scope? keep going
        iLocalIndex += iCount;
        pScope = pScope->NextScope();
    }
    iIndex = -1;
    return false;
}

bool MgXmlNamespaceManager::FindNamespace(CREFSTRING sNamespace,int& iIndex)
{
    MgUtilDictionary* pScope = m_pTopScope;
    int iLocalIndex = 0;
    while(pScope != NULL) {
        int iCount = pScope->Count();
        for(int i=0;i<iCount;i++) {
            if(sNamespace == (CPSZ)pScope->Value(i)) {
                iIndex = iLocalIndex + i;
                return true;
            }
        }
        // Not in this scope? keep going
        iLocalIndex += iCount;
        pScope = pScope->NextScope();
    }
    iIndex = -1;
    return false;
}

bool MgXmlNamespaceManager::IsEclipsed(int iIndex)
{
    STRING sPrefix;
    if(!Prefix(iIndex,sPrefix))
        return false;

    int iCheck;
    if(!FindPrefix(sPrefix,iCheck))
        return false;

    return iCheck != iIndex;
}





bool MgXmlNamespaceScope::CompareSame(CPSZ pszName,CPSZ pszCandidate)
{
    // default implementation: case sensitive stuff.
    return SZ_EQ(pszName,pszCandidate);
}
