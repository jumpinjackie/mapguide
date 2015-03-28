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
#include "FSDSAX2Parser.h"
#include "IOFeatureSource.h"
#include "UnicodeString.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


FSDSAX2Parser::FSDSAX2Parser() : DefaultHandler()
{
    m_parser       = NULL;
    m_handlerStack = NULL;
    m_tagOpen      = false;

    Flush();
    Initialize();
}


FSDSAX2Parser::~FSDSAX2Parser()
{
    delete m_handlerStack;
    delete m_parser;
    delete m_featureSource;
}


void FSDSAX2Parser::Flush()
{
    m_featureSource = NULL;
    m_succeeded = false;
}


void FSDSAX2Parser::Initialize()
{
    m_handlerStack = new HandlerStack();
    m_parser = XMLReaderFactory::createXMLReader();

    m_parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false); // true for validation
    m_parser->setFeature(XMLUni::fgXercesSchema, true);
    m_parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
    m_parser->setFeature(XMLUni::fgSAX2CoreValidation, false); // true for validation
    m_parser->setContentHandler(this);
    m_parser->setErrorHandler(this);
    m_strbuffer = L"";
}


void FSDSAX2Parser::DisableTabs()
{
    MgTab::disableTabs();
}


void FSDSAX2Parser::error(const SAXException& exc)
{
    char* str = XMLString::transcode(exc.getMessage());
    m_strParserError = toMdfString(str);
    XMLString::release(&str);
}


// Returns a reference to the parser's feature source.
// After this call the parser no longer owns the object.
FeatureSource* FSDSAX2Parser::DetachFeatureSource()
{
    FeatureSource* ret = m_featureSource;
    m_featureSource = NULL;
    return ret;
}


bool FSDSAX2Parser::GetSucceeded() const
{
    return m_succeeded;
}


void FSDSAX2Parser::ParseFile(std::string str)
{
    std::string s;
    std::ifstream ifs(str.c_str());
    ifs >> s;
    ifs.close();

    // Need a better test, probably one for the version, but for our
    // current purposes, this one will suffice.
    if (s != std::string("<?xml")) // NOXLATE
    {
        m_succeeded = false;
    }
    else
    {
        try
        {
            m_parser->parse(str.c_str());
            m_succeeded = true;
        }
        catch (const SAXException& exc)
        {
            error(exc);
            m_succeeded = false;
        }
    }
}


void FSDSAX2Parser::ParseFile(char* str)
{
    try
    {
        m_parser->parse(str);
        m_succeeded = true;
    }
    catch (const SAXException& exc)
    {
        error(exc);
        m_succeeded = false;
    }
}


void FSDSAX2Parser::ParseString(const char* str, size_t numBytes)
{
    MemBufInputSource memBufIS((const XMLByte*)str,
                               (const unsigned int)numBytes,
                               "MdfParse", // NOXLATE
                               false);
    try
    {
        m_parser->parse(memBufIS);
        m_succeeded = true;
    }
    catch (const SAXException& exc)
    {
        error(exc);
        m_succeeded = false;
    }
}


void FSDSAX2Parser::ParseString(const wchar_t* str, size_t numChars)
{
    std::string s;
    try
    {
        UnicodeString::WideCharToMultiByte(str, s);
    }
    catch (int)
    {
    }

    ParseString(s.c_str(), s.size());
}


void FSDSAX2Parser::WriteToFile(std::string name, FeatureSource* featureSource, Version* version)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if (fd.is_open())
    {
        MgTab tab;
        fd << tab.tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl; // NOXLATE

        if (NULL != featureSource)
            IOFeatureSource::Write(fd, featureSource, version, tab);
    }
    fd.close();
}


std::string FSDSAX2Parser::SerializeToXML(FeatureSource* featureSource, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != featureSource)
        IOFeatureSource::Write(fd, featureSource, version, tab);

    return fd.str();
}


void FSDSAX2Parser::startElement(const XMLCh* const uri,
                                 const XMLCh* const localname,
                                 const XMLCh* const qname,
                                 const Attributes&  attributes)
{
    std::wstring str = X2W(qname);
    m_strbuffer = L"";   // discard any text between start tags
    m_tagOpen = true;

    // If the stack is empty, then check to see if we've encountered the
    // start of one of the below definitions. Allocate space for the object
    // we will be creating, create the appropriate IO object, passing in the
    // reference to the object. Then push that IO object on top of the stack,
    // to handle further parsing events. Then call the StartElement function
    // of that IO object to initialize it.
    if (m_handlerStack->empty())
    {
        if (str == L"FeatureSource") // NOXLATE
        {
            // set the version
            Version version(1, 0, 0);

            _ASSERT(NULL == m_featureSource); // otherwise we leak
            m_featureSource = new FeatureSource();
            IOFeatureSource* IO = new IOFeatureSource(m_featureSource, version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
    }
    // Otherwise, if the stack has items on it, just pass the event through.
    else
    {
        (m_handlerStack->top())->StartElement(str.c_str(), m_handlerStack);
    }
}


void FSDSAX2Parser::characters(const XMLCh* const chars,
                               const XMLSize_t length)
{
    // The character data may be split into multiple calls, so just store it for now.
    // Also, do not record text outside of start/end tags
    if (m_tagOpen)
        m_strbuffer += X2W(chars);
}


void FSDSAX2Parser::endElement(const XMLCh* const uri,
                               const XMLCh* const localname,
                               const XMLCh* const qname)
{
    m_tagOpen = false;
    if (!m_handlerStack->empty())
    {
        // Now is the time to make the ElementChars() call.
        if (!m_strbuffer.empty())
        {
            (m_handlerStack->top())->ElementChars(m_strbuffer.c_str());
             m_strbuffer = L"";
        }

        (m_handlerStack->top())->EndElement(X2W(qname), m_handlerStack);
    }
}


const MdfString& FSDSAX2Parser::GetErrorMessage()
{
    return m_strParserError;
}


FeatureSource* FSDSAX2Parser::CreateClone(FeatureSource* featureSource)
{
    _ASSERT(NULL != featureSource);
    if (NULL == featureSource)
        return NULL;

    FSDSAX2Parser parser;
    std::string xmlOfFS("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfFS.append(parser.SerializeToXML(featureSource, NULL));
    parser.ParseString(xmlOfFS.c_str(), xmlOfFS.size());

    return parser.DetachFeatureSource();
}
