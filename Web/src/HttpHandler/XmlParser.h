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

#ifndef _XmlParser_h
#define _XmlParser_h
////////////////////////////////////////////////////////////////////////////////
//
//  D E S E R I A L I Z A T I O N   S U P P O R T
//
////////////////////////////////////////////////////////////////////////////////
//
//  These are light-weight readers, and while they're fairly robust, they do
//  assume fairly well-formed XML coming in.  (They aren't anal or paranoid
//  in their parsing, and there's no schema validation, either.)
//
//  In general, the goal is to provide a once-through pass of an XML-like
//  document, with a minimum of heap churn (other than the original document)
//  and a minimum of code complexity.
//
//  Assumes the relevant portion of the XML document is in memory.  (Typically,
//  the entire document is in memory, as a string somewhere.)

////////////////////////////////////////////////////////////////////////////////
// Types of nodes we know about
enum MgXmlNodeType {
  keUnknownXml,
  keText,                     // stuff between <>'s: ...>text text text<...
  keBeginElement,             // an ordinary XML <beginofelement> tag.
  keEndElement,               // an ordinary XML </endofelement> tag.
  keAttribute,                // an attribute, as in <begin attrname="attrvalue">
  keProcessingInstruction,    // a <?processing-instruction?>
  keComment,                  // a <!-- comment -->
  keDoctype,                  // a <!DOCTYPE> element
  keCdata                     // a <![CDATA[ ]]> block
};

// It's convenient for us to have a "size_t" type where an undefined
// value is just below zero (ie, -1).
// * The int type fits the bill, but causes problems when accepting
//   size_t (which is unsigned) -- generating a slew of compiler warnings
//   about data truncation possibilities.
// * Just going with size_t makes the compiler warnings disappear,
//   but horribly "complexifies" the logic needed to start with a
//   pointer "just before the beginning".
// * Casting size_t into int isn't great, because it masks a problem
//   if we should ever happen to get pointer spreads greater than max int.
// * While not perfect, a cast to a synonym of int at least allows us
//   to keep tabs on those casts, when and if data truncation becomes real.
//   (What? a document that large?  Well, it wasn't so long ago we
//   were saying the same about 64K, then 1MB segmented boundaries... ;)
//   At leat now there's something explicit to search for -- casting
//   to (xsize_t) -- and a central place to redefine the synonym.
typedef int xsize_t;
#define UNINITIALIZED_XSIZE_T ((xsize_t)-1)

////////////////////////////////////////////////////////////////////////////////
// Generic definition of node.  Abstract type.
class MgXmlNode
{
protected:
    MgXmlNode() {}

    int     Advance(CPSZ pszInput) const;
    int     AdvanceOverName(CPSZ pszInput) const;
    CPSZ    AdvanceOverWhitespace(CPSZ psz);
    int     AdvanceOverQuote(CPSZ psz);

public:
    virtual MgXmlNodeType Type() const =0;
    STRING Contents() const;

protected:
    CPSZ    m_pszStart;  // pointer to the start of the node's representation
    xsize_t m_iLen;      // the length, in characters, of the node's representtion
};


// Used to represent the "invalid" state prior to
// beginning and after end of stream.  Allows
// parser->Current() to return something at all
// times (since it is a reference)
class MgXmlInvalid: public MgXmlNode
{
public:
    MgXmlInvalid();

    MgXmlNodeType Type() const { return keUnknownXml; }
};

////////////////////////////////////////////////////////////////////////////////
// Attributes:  <element attrib_name="attrib_value" ...>
class MgXmlAttribute: public MgXmlNode
{
    friend class MgXmlAttributedNode;
public:
    MgXmlAttribute() {};

    MgXmlNodeType Type() const { return keAttribute; }

    // Enumerator-like semantics.
    bool Next(); // advances to the next Attribute.

    // Get the name and value of the attribute: name="value" or name='value'
    STRING Name() const;
    STRING Value() const;

private:
    CPSZ    m_pszName;
    int     m_iNameLen;
    CPSZ    m_pszValue;
    int     m_iValueLen;
};

////////////////////////////////////////////////////////////////////////////////
// Represents a DOCTYPE node, such as
//  <!DOCTYPE RootElement (SYSTEM "someDTD" | PUBLIC "Identifier" "someDTD")
//      ([ InternalSubset ]) >
class MgXmlDoctypeNode: public MgXmlNode
{
public:
    MgXmlDoctypeNode(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keDoctype; }

    STRING RootElement();

    bool IsSystem();
    bool IsPublic(REFSTRING sIdentifier);

    STRING DTD();

    STRING InternalSubset();

private:
    CPSZ AdvanceToDoctypeEnd(CPSZ psz);

    CPSZ m_pszRootElement;
    int m_iRootElementLen;

    bool m_bSystem;
    bool m_bPublic;

    CPSZ m_pszPublicIdentifier;
    int m_iPublicIdentifierLen;

    CPSZ m_pszDtd;
    int m_iDtdLen;

    CPSZ m_pszInternalSubset;
    xsize_t m_iInternalSubsetLen;

};

////////////////////////////////////////////////////////////////////////////////
// Represents a CDATA node, such as  <![CDATA[ ... ]]>
class MgXmlCdataNode: public MgXmlNode
{
public:
    MgXmlCdataNode(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keCdata; }

    STRING Data();

private:
    CPSZ AdvanceToCDataEnd(CPSZ psz);
};

////////////////////////////////////////////////////////////////////////////////
// Represents a node with attributes, such as <begin ...> or <?process-instruct ...?>
class MgXmlAttributedNode: public MgXmlNode
{
public:
    MgXmlAttributedNode() {};

    bool HasAttributes();
    MgXmlAttribute& Attributes();

    // Quick access to a specific named attribute.
    bool GetAttribute(CPSZ pszName,REFSTRING sValue);

protected:
    int m_iEndOfName;             // m_pszStart[m_iEndOfName] is first char past name
    virtual int EndDecorationLen() = 0; // how many chars at the end of content are not "attribute"
    MgXmlAttribute m_Attributes;
};

////////////////////////////////////////////////////////////////////////////////
// Represents a <?processing-instruction ...?>
class MgXmlProcessingInstruction: public MgXmlAttributedNode
{
public:
    MgXmlProcessingInstruction(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keProcessingInstruction; }

    // The name, sans ? (which is syntax): <?xml version='1.0'?> produces 'xml'
    STRING Name() const;

    // The almost-ubiquitous PI at the start of an XML file warrants its own
    // special check.  Otherwise, you could do PI.Name() == "xml", but with
    // a lot more overhead.
    bool IsXml() const;

protected:
    int EndDecorationLen();
};


////////////////////////////////////////////////////////////////////////////////
// Represents a <begin> as opposed to </end> of an element
class MgXmlBeginElement: public MgXmlAttributedNode
{
public:
    MgXmlBeginElement(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keBeginElement; }

    STRING Name() const;

    bool IsEmpty() const; // Is it <begin ... /> element? don't expect </end>

protected:
    virtual int EndDecorationLen();
};



////////////////////////////////////////////////////////////////////////////////
// Represents the </end> of an element
class MgXmlEndElement: public MgXmlNode
{
public:
    MgXmlEndElement(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keEndElement; }

    STRING Name() const;
};


////////////////////////////////////////////////////////////////////////////////
// Represents text between elements.
class MgXmlTextElement: public MgXmlNode
{
public:
    MgXmlTextElement(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keText; }

    // Returns true if the entire string is purely whitespace.
    bool IsWhitespace() const;
};

////////////////////////////////////////////////////////////////////////////////
// Represents a <!-- comment -->
class MgXmlComment: public MgXmlNode
{
public:
    MgXmlComment(CPSZ pszString,xsize_t& iStartStop);

    MgXmlNodeType Type() const { return keComment; }

    STRING Text() const;
};



////////////////////////////////////////////////////////////////////////////////
// The parsing tool.  Takes a string, and iterates over the XML tokens within that
// in a one-pass fashion, using Enumerator-like semantics.  Generally very
// efficient since there's no multiple-scanning of the string, and relatively
// little heap activity.
class MgXmlParser
{
public:
    MgXmlParser(CPSZ pszString); // TODO: rewrite to be stream-based?
    MgXmlParser(VPSZ pszString); //
    ~MgXmlParser();

public:
    // Enumerator-like semantics:
    // Advances to the next item.
    // Typical use: while(Next()) ...
    bool Next();
    bool More() const;

    // Are we at the end of the stream?
    // Not normally used, as Next() combines
    // this info with an advance to the next
    // token.  Not used in normal cases,
    // though some special applications need this.
    bool AtEnd() const;

    // Access to the current item.  Note that the reference is only valid
    // until the next call to Next() or the destruction of the parser,
    // as the parser owns the reference and discards it then.
    MgXmlNode& Current();

    // Do you want the parser to automatically skip whitespace text elements?
    // Processing Instructions? Comments? ... ?
    // If so, Current() will never report them to you.
    // Construction default is false; you must ask for them to be skipped.
    void SetOptions(LONGBITS dwOptions) { m_dwOptions = dwOptions; }

    // Returns options currently in effect.
    LONGBITS Options() { return m_dwOptions; }

    // Resets the enumerator.
    void Reset();

private:
    bool IsDoctype(xsize_t iPos);
    bool IsCdata(xsize_t iPos);

    CPSZ      m_sString;
    xsize_t   m_iLen;
    xsize_t   m_iPos;
    MgXmlNode* m_pCurrent;
    LONGBITS  m_dwOptions;
};


// For use in SetOptions
enum CParseOptions{
    keNone                       = 0,
    keSkipWhitespace             = 0x00000001,
    keSkipComments               = 0x00000002,
    keSkipProcessingInstructions = 0x00000004
};

////////////////////////////////////////////////////////////////////////////////
// A helper tool that facilitates the reading of an element's values.
// It (a) verifies that we're starting over an element's begin tag
//    (b) allows us to verify that we've reached an element's end tag, when we're ready
//    (c) allows us to advance past the end element, and
//    (d) automatically does (c) on destruction, making sure we're never out of sync
// Create one of these on the stack and when it goes out of scope, you'll be
// advanced past the ending tag.
class MgXmlSynchronizeOnElement
{
public:
    // Given a stream, and the element name we're supposed to synchronize on
    MgXmlSynchronizeOnElement(MgXmlParser& InStream,CPSZ pszElementName)
    : m_InStream(InStream),m_pszElementName(pszElementName)
    , m_bInElement(false),m_bInEmptyElement(false),m_bAtEnd(false)
    {
    }
    // At end, advance us so the InStream is ready for whatever follows.
    virtual ~MgXmlSynchronizeOnElement()
    {
        AdvancePastEnd();
    }

    // Are we at the beginning?  Make this call soon after construction
    // to make sure you are where you think you should be.
    // returns: false = no, out of sync; (not at a begin element, or wrong one)
    //          true  = yes, at expected begin element; pointer to element provided
    //                  Note: pointer pBegin only valid until next call to InStream.Next()
    bool AtBegin(MgXmlBeginElement** ppBegin)
    {
        // Not at <begining> tag?
        if(m_InStream.Current().Type() != keBeginElement)
            return false;

        // At <SomeOtherElement> tag?
        MgXmlBeginElement& Begin = (MgXmlBeginElement&) m_InStream.Current();
        if(!CompareNames(Begin,m_pszElementName))
            return false;

        // Make a note of the fact that we're in the element now.
        // (if there's an element to be within...)
        // FIX: regardless of whether we're over an empty <begin/>
        // element or not, we have to consider ourselves "in" the
        // element at this point... We may also be "at end" too
        // but that will be a separate check.
        m_bInElement = true;
        m_bInEmptyElement = Begin.IsEmpty();

        // If they're interested in
        if(ppBegin != NULL)
            *ppBegin = &((MgXmlBeginElement&) m_InStream.Current());

        return true;
    }

    // Convenience method: no interest in the begin element
    // so let's just automatically advance past it if we're
    // happy.
    bool AtBegin()
    {
        if(AtBegin(NULL)) {
          // Okay, we're happy.  Advance to be ready for what follows.
          m_InStream.Next();
          return true;
        }

        return false;
    }

    bool AtEnd()
    {
        // Not in the element.  Let's just get out.
        if(!m_bInElement)
            return true;

        // We're in an element, but it's empty, so
        // by definition, we're at the end.
        if(m_bInEmptyElement)
            return true;

        // Pretty conclusive: if the stream's at end, so are we.
        // Likely a bad chunk of XML, though.
        if(m_InStream.AtEnd())
            return true;

        // Not at </ending> tag?
        if(m_InStream.Current().Type() != keEndElement)
            return false;

        // At </SomeOtherElement> tag?
        MgXmlEndElement& End = (MgXmlEndElement&) m_InStream.Current();
        if(!CompareNames(End,m_pszElementName))
            return false;

        // Okay, we're at the end.  Happily leave.
        m_bAtEnd = true;
        return true;
    }

    void AdvancePastEnd()
    {
        // We were never meant to be here, let's not touch anything
        if(!m_bInElement)
            return;

        // We know we're within the element, so if we're not
        // already at the end, let's zip along until we're there.
        // Note that this is poor-man's logic that doesn't handle
        // recursive elements neatly.
        // So far, that's not a problem.
        if(!m_bAtEnd && !m_bInEmptyElement)
            while(!AtEnd())
                if(!m_InStream.Next())
                    break;

        // Now, go one past the next.  Harmless if we're already at the end.
        m_InStream.Next();

        // No longer within element anymore.
        m_bInElement = false;
    }

protected:
    // Allow derivations to determine equality
    // Default is plain-ol' string comparison.
    virtual bool CompareNames(MgXmlBeginElement& oBegin,CPSZ pszDesiredName)
    {
        return oBegin.Name() == pszDesiredName;
    }
    virtual bool CompareNames(MgXmlEndElement& oEnd,CPSZ pszDesiredName)
    {
        return oEnd.Name() == pszDesiredName;
    }

private:
    MgXmlParser& m_InStream;
    CPSZ m_pszElementName;
    bool m_bInElement,m_bInEmptyElement,m_bAtEnd;
};


/// ------------ Namespace specifics

class MgXmlNamespaceScope;

class MgXmlNamespaceManager
{
public:
    MgXmlNamespaceManager();
    ~MgXmlNamespaceManager();

    // These two methods track the scope of namespaces
    // by hooking into the Begin and End elements.
    bool TrackBeginElement(MgXmlBeginElement& oBegin);
    bool TrackEndElement  (MgXmlEndElement&   oEnd);

    // Given the element <begin xmlns:foo="http://bar.com/quux" ...>
    //  NamespaceFrom("foo") -> "http://bar.com/quux"
    //  PrefixFrom("http://bar.com/quux") -> "foo"
    STRING NamespaceFrom(STRING sPrefix);
    STRING PrefixFrom(STRING sNamespace);
    // Determines if a specific namespace has been defined,
    // regardless of the prefix used.
    bool HasNamespace(STRING sNamespace);

    // The prefix used to specify xmlns=
    static STRING ksDefaultNsPrefix;

    // Given the element <foo:begin xmlns:foo="http://bar.com/quux" ...>
    // QualifiedName("foo:begin") -> "http://bar.com/quux:begin"
    STRING QualifiedName(STRING sName);
    // Shorthands: operate as above, taking the name of the Xml Node indicated
    STRING QualifiedName(MgXmlBeginElement& oBegin);
    STRING QualifiedName(MgXmlAttribute& oAttr);
    STRING QualifiedName(MgXmlEndElement& oEnd);

    bool AddNamespace(CREFSTRING sPrefix,CREFSTRING sNamespace);

    // Gets the total count of namespaces, including
    // duplications brought about by scope eclipses.
    int TotalCount();
    // These are indexed 0 .. TotalCount()-1, and may reflect
    // duplicate prefix declarations.
    bool Namespace(int iIndex,REFSTRING sNamespace);
    bool Prefix(int iIndex,REFSTRING sPrefix);
    // Determines if the prefix=Namespace definition iIndex
    // is eclipsed by another definition using the same prefix.
    bool IsEclipsed(int iIndex);

    bool FindPrefix(CREFSTRING sPrefix,int& iIndex);
    bool FindNamespace(CREFSTRING sNamespace,int& iIndex);

private:
    bool IsXmlNs(CPSZ pszAttrName);
    MgXmlNamespaceScope* m_pTopScope;
};


class MgXmlSynchronizeOnNamespaceElement: public MgXmlSynchronizeOnElement
{
public:
    // This class works like its plain namesake, except that the
    // pszQualifiedElementName is a fully-qualified name (a la
    // "http://foo.com/bar:begin") so that <bar:begin xmlns:bar="http://foo.com/bar" ...>
    // will match it.  This is handled by the namespace manager
    // also passed along during construction.
    //
    MgXmlSynchronizeOnNamespaceElement(MgXmlParser& InStream,CPSZ pszQualifiedElementName,MgXmlNamespaceManager& oNamespaces)
    :   m_oNamespaces(oNamespaces),MgXmlSynchronizeOnElement(InStream,pszQualifiedElementName)
    {
    }

    ~MgXmlSynchronizeOnNamespaceElement()
    {
        // Need to do this now, since by the time the base dtor is
        // called, we're not "OnNamespace" flavor anymore.
        AdvancePastEnd();
    }



protected:
    // Before we see if we're synched on the desired Begin element node,
    // let's track the namespaces the element may introduce.  Based on what
    // we learn, (and whatever other namespaces may already be defined)
    // we build a qualified name to compare against.
    bool CompareNames(MgXmlBeginElement& oBegin,CPSZ pszDesiredName)
    {
        m_oNamespaces.TrackBeginElement(oBegin);

        return m_oNamespaces.QualifiedName(oBegin.Name()) == pszDesiredName;
    }

    // And let's see if this is the fully qualified name matches
    // the end element we're over.  If so, we also need to note
    // the end of the corresponding scope of namespaces.
    bool CompareNames(MgXmlEndElement& oEnd,CPSZ pszDesiredName)
    {
        bool bRet = m_oNamespaces.QualifiedName(oEnd.Name()) == pszDesiredName;

        m_oNamespaces.TrackEndElement(oEnd);

        return bRet;
    }


private:
    MgXmlNamespaceManager& m_oNamespaces;
};

#endif//_XmlParser_h
