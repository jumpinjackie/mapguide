//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _OgcServer_h
#define _OgcServer_h

#include "HttpRequestParameters.h"
#include "ResponseStream.h"
#include "OgcException.h"
#include "Dictionary.h"

// Forward declarations.
class MgXmlParser;
class MgXmlProcessingInstruction;

// Defines the signature for a user-provided function
// which loads a document.  The user's implementation can
// abstract the actual means of acquiring the bits, and
// handle any encoding issues, since what gets returned
// is a STRING (which has a predetermined encoding, presently
// UTF-16)
typedef bool (*DocLoaderFunc)(CPSZ pszDoc,REFSTRING sRet);

enum EscapeState {
    keNormal = 0,
    keEscaping = 1,
    keUnescaping = -1
};

// Simple interface for enumeration of resources.
class IOgcResourceEnumerator // abstract interface
{
public:
    // Is there a next?  Logically, the enumerator must begin
    // at a state before the first item, so that the initial Next()
    // advances to the first item, or returns false if the collection
    // is empty.  Allows while(Next()) logic flow.
    virtual bool Next() = 0;

    // While Next() returns true, the enumerator must be positioned at
    // a valid member of the collection, and this call pushes into the
    // dictionary whatever definitions are deemed appropriate.
    virtual void GenerateDefinitions(MgUtilDictionary& Dictionary) = 0;
};

class MgOgcServer {
public:
    MgOgcServer(MgHttpRequestParameters& Request,
                CStream& Response,
                MgUtilDictionary& rGlobalDefinitions);
    ~MgOgcServer();

    // Not much to be said here.  Just respond to the request sent in.
    // One stop shopping: call this after construction, and it all happens.
    virtual bool ProcessRequest(IMgOgcDataAccessor* dataAccessor);

    // Gets the value of the the Request's parameter
    // Note that this applies both pre-fetch mapping of
    // parameter names, and post-fetch mapping of parameter
    // values, if definitions are present in the template files.
    CPSZ RequestParameter(CPSZ pszParameter);

    // Uses a Map definition to map the From string to the resulting To string.
    bool MapValue(MgXmlParser& Map,CPSZ pszFrom,REFSTRING sTo);
    // Convenience overload: grabs the definition you provide from the
    // dictionary, and uses that to map from -> to; returns false if no
    // definition found, or if otherwise the mapping fails
    bool MapValue(CPSZ pszDefinitionName,CPSZ pszFrom,REFSTRING sTo);

    // Allows external agents to transform arguments as if they were
    // a variable of the same name.  Useful for allowing HTTP POST operations
    // the same template-guided fixups as HTTP GET has.
    CPSZ ProcessArgumentAs(CPSZ pszOstensibleName,CPSZ pszActualValue);

    // Generate an exception response given the exception indicated.
    // This loads the template, and attempts to find an Exception response
    // in it.  Failing that, it generates a default exception response.
    void ServiceExceptionReportResponse(const MgOgcException& Exception);

protected:

    // Method to be implemented by derived classes to validate request parameters
    virtual bool ValidateRequest() = 0;

    // Method to be implemented by derived classes to generate the response data
    virtual void RespondToRequest() = 0;


    // Generates a response from the template.  Typically, this is
    // what most derivations will call.  It merely looks for an
    // element of
    // <Response request="..." content-type="...">...</Response>
    // and plays back the contents to the response object a la
    // ProcessXmlStream, below.  Parameter pszSpecificMimeType can be NULL
    // in which case the first encountered Response matching the
    // desired pszRequest will be taken.
    //
    // Automatically sets the response type to the type indicated by the
    // template.
    //
    // Returns false if it was not able to find a suitable response object.
    bool GenerateResponse(CPSZ pszRequest,CPSZ pszSpecificMimeType);

    // This is the ExceptionResponse of last resort, using the globally
    // defined response.  Typically, a template file will define its own
    // response, so this should never get invoked, but just in case things
    // go south, this is here.
    bool DefaultExceptionResponse();

    // Get the value of the defined item indicated.  Definitions
    // differ from Request Parameters in that the latter are created
    // from the outside world, while definitions are created internally.
    // Other than that, though, they're fundamentally the same.
    CPSZ Definition(CPSZ pszItem);

    // Some overloads to facilitate adding definitions.
    bool AddDefinition(CPSZ pszItem,CPSZ pszValue);
    bool AddDefinition(CPSZ pszItem,CREFSTRING sValue);
    bool AddDefinition(CPSZ pszItem,int iValue);

    // Certain operations define transient definitions that should
    // eclipse definitions already made.  The one basic example is that
    // a bunch of global definitions are made, and each server instance
    // can provide its own definitions/overrides based on requests or
    // other circumstances.
    // These must be paired, and are typically governed by an on-stack
    // CDictionaryStackFrame object (see below.)
    void PushDictionary();
    void PopDictionary();

    // Small helper class, enforces stack frames by ensuring
    // that Pushes and Pops remain paired.
    class CDictionaryStackFrame
    {
    public:
        CDictionaryStackFrame(MgOgcServer* pServer);
        ~CDictionaryStackFrame();

    private:
        MgOgcServer* m_pServer;
    };

    // Allows temporary redirection of output stream
    class CSubstituteStream
    {
    public:
        CSubstituteStream(MgOgcServer*,CStream& Stream);
        ~CSubstituteStream();
    private:
        CStream*      m_pStreamOld;   // remember the prevous stream
        MgOgcServer*  m_pServer;      // remember the server
        // persist the original states
        bool m_bWriteEnabled;
        EscapeState m_eEscapeState;
    };

    // Pushes Exception values into the current dictionary stack frame
    // in order to be available for the ExceptinResponse.
    void DefineExceptionValues(const MgOgcException& Exception);

    // This determines the negotiated version number using OGC Version
    // Negotiation logic.
    CPSZ NegotiatedVersion(CPSZ pszRequested = NULL);

    // For convenience, any internal errors (not those specific to any service)
    // can be generated here.
    void InternalError(CPSZ pszMessage);

    // Two overloads, depending on whether you have a template open or not.
    // Tries to find the <Response> element for an exception, and failing that
    // goes with the default Exception response.
    bool ExceptionResponse(MgXmlParser& Template,const MgOgcException& Exception);
    bool ExceptionResponse(                      const MgOgcException& Exception);

    // Given an Xml Parser, simply pushes the contents to the response
    // object, expanding whatever definitions -- and processing
    // any Processing Instructions -- it encounters.
    void ProcessXmlStream(MgXmlParser& Xml);

    // Prior to responding (that is, prior to any call to Write)
    // you must set the response type.
    // Note that GenerateResponse does this for you.
    void SetContentType(CPSZ pszMimeType);

    // All writing must go through this collection of methods; no direct
    // references to m_pResponse outside of these methods are allowed.
    bool Write(CPSZ pszText);
    bool Write(CPSZ pszText,int iChars);
    bool Write(const STRING sText);
    bool WriteLine();

    // Support methods for ProcessXmlStream.
    //
    // Takes a string and expands any Expansion Text it finds
    void ProcessExpandableText(STRING pszTextWith);
    void ProcessExpandableText(CPSZ pszTextWith);
    void ProcessExpandableText(CPSZ pszTextWith,int iLength);

    void ProcessExpandableTextIntoString(CPSZ pszText,REFSTRING sOut);
    void ProcessExpandableTextIntoString(STRING sText,REFSTRING sOut);

    // Like ProcessExpandableTextIntoString, above, but allows full
    // XML processing, like PIs etc.  Only for use in full XML contexts,
    // NOT for processing PI arguments.
    void ProcessXmlIntoString(CPSZ pszText,REFSTRING sOut);

private:

    // Takes a single expansion and provides is substitution.
    // Copes with some syntactic dressing around the edges.
    CPSZ ProcessExpansion(CPSZ pszExpansionName);

    // Represents the actual text of the expansion.
    void Expansion(const STRING& sExpansionName);

    // Procedures to process various processing instructions.
    void ProcessInstruction(MgXmlProcessingInstruction& PI);

    // The following PIs are understood by the OgcServer.  Others may
    // be defined by derived classes.

    // <?Enum          -- enumerates a structured list of items.
    //   list="list"   -- the contents of the list, typically an expansion
    //                    of one (or more) Define items.  The format of these
    //                    Define(s) is zero or more <item>text</item> items,
    //                    which are enumerated in order.  Contents within
    //                    the <item> and </item> tags are bound to the
    //                    Enum.item definition in turn.  Furthermore, any
    //                    attributes in the <item> tag create Defines with
    //                    the name Enum.item.attrname and the value of the
    //                    attribute.  Any contents outside of <item>...</item>
    //                    tags is ignored, and can be used for comments that will
    //                    not appear in the enumeration.
    //   using="for-each-item"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &Enum.item;
    //   between="between-each-item"
    //                 -- the contents to be used to separarate each item in
    //                    the list; defaults to an empty string, but can be used
    //                    to embed a string between items.
    //   item="element-name"
    //                 -- the name of the element to consider instead of "item"
    //                    for example,
    //                     <?Enum list="&SelectedVersions;" ... item="Version"?>
    //                    will enumerate each Version element.  (Note that since
    //                    these are empty elements, Enum.item is an empty string,
    //                    but Enum.item.number becomes meaningful, since the Version
    //                    element has a number attribute.)
    //                    This attribute is optional, and if omitted, causes the
    //                    PI to look for <item> elements.
    //   subset="list"
    //                 -- a comma-separated list of integers (note: no spaces around
    //                    commas) to be iterated.  For example, subset="1,3,5,7"
    //                    only iterates the first four odd items encountered (assuming
    //                    that there are that many.)
    //                    This attribute is optional, and if omitted, or is an empty
    //                    string, no subsetting is performed: the entire list of items
    //                    is enumerated.
    //   Creates the definition "Enum.iteration" that reflects the current iteration value,
    //   a one-based index (that is, it will equal 1, 2, 3, etc. or as dictated by the
    //   subset.)
    // ?>
    void ProcedureEnum          (MgXmlProcessingInstruction& PIEnum);

    // <?EnumDelim    -- enumerates a delimited string of items
    //   list="list"  -- the contents of the string, typically an expansion
    //                   of one (or more) Define items, but could be entirely
    //                   in-line as a static literal.
    //   sep="sepch"  -- the separation character (or string) used to delimit
    //                   items in the list.  Defaults to a comma if omitted
    //                   from the EnumDelim PI.  Note that only exact matches
    //                   work, and no additional parsing occurs -- so sep=","
    //                   will slice list="Larry, Curly, Moe" into "Larry",
    //                   " Curly", and " Moe", including the extra blanks.
    //                   Instead, the string "Larry,Curly,Moe" would yield
    //                   the expected names, as would the sep=", "
    //   using="for-each-item"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &Enum.item;
    //   between="between-each-item"
    //                 -- the contents to be used to separarate each item in
    //                    the list; defaults to an empty string, but can be used
    //                    to embed a string between items.
    //   subset="list"
    //                 -- a comma-separated list of integers (note: no spaces around
    //                    commas) to be iterated.  For example, subset="1,3,5,7"
    //                    only iterates the first four odd items encountered (assuming
    //                    that there are that many.)
    //                    This attribute is optional, and if omitted, or is an empty
    //                    string, no subsetting is performed: the entire list of items
    //                    is enumerated.
    //   Creates the definition "Enum.iteration" that reflects the current iteration value,
    //   a one-based index (that is, it will equal 1, 2, 3, etc. or as dictated by the
    //   subset.)
    // ?>
    void ProcedureEnumDelim(MgXmlProcessingInstruction& PIEndif);

    // <?EnumDictionary
    //   using="for-each-item"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &EnumDictionary.Name; and/or &EnumDictionary.Value;
    //   between="between-each-definition"
    //                 -- the contents to be used to separarate each definition in
    //                    a frame; defaults to an empty string, but can be used
    //                    to embed a string between items.
    //   betweenframes="between-each-frame"
    //                 -- the contents to be used to separarate each frame in
    //                    the dictionary; defaults to an empty string, but can be used
    //                    to embed a string between items.
    //   depth="number-of-frames"
    //                 -- the number of frames to enumerate.  If omitted defaults
    //                    to the entire dictionary, as does any nonpositive integer.
    //                    (Certain operations segregate the dictionary into scope frames
    //                    to eclipse existing definitions.)
    //   This tool is used primarily for debugging and diagnostics, and is
    //   employed during internal errors.
    // ?>
    void ProcedureEnumDictionary(MgXmlProcessingInstruction& PIEnum);

    // <?Escape        -- Escapes the contents of the string provided,
    //                    allowing XML content to appear like text.
    //   text="string" -- The string to be escaped, < and > are
    //                    converted into &lt; and &gt; respectively.
    // ?>
    void ProcedureEscape(MgXmlProcessingInstruction& PIEscape);

    // <?Unscape       -- Unescapes the contents of the string provided,
    //                    allowing text to appear like XML.
    //   text="string" -- The string to be unescaped, < and > are
    //                    converted from &lt; and &gt; respectively.
    // ?>
    void ProcedureUnescape(MgXmlProcessingInstruction& PIEscape);

    // <?Ifdef         -- Begins a conditional block, as determined by
    //                    the existance of a Define item.
    //   item="dictionary-item"
    //                 -- If the item named appears in the dictionary,
    //                    then processing continues until the next
    //                    <?Else?> or <?Endif?> PI.  If the name does
    //                    not appear, then processing is suppressed
    //                    until the next <?Else?> or <?Endif?> PI.
    //   Note that conditional blocks cannot be nested.
    // ?>
    void ProcedureIfdef(MgXmlProcessingInstruction& PIIf);

    // <?If            -- Begins a condition block, as controlled by
    //                    the binary operation indicated.
    //   l=            -- The left-hand operand of the binary operation,
    //                    a string that may contain zero or more expansions
    //                    of Define items.
    //   op=           -- The operation to apply to the left and right
    //                    operands.  The following are defined:
    //
    //                                     Lexical  Lex/Ignore case
    //                                     -------  ---------------
    //                    Equality           eq           eqi
    //                    Inequality         ne           nei
    //                    Less-than          lt           lti
    //                    Greater-than       gt           gti
    //                    Less-or-equal      le           lei
    //                    Greater-or-equal   ge           gei
    //
    //   r=            -- The right-hand operand of the binary operation,
    //                    a string that may contain zero or more expansions
    //                    of Define items.
    // ?>
    void ProcedureIf(MgXmlProcessingInstruction& PIIf);

    // <?Else          -- Toggles the conditional processing state
    // ?>
    void ProcedureElse(MgXmlProcessingInstruction& PIElse);

    // <?Endif         -- Terminates the conditional processing state
    // ?>
    void ProcedureEndif(MgXmlProcessingInstruction& PIEndif);

    // <?Translate     -- Translates a string using a translation table
    //   text="str"    -- The input string to be translated.  Appears as
    //                    the definition "Translate.From" for the duration
    //                    of the translation.
    //   with="translatetable"
    //                 -- The contents of the table, typically an expansion
    //                    of one (or more) Define items.  The format of these
    //                    Define(s) is a structured sequence of zero or more
    //                    <translate from="in">...</translate> values, where the
    //                    input string is compared in turn to each from= value,
    //                    and if they are equal, the result is the contents
    //                    within the <translate>...</translate> elements.  If
    //                    the end of the table is reached and no match has been
    //                    found, the input string passes through unchanged.  If the
    //                    table contains a <translate> element without a from=
    //                    attribute, it is assumed to match any string str,
    //                    so the contents of that element will be the result
    //                    if no other match has been made up to that point.
    //                    Typically, this is used at the bottom of the table
    //                    to provide a "default" case.
    // ?>
    void ProcedureTranslate(MgXmlProcessingInstruction& PITranslate);

    // <?EnumXml       -- Enumerates elements in XML.
    //   list="xml"    -- The input XML string to be parsed.  Typically one
    //                    or more definition containing a fragment of XML.
    //                    Note: to avoid extensive pre-processing of the
    //                    source xml, use the "apostrophe escape notation"
    //                    for any given definition: &'foo; instead of &foo;
    //                    This merely expands the definition (of foo, in this
    //                    case) without evaluating the contents within it.)
    //   using="for-each-item"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &XmlNode.Contents;
    //                    The following items are defined during the enumeration:
    //                    Enum.Fragment     -- the source text (text="...") being
    //                                        enumerated.
    //                    Enum.iteration    -- the iteration count of nodes encountered.
    //                    Enum.depth        -- the current depth (unmatched <begin> elements)
    //                                         encountered; initially 0.
    //                    Enum.item         -- the raw contents of the node, regardless of type
    //                    XmlNode.Type      -- The node type:
    //                                         "text"    -- text
    //                                         "space"   -- whitespace
    //                                         "begin"   -- a begin element
    //                                         "end"     -- an end element. [See Note]
    //                                         "pi"      -- a processing instruction
    //                                         "comment" -- a comment
    //                                         "doctype" -- a DOCTYPE declaration
    //                                         "cdata"   -- a CDATA block
    //   The following only applies to "begin" type
    //                    XmlNode.EmptyBegin-- "/" if empty begin element, else ""
    //   The following only apply to "begin", "end", and possibly "pi"
    //                    XmlNode.Name      -- actual name as it occurs in the source fragment
    //                    XmlNode.Base      -- the base name, stripped of prefix (if any, else == name)
    //                    XmlNode.Prefix    -- the prefix, if any, of the name given
    //                    XmlNode.Namespace -- the namespace, if any, of the element name.
    //                    XmlNode.Fullname  -- the full name of the element: namespace and base.
    //                    XmlNode.Attributes -- the raw collection of attributes.
    //                    XmlNode.AttributeList -- the attributes, formatted as an <item> list
    //   Note: for "end" elements, it may be desirable to use &'Enum.item; instead of &Enum.item;
    //   (the apostrophe escape notation) since that ensures the actual end element won't be parsed
    //   away.  Also note that Enum.depth does not reset to a lesser value until after the end element
    //   has been processed.
    // ?>
    void ProcedureEnumXml(MgXmlProcessingInstruction& PIEnumXml);

    // Helper function for EnumXml
    void GetAttributesFrom(MgXmlAttribute& oAttributes,REFSTRING sAttributes,REFSTRING sAttributeList);

#ifdef _DEBUG
    void ProcedureDiagnostic(MgXmlProcessingInstruction& PIDiag);
#endif

protected:

    // Gets the type of service, such as "WMS", "WFS", etc.
    virtual CPSZ GetServiceType() = 0;

    // Gets the Document Prefix, such as "Wms:" or "Wfs:", ...
    virtual CPSZ GetTemplatePrefix() = 0;

    // Gets the template info for default exception, when no other exception type is at hand.
    virtual void GetDefaultExceptionInfo(REFSTRING sTemplate,REFSTRING sMimeType) = 0;

    // Derivations can add their own PI handling, if they want to.
    virtual bool ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI);

    // This is the top of the dictionary stack.  Generally, adds happen
    // to this stack frame.
    MgUtilDictionary* m_pTopOfDefinitions;

    // Determines if the particular iteration is of interest,
    // as determined by the subset provided.
    // Useful for allowing subset="" handling for any <?EnumXxx?> pi
    // caller provides Subset string (presumably via <?EnumXxx subset="..."?>
    // and maintains iNum count.  For consistency, counts are expected to
    // be 1-based.  Puts the value of iNum into the dictionary under the
    // definition indicated by pszDefinition; if omitted, defaults to "Enum.iteration"
    bool IsIterationInSubset(int iNum,STRING sSubset,CPSZ pszDefinition = NULL);

private:
    // Specifically digests <Template>...</Template>
    bool ProcessTemplateElement(MgXmlParser& Template,CPSZ pszResponse,CPSZ pszSpecificMimeType,CPSZ pszVersion);
    // Specifically digests <Response>...</Response>
    bool ProcessResponseElement(MgXmlParser& Template,CPSZ pszRequest, CPSZ pszSpecificMimeType);

    // Actual recursive work-horse of ProcedureEnumXml.
    void DoEnumXml(MgXmlParser& Fragment,MgXmlNamespaceManager& Namespaces,
                   int& iNum,int iDepth,
                   CREFSTRING sFormat,CREFSTRING sSubset);


    // These are the links to the outside world.  The input (request) and
    // the output (response) objects.
    MgHttpRequestParameters& m_Request;
    CStream*                m_pResponse;


    // This determines whether writing is enabled.
    bool m_bWriteEnabled;
    EscapeState m_eEscapeState;

    // This is the cached version as determined by negotiation.
    STRING m_sNegotiatedVersion;
    STRING m_sValueCache; // temporary cache.

    // Some re-entrancy guards, to prevent bottomless recursion.
    bool m_bBusyEnumeratingDictionary;
    int  m_iExpansionRecursionDepth;


    // Statics...
protected:
    // Given a parser and a dictionary, shove definitions found in the former
    // into the latter.
    static void ProcessDefinitions(MgXmlParser& Template,MgUtilDictionary& Dictionary);

    // Global initialization.  Called by each service instance constructor.
    static bool InitServer(CPSZ pszFilenameGlobalConfig,
                           MgUtilDictionary& GlobalDefinitions,
                           REFSTRING sExceptionTemplate,
                           REFSTRING sExceptionMimeType);
    // Global shutdown.  Called by each service instance destructor.
    static void ShutDownServer();


public:
    // Generic file slurping.  Assumes contents are Unicode (file is a stream of CHARTYPE.)
    // Note: caller owns, and must free(), the returned character-pointer string.
    static VPSZ LoadFile(CPSZ pszFileName);

    // This is the back-end method the outside world can provide
    // to actually do the loading.  See signature, above.
    static void SetLoader(DocLoaderFunc fnDocLoad);

private:

    // Pointer to the document loader function; modified by SetLoader().
    static DocLoaderFunc ms_fnDocLoader;

    // Handles one <Define>...</Define> element in the template or global config.
    // Assumes that the parser is positioned over the <Define> begin element.
    // Used both by the global initialization, as well as each service instance.
    static void ProcessDefine(     MgXmlParser& Template,MgUtilDictionary& Dictionary);

    // Used by global initialization to process the default <Exception> form of response.
    // Used as a fallback in case the service instance doesn't find a <Response> for
    // exceptions.
    static void ProcessDefaultException(MgXmlParser& Template,
                                        REFSTRING sExceptionTemplate,
                                        REFSTRING sExceptionMimeType);
};


#ifdef _WFS_LOGGING
// Temporary logger.  Note that we need separate control over this
// vs the MapAgent Logger, since this might need to be deployed in release
// builds.
class QuickLogger
{
public:
    QuickLogger(CPSZ sName);
    ~QuickLogger();

    void Write(CPSZ sContent = _("\r\n"));
    void Write(STRING sContent);

private:
    STRING m_sName;
};

extern QuickLogger MyLog;
#endif

#endif//_OgcServer_h
