//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "OgcServer.h"

#include "XmlElementEmitter.h"
#include "XmlParser.h"

#include <algorithm>// temporary mechanism to overcome case sensitivity in the outside world.

#include "StringStream.h"

// WMS spec says these are case-insensitive.  (Research says lowercase is more readable.)
CPSZ kpszQueryStringService         = _("service");
CPSZ kpszQueryStringRequest         = _("request");
CPSZ kpszQueryStringVersion         = _("version");
CPSZ kpszQueryStringWmtVersion         = _("wmtver");
CPSZ kpszQueryStringFormat          = _("format");
CPSZ kpszQueryStringCrs             = _("crs");
CPSZ kpszQueryStringSrs             = _("srs");
CPSZ kpszQueryStringBBox            = _("bbox");

// WMS spec says these values are case-specific.
CPSZ kpszQueryValueGetCapabilities  = _("GetCapabilities");

// Mime types.  Case Specific
CPSZ kpszMimeTypeXml                = _("text/xml");
CPSZ kpszMimeTypeHtml               = _("text/html");
CPSZ kpszMimeTypePlain              = _("text/plain");

// Xml element names.  Case Specific
CPSZ kpszElementConfiguration       = _("Configuration");

CPSZ kpszElementNameTemplate        = _("Template");
CPSZ kpszAttributeService           = _("service");
CPSZ kpszAttributeRequest           = _("request");
CPSZ kpszAttributeVersion           = _("version");

CPSZ kpszElementDefinitions         = _("Definitions");
CPSZ kpszElementDefine              = _("Define");
CPSZ kpszAttributeItem              = _("item");

CPSZ kpszElementResponse            = _("Response");
CPSZ kpszAttributeContentType       = _("content-type");

CPSZ kpszElementVersion             = _("Version");
CPSZ kpszAttributeNumber            = _("number");

CPSZ kpszElementTranslate           = _("translate");
CPSZ kpszAttributeFrom              = _("from");

CPSZ kpszDefinitionParameterPrefix  = _("Parameter.");
CPSZ kpszDefinitionValueMapSuffix   = _(".ValueMap");

CPSZ kpszException                  = _("Exception");

// Xml predefined entities.  Case specific.
CPSZ kpszEntityLessThan             = _("lt");
CPSZ kpszEntityGreaterThan          = _("gt");
CPSZ kpszEntityAmpersand            = _("amp");
CPSZ kpszEntityQuote                = _("quot");
CPSZ kpszEntityApostrophe           = _("apos");
CPSZ kpszEntityLessThanFullSyntax   = _("&lt;");
CPSZ kpszEntityGreaterThanFullSyntax= _("&gt;");
CPSZ kpszExpansionPrefix            = _("&");
CPSZ kpszExpansionSuffix            = _(";");

// Definitions created by various operations.
CPSZ kpszDictionarySupportedVersions = _("SupportedVersions");
CPSZ kpszDictionaryTemplateVersion   = _("TemplateVersion");
CPSZ kpszDictionaryEnumDictName      = _("EnumDictionary.Name");
CPSZ kpszDictionaryEnumDictValue     = _("EnumDictionary.Value");
CPSZ kpszDefinitionInitServerError   = _("InitServer.Error");
CPSZ kpszDefinitionInitServerFile    = _("InitServer.File");
CPSZ kpszDefinitionExceptionType     = _("Exception.Type");
CPSZ kpszDefinitionExceptionMessage  = _("Exception.Message");
CPSZ kpszDefinitionExceptionElement  = _("Exception.Element");
CPSZ kpszDefinitionExceptionContents = _("Exception.Contents");
CPSZ kpszDefinitionEnumItem          = _("Enum.item");
CPSZ kpszDefinitionEnumItemPrefix    = _("Enum.item.");
CPSZ kpszDefinitionEnumIteration     = _("Enum.iteration");

CPSZ kpszDefinitionRequestPassword   = _("Request.PASSWORD");
CPSZ kpszOmittedValue                = _("(omitted)"); // if any dictionary value needs to be omitted...

CPSZ kpszEmpty                      = _("");
CPSZ kpszNewline                    = _("\n");

// BEGIN LOCALIZATION --
CPSZ kpszDefaultExceptionTemplate   = _("<html><head><title>Server Exception</title></head><body><h1>Server Exception:</h1><p>Type: &Exception.Type;</p><p>Message: &Exception.Message;</p><pre><?EnumDictionary?></pre></body></html>");
CPSZ kpszInternalErrorNoSupportedVersions = _("Service Config file does not contain any SupportedVersions definition; unable to perform version negotiation.");
CPSZ kpszInternalErrorNoTemplateFile      = _("Template file was expected but not found in the expected location.");
CPSZ kpszInternalErrorBadServiceAttribute = _("Template file should be attributed with the correct service= attribute");
CPSZ kpszInternalErrorBadVersionAttribute = _("Template file should be attributed with the correct version= attribute");
CPSZ kpszInternalErrorConfigLoadFail = _("LoadFile failed to load global configuration file");
CPSZ kpszPiErrorUnknownPiPrefix      = _("<!-- Unknown Processing Instruction ");
CPSZ kpszPiErrorUnknownPiSuffix      = _(" -->");
// END LOCALIZATION

//  Filenames and file op args.
CPSZ kpszFilenameTemplateSuffix     = _(".xml");


CPSZ kpszElementNameServiceException = _("ServiceException");
CPSZ kpszAttributeValueInternalError = _("InternalError");
CPSZ kpszAttributeTypeFormat         = _("type='%s'");

CPSZ kpszDefinitionPrefixRequest     = _("Request.");

SZBUF kpszBOM[2] = { 0xfeff, 0 }; // Byte Order Mark, indicating UCS-2/UTF-16
CPSZ kpszXmlProcessingInstruction    = _("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"); // encoding=\"UTF-16\"
#ifdef _DEBUG
CPSZ kpszDebugVersionRegress         = _("REGRESS");
#endif


CPSZ kpszPiEnum                      = _("Enum");
CPSZ kpszPiEnumDictionary            = _("EnumDictionary");
CPSZ kpszPiEnumDelim                 = _("EnumDelim");
CPSZ kpszPiEscape                    = _("Escape");
CPSZ kpszPiIf                        = _("If");
CPSZ kpszPiIfdef                     = _("Ifdef");
CPSZ kpszPiEndif                     = _("Endif");
CPSZ kpszPiElse                      = _("Else");
CPSZ kpszPiTranslate                 = _("Translate");

CPSZ kpszPiIfAttrLeft                = _("l");
CPSZ kpszPiIfAttrOp                  = _("op");
CPSZ kpszPiIfAttrRight               = _("r");

// The supported relational operators for the <?If?> PI
CPSZ kpszPiIfAttrOpEq                = _("eq");
CPSZ kpszPiIfAttrOpEqi               = _("eqi");
CPSZ kpszPiIfAttrOpNe                = _("ne");
CPSZ kpszPiIfAttrOpNei               = _("nei");
CPSZ kpszPiIfAttrOpLt                = _("lt");
CPSZ kpszPiIfAttrOpLti               = _("lti");
CPSZ kpszPiIfAttrOpGt                = _("gt");
CPSZ kpszPiIfAttrOpGti               = _("gti");
CPSZ kpszPiIfAttrOpLe                = _("le");
CPSZ kpszPiIfAttrOpLei               = _("lei");
CPSZ kpszPiIfAttrOpGe                = _("ge");
CPSZ kpszPiIfAttrOpGei               = _("gei");

CPSZ kpszPiAttributeText             = _("text"); // indicates a free-form subject string
CPSZ kpszPiAttributeList             = _("list"); // indicates a list, in whatever form is indicated by the PI
CPSZ kpszPiAttributeUsing            = _("using");// indicates a formatting string, used by all enumerators
CPSZ kpszPiAttributeWith             = _("with"); // used by translate to indicate the translation table to use
CPSZ kpszPiAttributeSep              = _("sep");  // indicates a separator
CPSZ kpszPiAttributeSubset           = _("subset"); // indicates which subset of all iterated values to report


CPSZ kpszPiDefaultFormat             = _("&Enum.item;\n"); // When using= is omitted, this is frequently the default
CPSZ kpszPiDefaultSeparator          = _(",");             // When sep= is omitted, this is the default
CPSZ kpszPiDefaultDictionaryFormat   = _("&EnumDictionary.Name;=&EnumDictionary.Value;\n");
                                                           // When using is omitted in a dictionary enumeration, ...

CPSZ kpszVersionRediculouslyHighVersion = _("9999.9.9"); // If no version is provided, we "ask" for this (to get
                                                         // the highest version possible.

DocLoaderFunc MgOgcServer::ms_fnDocLoader = NULL;


// The central point of contact in request processing. This method
// calls virtual methods that must be implemented by derived classes
// to retrieve data required for validation and response generation.
bool MgOgcServer::ProcessRequest(IMgOgcDataAccessor* dataAccessor)
{
    if(dataAccessor != NULL)
    {
        dataAccessor->AcquireValidationData(this);
    }

    bool bValid = ValidateRequest();
    if(bValid)
    {
        if(dataAccessor != NULL)
        {
            dataAccessor->AcquireResponseData(this);
        }
        RespondToRequest();
    }
    return bValid;
}

void MgOgcServer::SetLoader(DocLoaderFunc fnDocLoad)
{
    ms_fnDocLoader = fnDocLoad;
}


class MgOgcInternalException: public MgOgcException
{
public:
    MgOgcInternalException(CPSZ pszMessage)
    : MgOgcException(kpszElementNameServiceException,kpszAttributeValueInternalError,pszMessage)
    {
    }

    void WriteTo(CStream& Response) const
    {
        SZBUF szTypeAttribute[64];
        // Rudimentary buffer overrun check.
        ASSERT(szlen(this->Type()) + 8 /* "type=''" + EOS */ < char_sizeof(szTypeAttribute));

        szsprintf(szTypeAttribute,sizeof(szTypeAttribute),kpszAttributeTypeFormat,this->Type());

        MgXmlElementEmitter Error(Response,this->ElementName(),szTypeAttribute);
        if(this->Message() != NULL)
            Error.Write(this->Message());
    }

};


// Static global initialization.
bool MgOgcServer::InitServer(CPSZ pszFilenameGlobalConfig,
                             MgUtilDictionary& GlobalDefinitions,
                             REFSTRING sExceptionTemplate,
                             REFSTRING sExceptionMimeType)
{

    if(GlobalDefinitions.Count() == 0) {
        VPSZ pszConfig = LoadFile(pszFilenameGlobalConfig);
        if(pszConfig != NULL) {

            MgXmlParser GlobalConfig(pszConfig);
            GlobalConfig.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

            GlobalConfig.Next(); // Advance to first item.

            MgXmlSynchronizeOnElement Root(GlobalConfig,kpszElementConfiguration);
            if(Root.AtBegin()) {
                ProcessDefinitions(GlobalConfig,GlobalDefinitions);
                ProcessDefaultException(GlobalConfig,sExceptionTemplate,sExceptionMimeType);
            }

            // If no Exception type and/or template were discovered,
            // use a hard-coded default.  Not pretty, but functional.
            if(sExceptionMimeType.length() == 0)
              sExceptionMimeType = kpszMimeTypeHtml;
            if(sExceptionTemplate.length() == 0)
              sExceptionTemplate = kpszDefaultExceptionTemplate;

            return true;
        }
        else {
            // Just a note about what happened.
            GlobalDefinitions.AddDefinition(kpszDefinitionInitServerError,kpszInternalErrorConfigLoadFail);
            GlobalDefinitions.AddDefinition(kpszDefinitionInitServerFile,pszFilenameGlobalConfig);
            // Bare bones Exception support.
            sExceptionMimeType = kpszMimeTypeHtml;
            sExceptionTemplate = kpszDefaultExceptionTemplate;
        }
    }
    return false;
}

// Static global shutdown
void MgOgcServer::ShutDownServer()
{
#if 0
#define _VERIFYING_ZERO_MEMORY_LEAKS
#ifdef _VERIFYING_ZERO_MEMORY_LEAKS
  if(ms_GlobalDefinitions.Count() > 0) {
      ms_GlobalDefinitions.Empty();
  }
  // STL doesn't seem to free the buffers of statics
  // until d'tor time (which if it comes, is after main
  // exits....)
  ms_sExceptionMimeType.clear();
  ms_sExceptionTemplate.clear();
#endif
#endif
}


MgOgcServer::MgOgcServer(MgHttpRequestParameters& Request,
                         CStream& Response,
                         MgUtilDictionary& rGlobalDefinitions)
: m_pTopOfDefinitions(&rGlobalDefinitions)
, m_Request(Request)
, m_pResponse(&Response)
, m_bBusyEnumeratingDictionary(false)
, m_iExpansionRecursionDepth(32)
, m_bWriteEnabled(true)
, m_bEscaping(false)
{
    //InitServer();

    // Create our first local stack frame.
    PushDictionary();

    // Copy all the request variables into the dictionary,
    // They will have the names Request.Xxx
    for(int i=0;i<Request.Count();i++) {
        CPSZ pszName  = Request.Name(i);
        CPSZ pszValue = (CPSZ)Request.Value(i);
        STRING sName = kpszDefinitionPrefixRequest;
        sName += pszName;
        this->m_pTopOfDefinitions->AddDefinition(sName.c_str(),pszValue);
    }
}

MgOgcServer::~MgOgcServer()
{
    // Remove the first stack frame created in the ctor.
    PopDictionary();

    ShutDownServer();
}



// Declare the content type that the response will have.
void MgOgcServer::SetContentType(CPSZ pszMimeType)
{
    m_pResponse->SetContentType(pszMimeType);

    // Centralized help: write out the XML declarative PI.
    if(SZ_EQ(pszMimeType,kpszMimeTypeXml)) {
      // removed Write(kpszBOM);
      Write(kpszXmlProcessingInstruction);
    }
}






// Processes the default exception, in the form of an <Exception> element
// from the global configuration file.  This represents a complete fallback
// as each version file should probably have a <Response request="Exception">
// element defined.
void MgOgcServer::ProcessDefaultException(MgXmlParser& Template,
                                          REFSTRING sExceptionTemplate,
                                          REFSTRING sExceptionMimeType)
{
    MgXmlSynchronizeOnElement ExceptionElement(Template,kpszException);
    MgXmlBeginElement* pBegin;

    LONGBITS dwOptions = Template.Options();
    Template.SetOptions(keNone);

    if(ExceptionElement.AtBegin(&pBegin)) {
        if(!pBegin->GetAttribute(kpszAttributeContentType,sExceptionMimeType))
            sExceptionMimeType = kpszMimeTypeXml;

        Template.Next();

        while(!ExceptionElement.AtEnd()) {
            sExceptionTemplate += Template.Current().Contents();
            Template.Next();
        }
    }

    Template.SetOptions(dwOptions);
}


//
bool MgOgcServer::DefaultExceptionResponse()
{
    // Gets the template info for default exception, when no other exception type is at hand.
    STRING sExceptionTemplate;
    STRING sExceptionMimeType;
    GetDefaultExceptionInfo(sExceptionTemplate,sExceptionMimeType);

    ASSERT(sExceptionMimeType.length() > 0);
    ASSERT(sExceptionTemplate.length() > 0);
    SetContentType(sExceptionMimeType.c_str());

    // Linux doesn't seem to like unnamed objects called directly as an
    // argument, so we have to name it before the call...
    MgXmlParser TempName(sExceptionTemplate.c_str());
    ProcessXmlStream(TempName);

    return true;
}




void MgOgcServer::DefineExceptionValues(const MgOgcException& Exception)
{
    m_pTopOfDefinitions->AddDefinition(kpszDefinitionExceptionType,Exception.Type());
    m_pTopOfDefinitions->AddDefinition(kpszDefinitionExceptionMessage,Exception.Message());
    m_pTopOfDefinitions->AddDefinition(kpszDefinitionExceptionElement,Exception.ElementName());
    CStringStream Out;
    Exception.WriteTo(Out);
    m_pTopOfDefinitions->AddDefinition(kpszDefinitionExceptionContents,Out.Contents());
}


bool MgOgcServer::ExceptionResponse(MgXmlParser& Template,const MgOgcException& Exception)
{
    CDictionaryStackFrame(this);

    DefineExceptionValues(Exception);

    // Let's see if the template file has any guidance
    while(Template.Next()) {
      if(ProcessResponseElement(Template,kpszException,NULL))
        return true;
    }

    return true;
}


void MgOgcServer::ProcessDefine(MgXmlParser& Template,MgUtilDictionary& Dictionary)
{
    // This ensures we enter pointing at <Define> and leave pointing past </Define>
    MgXmlSynchronizeOnElement Define(Template,kpszElementDefine);

    // Let's examine the Define element's attributes.
    MgXmlBeginElement* pBegin;
    if(Define.AtBegin(&pBegin)) {
        STRING sName;
        pBegin->GetAttribute(kpszAttributeItem,sName);
        Template.Next();

        STRING sDefinition;
        while(!Define.AtEnd()) {
            sDefinition += Template.Current().Contents();
            Template.Next();
        }

        Dictionary.AddDefinition(sName,sDefinition);
    }
}

void MgOgcServer::ProcessDefinitions(MgXmlParser& Template,MgUtilDictionary& Dictionary)
{
    // This ensures we enter this method pointing at <Definitions> and
    // that we leave pointing past </Definitions>
    MgXmlSynchronizeOnElement Definitions(Template,kpszElementDefinitions);

    LONGBITS dwOptions = Template.Options();
    Template.SetOptions(keNone);

    // If we're where we expect to be...
    if(Definitions.AtBegin()) {
        // Note: Since we didn't ask AtBegin to provide us the Xml Begin node
        // It has conveniently done a Next() over it; we're now within the node.
        // Loop until we're at the end of the element.
        while(!Definitions.AtEnd()) {
            // If we've encountered a <Define> element, process it
            if(Template.Current().Type() == keBeginElement) {
                MgXmlBeginElement& Begin = (MgXmlBeginElement&)Template.Current();
                if(Begin.Name() == kpszElementDefine)
                    ProcessDefine(Template,Dictionary);
                    // Note: upon return, Template.Current() is past </Define>
                    // so no need for Next().
                else
                    Template.Next();
            }
            else
                Template.Next();
        }
    }

    Template.SetOptions(dwOptions);
}

// TODO: use Synch mechanism?
void MgOgcServer::ProcessXmlStream(MgXmlParser& Xml)
{
    while(Xml.Next()) {
        switch(Xml.Current().Type()) {
        case keBeginElement:
            {
                MgXmlBeginElement& Begin = (MgXmlBeginElement&)Xml.Current();
                // Print ourselves (Attributes may contain expansions)
                ProcessExpandableText(Begin.Contents());
                // And if necessary,
                if(!Begin.IsEmpty()) {
                    // Print our contents, then
                    ProcessXmlStream(Xml);
                    // Print our </end> element -- Recursion has advanced
                    // us to Xml.Current().Type() == keEndElement.
                    ProcessExpandableText(Xml.Current().Contents());
                }
            }
            break;

        case keProcessingInstruction:
            ProcessInstruction((MgXmlProcessingInstruction&)Xml.Current());
            break;

        case keText:
            ProcessExpandableText(Xml.Current().Contents());
            break;

        case keComment:
            ProcessExpandableText(Xml.Current().Contents());
            break;

        default:
            Write(Xml.Current().Contents());
            break;

        case keEndElement:
            // The processing of the end element is handled in the outer context.
            return;
        }
    }
}


// This takes a Processing Instruction and decides what to do with it
void MgOgcServer::ProcessInstruction(MgXmlProcessingInstruction& PI)
{
    STRING sProc = PI.Name();

    if(sProc == kpszPiEnum)
        ProcedureEnum(PI);
    else if(sProc == kpszPiEnumDictionary)
        ProcedureEnumDictionary(PI);
    else if(sProc == kpszPiEnumDelim)
        ProcedureEnumDelim(PI);
    else if(sProc == kpszPiEscape)
        ProcedureEscape(PI);
    else if(sProc == kpszPiIf)
        ProcedureIf(PI);
    else if(sProc == kpszPiIfdef)
        ProcedureIfdef(PI);
    else if(sProc == kpszPiEndif)
        ProcedureEndif(PI);
    else if(sProc == kpszPiElse)
        ProcedureElse(PI);
    else if(sProc == kpszPiTranslate)
        ProcedureTranslate(PI);
#ifdef _DEBUG
    else if(sProc == _("Diagnostic")) // NOXLATE
        ProcedureDiagnostic(PI);
#endif
    else if(!ProcessOtherInstruction(sProc,PI)) {
        Write(kpszPiErrorUnknownPiPrefix);
        Write(PI.Contents());
        Write(kpszPiErrorUnknownPiSuffix);
    }
}


#ifdef _DEBUG
// This debug-only PI is used for REGRESS testing of things that might
// otherwise be inaccessible.
void MgOgcServer::ProcedureDiagnostic(MgXmlProcessingInstruction& PIDiag)
{
    STRING sTest;
    if(!PIDiag.GetAttribute(_("test"),sTest)) // NOXLATE -- debug only
        return;

    STRING sParam;
    if(!PIDiag.GetAttribute(_("param"),sParam)) // NOXLATE -- debug only
        return;

    if(sTest == _("version")) { // NOXLATE -- debug only
        CPSZ pszVersion = NegotiatedVersion(sParam.c_str());
        ProcessExpandableText(pszVersion);
    }
}
#endif


// We have nothing to do here; derivations override and add to this.
// thunking up to their base for completeness.
bool MgOgcServer::ProcessOtherInstruction(CREFSTRING /*sProc*/,MgXmlProcessingInstruction& /*PI*/)
{
    return false;
}


void MgOgcServer::ProcedureIf(MgXmlProcessingInstruction& PIIf)
{
    // Get the Left-hand-side
    STRING sLeft;
    if(!PIIf.GetAttribute(kpszPiIfAttrLeft,sLeft))
        return;

    // Get the Relational Operator
    STRING sOp;
    if(!PIIf.GetAttribute(kpszPiIfAttrOp,sOp))
        return;

    // Get the Right-hand-side
    STRING sRight;
    if(!PIIf.GetAttribute(kpszPiIfAttrRight,sRight))
        return;

    // Evaluate the strings
    ProcessExpandableTextIntoString(sLeft, sLeft);
    ProcessExpandableTextIntoString(sRight,sRight);

    if(sOp == kpszPiIfAttrOpEq)
        this->m_bWriteEnabled = (sLeft == sRight);
    else if(sOp == kpszPiIfAttrOpNe)
        this->m_bWriteEnabled = (sLeft != sRight);
    else if(sOp == kpszPiIfAttrOpLt)
        this->m_bWriteEnabled = (sLeft <  sRight);
    else if(sOp == kpszPiIfAttrOpGt)
        this->m_bWriteEnabled = (sLeft >  sRight);
    else if(sOp == kpszPiIfAttrOpLe)
        this->m_bWriteEnabled = (sLeft <= sRight);
    else if(sOp == kpszPiIfAttrOpGe)
        this->m_bWriteEnabled = (sLeft >= sRight);
    else if(sOp == kpszPiIfAttrOpEqi)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str()) == 0;
    else if(sOp == kpszPiIfAttrOpNei)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str()) != 0;
    else if(sOp == kpszPiIfAttrOpLti)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str())  < 0;
    else if(sOp == kpszPiIfAttrOpGti)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str())  > 0;
    else if(sOp == kpszPiIfAttrOpLei)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str()) <= 0;
    else if(sOp == kpszPiIfAttrOpGei)
        this->m_bWriteEnabled = szicmp(sLeft.c_str(),sRight.c_str()) >= 0;
    else // sOp == "eq#" -- numerical comparison?
        return;
}

void MgOgcServer::ProcedureIfdef(MgXmlProcessingInstruction& PIIf)
{
    // Get the Left-hand-side
    STRING sName;
    if(!PIIf.GetAttribute(kpszAttributeItem,sName))
        return;

    CPSZ pszDef = Definition(sName.c_str());
    this->m_bWriteEnabled = (pszDef != NULL && szlen(pszDef) > 0);
}

void MgOgcServer::ProcedureElse(MgXmlProcessingInstruction& PIElse)
{
    m_bWriteEnabled = !m_bWriteEnabled;
}

void MgOgcServer::ProcedureEndif(MgXmlProcessingInstruction& PIEndif)
{
    m_bWriteEnabled = true;
}


void MgOgcServer::ProcedureEscape(MgXmlProcessingInstruction& PIEscape)
{
    bool bOldEscapeState = m_bEscaping;
    m_bEscaping = true;

    STRING sText;
    if(PIEscape.GetAttribute(kpszPiAttributeText,sText))
        ProcessExpandableText(sText);

    m_bEscaping = bOldEscapeState;
}

// Interpret a <?Enum list= using= ?> processing instruction
void MgOgcServer::ProcedureEnum(MgXmlProcessingInstruction& PIEnum)
{
    STRING sList;
    if(!PIEnum.GetAttribute(kpszPiAttributeList,sList))
        return;

    // Okay, by default, we look for <item> elements and take
    // their value.  But the PI might have an item="somethingelse"
    // declared, in which case we'll look for <somethingelse> and
    // take that value.  Note that any attributes we discover are also
    // pushed into the dictionary.
    STRING sItemElement;
    if(!PIEnum.GetAttribute(kpszAttributeItem,sItemElement))
        sItemElement = kpszAttributeItem;
    ProcessExpandableTextIntoString(sItemElement,sItemElement);

    CPSZ pszItem = sItemElement.c_str();

    STRING sExpandedList;
    ProcessExpandableTextIntoString(sList,sExpandedList);

    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;

    MgXmlParser List(sExpandedList.c_str());
    List.Next(); // advance to beginning.

    while(List.More()) {
        if(List.Current().Type() == keBeginElement) {
            MgXmlBeginElement& Begin = (MgXmlBeginElement&)List.Current();
            if(Begin.Name() == pszItem) {
                // Each item/iteration gets its own stack frame, so
                // definitions from one iteration don't carry forth
                // into the next.
                CDictionaryStackFrame ForProcedureVariables(this);

                if(IsIterationInSubset(++iNum,sSubset)) {
                    if(Begin.HasAttributes()) {
                        MgXmlAttribute& Attr = Begin.Attributes();
                        while(Attr.Next()) {
                            STRING sName = Attr.Name();
                            STRING sValue = Attr.Value();
                            sName = kpszDefinitionEnumItemPrefix + sName;
                            m_pTopOfDefinitions->AddDefinition(sName,sValue);
                        }
                    }

                    MgXmlSynchronizeOnElement Item(List,pszItem);
                    STRING sItem;

                    if(Item.AtBegin()) {
                        while(!Item.AtEnd()) {
                            sItem += List.Current().Contents();
                            List.Next();
                        }

                        m_pTopOfDefinitions->AddDefinition(kpszDefinitionEnumItem,sItem.c_str());

                    } // if Item.AtBegin();
                    else
                        m_pTopOfDefinitions->AddDefinition(kpszDefinitionEnumItem,_(""));

                    ProcessExpandableText(sFormat);
                } // if it's a subset of interest
                else // it's not one of the subset= items.
                    List.Next(); // skip it.

            } // if Begin.Name() == "item" (or requested substitute)
            else // it's not one of the items named by item=
                List.Next(); // skip it.
        } // if Type is Begin
        else // not an <item> beginning
            List.Next(); // skip it.
    } // While more tokens in the list.
}


// For now, the accepted syntax is num,num,...
// though eventually it would be nice to have open or closed
// ranges, as in [-num,](num-num|num),...[,num-]
bool MgOgcServer::IsIterationInSubset(int iNum,STRING sSubset,CPSZ pszDefinition)
{
    // Stringify the number; we almost always need this.
    SZBUF szInteger[32];
    szsprintf(szInteger,sizeof(szInteger),_("%d"),iNum);

    // Let's be optimistic.  Will it be in the set?  Probably.
    bool bInclude = true;

    // If there's a constraint, we need to look at it.
    if(sSubset.length() > 0) {
        sSubset=_(",")+sSubset+_(",");

        STRING sNum = _(",");
        sNum += szInteger;
        sNum +=_(",");
        STRING::size_type iDebug = sSubset.find(sNum);
        bInclude = (iDebug != STRING::npos);
    }

    if(bInclude) {
        m_pTopOfDefinitions->AddDefinition(pszDefinition? pszDefinition : kpszDefinitionEnumIteration,szInteger);
        return true;
    }

    return false;

}

// This enumerates a delimited string, using the specified separator, or a comma.
void MgOgcServer::ProcedureEnumDelim(MgXmlProcessingInstruction& PIEnum)
{
    //
    STRING sDebugContents = PIEnum.Contents();

    STRING sList;
    if(!PIEnum.GetAttribute(kpszPiAttributeList,sList))
        return; // empty string... do nothing.
    // do a lookup against it.

    this->ProcessExpandableTextIntoString(sList,sList);

    STRING sSep;
    if(!PIEnum.GetAttribute(kpszPiAttributeSep,sSep))
        sSep = kpszPiDefaultSeparator;

    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;   // iteration count.
    STRING::size_type iSep = STRING::npos;  // last known position of separator; before start of string initially
    STRING::size_type iItem = 0;            // start of current item; at start of string initially.
    while((iSep = sList.find(sSep,iItem)) != STRING::npos ) {
        // Each item/iteration gets its own stack frame, so
        // definitions from one iteration don't carry forth
        // into the next.
        CDictionaryStackFrame ForProcedureVariables(this);

        STRING sItem = sList.substr(iItem,iSep - iItem);
        if(IsIterationInSubset(++iNum,sSubset)) {
            m_pTopOfDefinitions->AddDefinition(kpszDefinitionEnumItem,sItem.c_str());
            ProcessExpandableText(sFormat);
        }
        iItem = iSep+1;
    }
    {
        CDictionaryStackFrame ForProcedureVariables(this);
        // Now, one final "iteration" since there's always a dangler after the last separator.
        if(IsIterationInSubset(++iNum,sSubset)) {
            m_pTopOfDefinitions->AddDefinition(kpszDefinitionEnumItem,sList.substr(iItem).c_str());
            ProcessExpandableText(sFormat);
        }
    }
}

// This enumerates the "dictionary" -- the entire, nested structure.
void MgOgcServer::ProcedureEnumDictionary(MgXmlProcessingInstruction& PIEnum)
{
    // In order to avoid recursion, we simply print ourselves out
    // if we're already busy enumerating something.
    if(m_bBusyEnumeratingDictionary) {
        Write(PIEnum.Contents());
        return;
    }

    m_bBusyEnumeratingDictionary = true;
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiDefaultDictionaryFormat;

    int iOldExpansionRecursionDepth = m_iExpansionRecursionDepth;
    m_iExpansionRecursionDepth = 2;

    // Traverse the dictionary stack
    MgUtilDictionary* pCurrentDictionary = m_pTopOfDefinitions;

    // This pushes an invisible stack frame, which will not
    // appear in the dump; it avoids separate logic that attempts
    // to skip our own two definitions, avoiding a nasty bit of
    // Goedelian self-reference.
    CDictionaryStackFrame ToAvoidHavingOurDefinitionsShow(this);

    while(pCurrentDictionary != NULL) {
        int iCount = pCurrentDictionary->Count();
        for(int i=0; i < iCount; i++) {
            CPSZ pszName  =       pCurrentDictionary->Name(i);
            CPSZ pszValue = (CPSZ)pCurrentDictionary->Value(i);
            if(SZ_EQI(pszName,kpszDefinitionRequestPassword))
                pszValue = kpszOmittedValue; // must suppress the password.
            m_pTopOfDefinitions->AddDefinition(kpszDictionaryEnumDictName, pszName);
            m_pTopOfDefinitions->AddDefinition(kpszDictionaryEnumDictValue,pszValue);

            ProcessExpandableText(sFormat);
        }
        pCurrentDictionary = pCurrentDictionary->NextScope();
    }

    m_bBusyEnumeratingDictionary = false;
    m_iExpansionRecursionDepth = iOldExpansionRecursionDepth;
}


void MgOgcServer::ProcedureTranslate(MgXmlProcessingInstruction &PITranslate)
{
    // Missing arg?
    STRING sString;
    if(!PITranslate.GetAttribute(kpszPiAttributeText,sString))
        return; // TODO: issue an internal error?

    // Missing arg?
    STRING sMapDef;
    if(!PITranslate.GetAttribute(kpszPiAttributeWith,sMapDef))
        return; // TODO: issue an internal error?

    // Arg doesn't point to a valid definition?
    ProcessExpandableTextIntoString(sMapDef,sMapDef);
    CPSZ pszMapDef = sMapDef.c_str();
    if(pszMapDef == NULL)
        return; // TODO: issue an internal error?

    // We're here, now we can do some work.
    STRING sStringExpanded; // Expanded equivalent of our input string.
    STRING sMappedResults;
    // Let's evaluate the string to resolve any embedded
    // definitions that need expanding.
    ProcessExpandableTextIntoString(sString,sStringExpanded);


    // Now, using the translation map we've been given,
    // let's do the mapping lookup.
    MgXmlParser MapDefinitions(pszMapDef);
    MapValue(MapDefinitions,sStringExpanded.c_str(),sMappedResults);

    // Process the results back to the outer stream.
    ProcessExpandableText(sMappedResults);
}


void MgOgcServer::ProcessExpandableTextIntoString(CPSZ pszText,REFSTRING sOut)
{
    CStringStream Str;
    CSubstituteStream ForThisMethod(this,Str);

    ProcessExpandableText(pszText);

    sOut = Str.Contents();
}

void MgOgcServer::ProcessExpandableTextIntoString(STRING sText,REFSTRING sOut)
{
    CStringStream Str;
    CSubstituteStream ForThisMethod(this,Str);

    ProcessExpandableText(sText);

    sOut = Str.Contents();
}

// Right now, an "expansion" is just an entity, though there's some thought
// about changing this to another notation.
void MgOgcServer::ProcessExpandableText(CPSZ pszTextWithExpansions,int iLength)
{
    int iPos = 0;
    CPSZ pszStart = pszTextWithExpansions;
    while(true) {
        switch(pszStart[iPos]) {
        case '\0':
            Write(pszStart);
            return; // we're done.

        case '&':
            // Write the ordinary text up to this expansion...
            if(iPos > 0)
              Write(pszStart,iPos);
            // Process the expansion.  Note pszStart gets
            // advanced past the expansion to start everything
            // all over again.
            pszStart = ProcessExpansion(pszStart+iPos+1);
            iPos = 0;
            break;

        case '<':
            if(m_bEscaping) {
                Write(pszStart,iPos);
                Write(kpszEntityLessThanFullSyntax);
                pszStart+= iPos+1;
                iPos=0;
            }
            else
                iPos++;
            break;
        case '>':
            if(m_bEscaping) {
                Write(pszStart,iPos);
                Write(kpszEntityGreaterThanFullSyntax);
                pszStart+= iPos+1;
                iPos=0;
            }
            else
                iPos++;
            break;

        default:
            iPos++;
            break;
        }
    }
}

// This extracts the expansion (ie entity) name and returns
// a pointer past its end.  It will have emitted the response
// for this entity.
CPSZ MgOgcServer::ProcessExpansion(CPSZ pszExpansionName)
{
    int iEnd = 0;

    while(true) {
        switch(pszExpansionName[iEnd]) {

        case '\0': // End of string... there aint no more
            Expansion(STRING(pszExpansionName,iEnd));
            return pszExpansionName+iEnd;

        case ';': // Semicolon: normal end of entity.
            Expansion(STRING(pszExpansionName,iEnd));
            return pszExpansionName+iEnd+1;
        }
        iEnd++;
    }
    ASSERT(0); // Should never get here.
}


// This handles generating expansions (which currently take the
// form of entities.  If the expansion does not correspond to
// a known definition, we leave it in-situ, since it might be
// an entity outside of our scope.  We also never allow redefinition
// of the five XML entities.
void MgOgcServer::Expansion(const STRING& sExpansionName)
{
    // First, cope with the XML-defined entity names.  We *never* touch these.
    if(sExpansionName == kpszEntityLessThan  || sExpansionName == kpszEntityGreaterThan ||
       sExpansionName == kpszEntityAmpersand || sExpansionName == kpszEntityQuote       ||
       sExpansionName == kpszEntityApostrophe ) {
        Write(kpszExpansionPrefix);
        Write(sExpansionName);
        Write(kpszExpansionSuffix);
    }
    // Otherwise, it's probably a definition (or an entity, which we can think of
    // as a definition we just don't know about.)
    else  {
        CPSZ pszName = sExpansionName.c_str();
        CPSZ pszValue = Definition(pszName);
        --m_iExpansionRecursionDepth;
        if(pszValue != NULL && m_iExpansionRecursionDepth >= 0) {
            MgXmlParser AvoidInlineReference(pszValue);
            ProcessXmlStream(AvoidInlineReference);
        }
        else { // If we don't understand it, leave it in place -- it might be a real entity.
            Write(kpszExpansionPrefix);
            Write(sExpansionName);
            Write(kpszExpansionSuffix);
        }
        m_iExpansionRecursionDepth++;
    }
}

// This does a "complete" lookup of any "Expansion" aka definition,
// first examining the local (non-static) dictionary,
// and failing that, looks in the static dictionary.
// In this way, the server *instance* inherits the global
// definitions, but can override them.
CPSZ MgOgcServer::Definition(CPSZ pszItem)
{
    if(m_pTopOfDefinitions != NULL)
        return (*m_pTopOfDefinitions)[pszItem];
    else
        return NULL;
    // TODO: verify that
    //else // really, probably an error condition...
    //    return ms_GlobalDefinitions[pszItem];
}


// This is the central entrypoint to template processing.  It will load a version-
// appropriate template file, and attempt to generate a response of the indicated
// mime-type, as driven by the template file.
bool MgOgcServer::GenerateResponse(CPSZ pszResponse,CPSZ pszSpecificMimeType)
{
    CPSZ pszVersion = NegotiatedVersion();
    if(pszVersion == NULL) {
        InternalError(kpszInternalErrorNoSupportedVersions);
        return true; // we generated *a* response, but probably not the one expected ... ;-)
    }

    STRING sFileName(GetTemplatePrefix());
    sFileName += pszVersion;
    sFileName += kpszFilenameTemplateSuffix;

    VPSZ pszTemplate = LoadFile(sFileName.c_str());
    if(pszTemplate == NULL) {
        m_pTopOfDefinitions->AddDefinition(kpszDefinitionInitServerFile,sFileName.c_str());
        InternalError(kpszInternalErrorNoTemplateFile);
        return true; // we generated *a* response, but probably not the one expected ... ;-)
    }

    MgXmlParser Template(pszTemplate); // parser takes ownership of this pointer.
    Template.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    // Advance to the first item in the document.
    Template.Next();

    return ProcessTemplateElement(Template,pszResponse,pszSpecificMimeType,pszVersion);
}

// This is the work-horse of the server.
// This processes the <Template>...</Template> element, making sure that the template
// declares itself as being the desired version.  This then creates any definitions
// encountered, and attempts to locate the specific Response/MimeTipe element.
//
// Returns true if processed (and caller can then go on) or false if nothing
// was accomplished.
bool MgOgcServer::ProcessTemplateElement(MgXmlParser& Template,CPSZ pszResponse,
                                           CPSZ pszSpecificMimeType,CPSZ pszVersion)
{
    MgXmlSynchronizeOnElement TemplateElement(Template,kpszElementNameTemplate);

    MgXmlBeginElement* pTemplate;
    if(TemplateElement.AtBegin(&pTemplate)) {
        STRING sValue;
        // Some integrity checking.  We expect, for example, <Template service="WMS" version="x.y.z">
        if(!pTemplate->GetAttribute(kpszAttributeService,sValue) || sValue != GetServiceType()) {
            InternalError(kpszInternalErrorBadServiceAttribute);
            return true; // we processed something; we're done.  Get out.
        }
        if(!pTemplate->GetAttribute(kpszAttributeVersion,sValue) || sValue != pszVersion) {
            InternalError(kpszInternalErrorBadVersionAttribute);
            return true; // we processed something; we're done.  Get out.
        }

        // Done looking at the <Template>, scoot past it to its contents.
        Template.Next();

        if(!TemplateElement.AtEnd()) {
            // Load up the definitions, if any.
            ProcessDefinitions(Template,*m_pTopOfDefinitions);

            while(!TemplateElement.AtEnd()) {
                if(ProcessResponseElement(Template,pszResponse,pszSpecificMimeType))
                    return true; // we're done...
            }
        }
    }
    return false;
}


// This method processes the <Response> ... </Response> element found
// within a template file.  Assumes that it's positioned over the <Response>
// in the token stream.
bool MgOgcServer::ProcessResponseElement(MgXmlParser& Template,CPSZ pszRequest, CPSZ pszSpecificMimeType)
{
    MgXmlSynchronizeOnElement ResponseElement(Template,kpszElementResponse);

    MgXmlBeginElement* pBegin;
    // We're not even on a <Response> element... what are we doing here?!?!?
    if(!ResponseElement.AtBegin(&pBegin)) {
        // For now, we do this.  If our "schema" ever allows things other than <Response>
        // we won't want to do this here, but instead communicate a richer return
        // and let the outside context decide if advancing to the next token is warranted
        // For now, though, it prevents a hot loop if the file is somehow incorrect.
        Template.Next();
        return false;
    }

    STRING sValue;
    // Not a response to the right kind of request (GetCapabilities vs GetFeatureInfo?)
    if(!pBegin->GetAttribute(kpszAttributeRequest,sValue) || sValue != pszRequest)
        return false;

    // Examine the content-type
    STRING sContentType;
    if(pBegin->GetAttribute(kpszAttributeContentType,sContentType)) {
        // If the caller wants a specific type, and this aint it, we're outa here.
        if(pszSpecificMimeType && sContentType != pszSpecificMimeType)
            return false;
        SetContentType(sContentType.c_str());
    }
    else {
        // If the caller wants a specific type, and this aint it, we're outa here.
        if(pszSpecificMimeType && SZ_NE(pszSpecificMimeType,kpszMimeTypeXml))
            return false;
        SetContentType(kpszMimeTypeXml);
    }

    LONGBITS dwOptions = Template.Options();
    Template.SetOptions(keNone);

    // Okay, we're in the right Response object.
    // Let's spew it out.
    ProcessXmlStream(Template);

    Template.SetOptions(dwOptions);

    // Happy endings are always nice.
    return true;
}



// This slurps a file into memory.
// TODO: Taking it to the Next Step; now that Loading is farmed
// out to the caller, we can change the signature away from a
// returned allocated pointer and towards a referenced caller-provided
// string.
VPSZ MgOgcServer::LoadFile(CPSZ pszFileName)
{
    STRING sRet;
    MgOgcServer::ms_fnDocLoader(pszFileName,sRet);
    STRING::size_type iSizeInBytes = sRet.length() * sizeof(CHARTYPE);  // Nice O(1) bit of info
                                                                        // Can't do that with UTF8!

    // Copy... ( +sizeof(CHARTYPE) == ending null char)
    VPSZ pSlurp = (VPSZ)malloc(iSizeInBytes+sizeof(CHARTYPE));
    memcpy(pSlurp,sRet.c_str(),iSizeInBytes+sizeof(CHARTYPE));

    // Kill the UNICODE Byte Order Mark, if there is one.
    if(pSlurp[0] == 0xfeff) {
        memcpy(pSlurp,pSlurp+1,iSizeInBytes);
        iSizeInBytes -= 2;
    }

    // Make sure the thing is null-terminated.
    STRING::size_type iSizeInChars = iSizeInBytes / sizeof(CHARTYPE);
    pSlurp[iSizeInChars] = 0;

    return pSlurp;
}



// Version negotiation happens here.
CPSZ MgOgcServer::NegotiatedVersion(CPSZ pszRequested)
{
    if(m_sNegotiatedVersion.length() == 0 || pszRequested != NULL) 
    {
        if(pszRequested == NULL)  
        {
            // Read the VERSION parameter
            pszRequested = this->RequestParameter(kpszQueryStringVersion);
            if(pszRequested == NULL)
            {
                // Look for the WMTVER parameter used in older request formats
                pszRequested = this->RequestParameter(kpszQueryStringWmtVersion);
                if(pszRequested == NULL) // still
                {
                    pszRequested = kpszVersionRediculouslyHighVersion;
                }
            }
        }

#ifdef _DEBUG
        // For debug builds only, allow a regression test version.
        if(pszRequested != NULL && SZ_EQ(pszRequested,kpszDebugVersionRegress)) {
            m_sNegotiatedVersion = pszRequested;
            // Provide a default definition for this.
            m_pTopOfDefinitions->AddDefinition(kpszDictionaryTemplateVersion,pszRequested);
            return m_sNegotiatedVersion.c_str();
        }
#endif

        // TODO: global init must define minimal version supprt
        CPSZ pszSupportedVersions = Definition(kpszDictionarySupportedVersions);
        // Pass out a NULL, which caller must use to tickle an exception.
        if(pszSupportedVersions == NULL)
            return NULL;

        MgXmlParser SupportedVersions(pszSupportedVersions);

        STRING sVersion;
        bool bLowest = true;
        while(SupportedVersions.Next()) {
            if(SupportedVersions.Current().Type() == keBeginElement) {
                MgXmlBeginElement& Begin = (MgXmlBeginElement&)SupportedVersions.Current();
                if(Begin.Name() == kpszElementVersion && Begin.GetAttribute(kpszAttributeNumber,sVersion)) {
                    // Note: lexical comparison, not numerical one;
                    // Okay for single-digit subversions; might be a problem
                    // for 1.10.0 vs 1.2.0 -- if there's ever such a thing.
                    if(bLowest) {
                        // OGC spec: "If a version lower than any of those known to the server
                        // then the server shall send the lowest version it supports."
                        if(pszRequested <= sVersion) {
                            m_sNegotiatedVersion = sVersion;
                            break;
                        }
                        bLowest = false;
                    }
                    else {
                        // OGC spec: "If a version unknown to the server and higher than
                        // the lowest supported version, the server shall send the highest
                        // version it supports that is less than the requested version"
                        if(sVersion == pszRequested) {
                            m_sNegotiatedVersion = sVersion;
                            break;
                        }
                        else if(sVersion < pszRequested)
                            m_sNegotiatedVersion = sVersion;
                            // but keep looking/
                    }
                    // else,
                    //     client didn't request a specific version so
                    //     we spin through to find the highest version we support.
                }
            }
        }

        // Still not found?  requested version is too new for us.
        // Tell them about the latest we've got.
        if(m_sNegotiatedVersion.length() == 0)
            m_sNegotiatedVersion = sVersion; // the last version we found.
    }

    m_pTopOfDefinitions->AddDefinition(kpszDictionaryTemplateVersion,m_sNegotiatedVersion.c_str());
    return m_sNegotiatedVersion.c_str();
}



void MgOgcServer::InternalError(CPSZ pszMessage)
{
    CDictionaryStackFrame ForThisException(this);

    DefineExceptionValues(MgOgcInternalException(pszMessage));

    // For internal errors, always just provide the default response
    // We can't afford the risk of opening a template.
    DefaultExceptionResponse();
}



void MgOgcServer::ServiceExceptionReportResponse(const MgOgcException& Exception)
{
    CDictionaryStackFrame ForThisException(this);

    DefineExceptionValues(Exception);

    if(!GenerateResponse(kpszException,NULL))
        DefaultExceptionResponse();
}


// When going into a stack frame...
void MgOgcServer::PushDictionary()
{
    this->m_pTopOfDefinitions = new MgUtilDictionary(this->m_pTopOfDefinitions);
}

void MgOgcServer::PopDictionary()
{
    MgUtilDictionary* pOldTop = m_pTopOfDefinitions;

    m_pTopOfDefinitions = m_pTopOfDefinitions->NextScope();

    delete(pOldTop);
}



bool MgOgcServer::Write(CPSZ pszText)
{
    if(!m_bWriteEnabled)
        return true; // pretend to succeed.
    STRING::size_type uLen = szlen(pszText)*sizeof(CHARTYPE);
    STRING::size_type uWritten;
    m_pResponse->Write(pszText,uLen,&uWritten);
    return uWritten == uLen;
}

bool MgOgcServer::Write(CPSZ pszText,int iChars)
{
    if(!m_bWriteEnabled)
        return true; // pretend to succeed.
    STRING::size_type uLen = (STRING::size_type)iChars * sizeof(CHARTYPE);
    STRING::size_type uWritten;
    m_pResponse->Write(pszText,uLen,&uWritten);
    return uWritten == uLen;
}


bool MgOgcServer::Write(const STRING sText)
{
    return Write(sText.c_str());
}


bool MgOgcServer::WriteLine()
{
    return Write(kpszNewline);
}

// Uses a Map definition to map the From string to the resulting To string.
bool MgOgcServer::MapValue(MgXmlParser& Xml,CPSZ pszFrom,REFSTRING sTo)
{
    while(Xml.Next()) {
        if(Xml.Current().Type() == keBeginElement) {
            MgXmlSynchronizeOnElement MapElement(Xml,kpszElementTranslate);
            MgXmlBeginElement* pBegin;
            // Are we at the beginning of a <translate> element?
            if(MapElement.AtBegin(&pBegin)) {
                STRING sFrom;
                // Does the <map> element have a from attribute, and is it the
                // one we're looking for?
                bool bHasFrom = pBegin->GetAttribute(kpszAttributeFrom,sFrom);
                // If the <map> element has a match, or is a catchall
                if((bHasFrom && sFrom == pszFrom) || !bHasFrom) {
                    // Cool; move into the <map> element's contents and slurp
                    // it up.
                    Xml.Next();
                    while(!MapElement.AtEnd()) {
                        sTo+= Xml.Current().Contents();
                        Xml.Next();
                    }

                    // Stop looking; we've found our mapping.  Go home happy.
                    return true;
                }
            }
        }
    }
    return false;
}


CPSZ MgOgcServer::RequestParameter(CPSZ pszParameter)
{
    // This allows template versions to rename parameters
    // as is the case with, for example, SRS and CRS,
    // WMTVER and VERSION, and so on.
    STRING sName = kpszDefinitionParameterPrefix;
    sName += pszParameter;
    CPSZ pszMappedName = this->Definition(sName.c_str());
    // If there's a local override, we ask for *that* parameter
    // instead.
    if(pszMappedName != NULL)
        pszParameter = pszMappedName;

    // Here's the actual lookup.
    CPSZ pszUnmappedValue = m_Request[pszParameter];
    if(pszUnmappedValue != NULL) {
        // Now, if that parameter has a value map,
        // let's see if the unmapped value has a mapping.
        sName += kpszDefinitionValueMapSuffix;
        CPSZ pszValueMap = this->Definition(sName.c_str());
        if(pszValueMap != NULL) {
            MgXmlParser Xml(pszValueMap);
            if(MapValue(Xml,pszUnmappedValue,m_sValueCache)) {
                return m_sValueCache.c_str();
            }
        }
    }

    return pszUnmappedValue;
}


void MgOgcServer::ProcessExpandableText(STRING sTextWithExpansion)
{
    ProcessExpandableText(sTextWithExpansion.c_str(),(int)sTextWithExpansion.length());
}

void MgOgcServer::ProcessExpandableText(CPSZ pszTextWithExpansion)
{
    ProcessExpandableText(pszTextWithExpansion,(int)szlen(pszTextWithExpansion));
}


MgOgcServer::CDictionaryStackFrame::CDictionaryStackFrame(MgOgcServer* pServer)
: m_pServer(pServer)
{
    m_pServer->PushDictionary();
}

MgOgcServer::CDictionaryStackFrame::~CDictionaryStackFrame()
{
    m_pServer->PopDictionary();
}


MgOgcServer::CSubstituteStream::CSubstituteStream(MgOgcServer* pServer,CStream& StreamNew)
:   m_pServer(      pServer)
,   m_pStreamOld(   pServer->m_pResponse)
,   m_bWriteEnabled(pServer->m_bWriteEnabled)
,   m_bEscaping(    pServer->m_bEscaping)
{
    // Redirect to the new stream
    m_pServer->m_pResponse = &StreamNew;
    // Using default settings.
    m_pServer->m_bEscaping     = false;
    m_pServer->m_bWriteEnabled = true;
}

MgOgcServer::CSubstituteStream::~CSubstituteStream()
{
    // Restore the previous values.
    m_pServer->m_bEscaping     = m_bEscaping;
    m_pServer->m_bWriteEnabled = m_bWriteEnabled;
    // Restore the previous stream.
    m_pServer->m_pResponse     = m_pStreamOld;
    // There, things should be exactly like we left it.
}


#ifdef _WFS_LOGGING

QuickLogger::QuickLogger(CPSZ sName)
: m_sName(sName)
{
    Write(_("--- start of session ---\r\n"));
}


QuickLogger::~QuickLogger()
{
    Write(_("--- end of session ---\r\n"));
}

void QuickLogger::Write(CPSZ pszContent)
{
    //*
    FILE* pFile = szopen(m_sName.c_str(),_("ab"));
    size_t iLen = szlen(pszContent);
    fwrite(pszContent,sizeof(CHARTYPE),iLen,pFile);
    fclose(pFile);
    //*/
}

void QuickLogger::Write(STRING sContent)
{
    Write(sContent.c_str());
}

QuickLogger MyLog(_("tmt_ogc.log"));

#endif//_WFS_LOGGING
