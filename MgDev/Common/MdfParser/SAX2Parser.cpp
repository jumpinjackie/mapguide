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
#include "SAX2Parser.h"
#include "IOMapDefinition.h"
#include "IOVectorLayerDefinition.h"
#include "IODrawingLayerDefinition.h"
#include "IOGridLayerDefinition.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOCompoundSymbolDefinition.h"
#include "IOTileSetDefinition.h"
#include "UnicodeString.h"
#include "PrintLayout/IOPrintLayoutDefinition.h"
#include "PrintLayout/IOPrintLayoutElementDefinition.h"
#include "PrintLayout/IOMapViewportDefinition.h"
#include "IOWatermarkDefinition.h"
#include "IOProfileResult.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


// Dummy class used to automate initialization/uninitialization of Xerces.
class CInitXerces
{
public:
    CInitXerces()
    {
        XMLPlatformUtils::Initialize();
    }
    ~CInitXerces()
    {
        XMLPlatformUtils::Terminate();
    }
};

static CInitXerces s_InitXerces;


SAX2Parser::SAX2Parser() : DefaultHandler()
{
    m_parser       = NULL;
    m_handlerStack = NULL;
    m_tagOpen      = false;

    Flush();
    Initialize();
}


SAX2Parser::~SAX2Parser()
{
    delete m_handlerStack;
    delete m_parser;

    delete m_map;
    delete m_vLayer;
    delete m_dLayer;
    delete m_gLayer;
    delete m_sSymbol;
    delete m_cSymbol;
    delete m_printLayout;
    delete m_mapViewport;
    delete m_watermark;
    delete m_profileResult;
}


void SAX2Parser::Flush()
{
    m_version = Version();
    m_map = NULL;
    m_vLayer = NULL;
    m_dLayer = NULL;
    m_gLayer = NULL;
    m_sSymbol = NULL;
    m_cSymbol = NULL;
    m_printLayout = NULL;
    m_mapViewport = NULL;
    m_watermark = NULL;
    m_profileResult = NULL;
    m_succeeded = false;
}


void SAX2Parser::Initialize()
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


void SAX2Parser::DisableTabs()
{
    MgTab::disableTabs();
}


void SAX2Parser::error(const SAXException& exc)
{
    char* str = XMLString::transcode(exc.getMessage());
    m_strParserError = toMdfString(str);
    XMLString::release(&str);
}


// Returns a reference to the parser's map definition.
// After this call the parser no longer owns the object.
MapDefinition* SAX2Parser::DetachMapDefinition()
{
    MapDefinition* ret = m_map;
    m_map = NULL;
    return ret;
}


// Returns a reference to the parser's print layout definition
// After this call the parser no longer owns the object.
PrintLayoutDefinition* SAX2Parser::DetachPrintLayoutDefinition()
{
    PrintLayoutDefinition* ret = m_printLayout;
    m_printLayout = NULL;
    return ret;
}


// Returns a reference to the parser's map viewport element definition
// After this call the parser no longer owns the object.
MapViewportDefinition* SAX2Parser::DetachMapViewportDefinition()
{
    MapViewportDefinition* ret = m_mapViewport;
    m_mapViewport = NULL;
    return ret;
}


// Returns a reference to the parser's print layout element definition
// After this call the parser no longer owns the object.
PrintLayoutElementDefinition* SAX2Parser::DetachPrintLayoutElementDefinition()
{
    if (m_mapViewport)
        return DetachMapViewportDefinition();
    return NULL;
}


// Returns a reference to the parser's vector layer definition
// After this call the parser no longer owns the object.
VectorLayerDefinition* SAX2Parser::DetachVectorLayerDefinition()
{
    VectorLayerDefinition* ret = m_vLayer;
    m_vLayer = NULL;
    return ret;
}


// Returns a reference to the parser's drawing layer definition
// After this call the parser no longer owns the object.
DrawingLayerDefinition* SAX2Parser::DetachDrawingLayerDefinition()
{
    DrawingLayerDefinition* ret = m_dLayer;
    m_dLayer = NULL;
    return ret;
}


// Returns a reference to the parser's grid layer definition
// After this call the parser no longer owns the object.
GridLayerDefinition* SAX2Parser::DetachGridLayerDefinition()
{
    GridLayerDefinition* ret = m_gLayer;
    m_gLayer = NULL;
    return ret;
}


// Returns a reference to the parser's layer definition
// After this call the parser no longer owns the object.
LayerDefinition* SAX2Parser::DetachLayerDefinition()
{
    if (m_vLayer)
        return DetachVectorLayerDefinition();

    if (m_dLayer)
        return DetachDrawingLayerDefinition();

    if (m_gLayer)
        return DetachGridLayerDefinition();

    return NULL;
}


// Returns a reference to the parser's simple symbol definition
// After this call the parser no longer owns the object.
SimpleSymbolDefinition* SAX2Parser::DetachSimpleSymbolDefinition()
{
    SimpleSymbolDefinition* ret = m_sSymbol;
    m_sSymbol= NULL;
    return ret;
}


// Returns a reference to the parser's compound symbol definition
// After this call the parser no longer owns the object.
CompoundSymbolDefinition* SAX2Parser::DetachCompoundSymbolDefinition()
{
    CompoundSymbolDefinition* ret = m_cSymbol;
    m_cSymbol= NULL;
    return ret;
}


// Returns a reference to the parser's symbol definition
// After this call the parser no longer owns the object.
SymbolDefinition* SAX2Parser::DetachSymbolDefinition()
{
    if (m_sSymbol)
        return DetachSimpleSymbolDefinition();

    if (m_cSymbol)
        return DetachCompoundSymbolDefinition();

    return NULL;
}


// Returns a reference to the parser's watermark definition
// After this call the parser no longer owns the object.
WatermarkDefinition* SAX2Parser::DetachWatermarkDefinition()
{
    WatermarkDefinition* ret = m_watermark;
    m_watermark = NULL;
    return ret;
}


// Returns a reference to the parser's profile result
// After this call the parser no longer owns the object.
ProfileResult* SAX2Parser::DetachProfileResult()
{
    ProfileResult* ret = m_profileResult;
    m_profileResult = NULL;
    return ret;
}

TileSetDefinition* SAX2Parser::DetachTileSetDefinition()
{
    TileSetDefinition* ret = m_tileset;
    m_tileset = NULL;
    return ret;
}


bool SAX2Parser::GetSucceeded() const
{
    return m_succeeded;
}


void SAX2Parser::ParseFile(std::string str)
{
    // reset the version
    m_version = Version();

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


void SAX2Parser::ParseFile(char* str)
{
    // reset the version
    m_version = Version();

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


void SAX2Parser::ParseString(const char* str, size_t numBytes)
{
    // reset the version
    m_version = Version();

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


void SAX2Parser::ParseString(const wchar_t* str, size_t numChars)
{
    // reset the version
    m_version = Version();

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


void SAX2Parser::WriteToFile(std::string name,
                             MapDefinition* map,
                             VectorLayerDefinition* vLayer,
                             DrawingLayerDefinition* dLayer,
                             GridLayerDefinition* gLayer,
                             PrintLayoutDefinition* printLayout,
                             MapViewportDefinition* mapViewport,
                             Version* version)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if (fd.is_open())
    {
        MgTab tab;
        fd << tab.tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl; // NOXLATE

        if (NULL != map)
            IOMapDefinition::Write(fd, map, version, tab);
        else if (NULL != vLayer)
            IOVectorLayerDefinition::Write(fd, vLayer, version, tab);
        else if (NULL != dLayer)
            IODrawingLayerDefinition::Write(fd, dLayer, version, tab);
        else if (NULL != gLayer)
            IOGridLayerDefinition::Write(fd, gLayer, version, tab);
        else if (NULL != printLayout)
            IOPrintLayoutDefinition::Write(fd, printLayout, version, tab);
        else if (NULL != mapViewport)
            IOMapViewportDefinition::Write(fd, mapViewport, version, tab);
    }
    fd.close();
}


void SAX2Parser::WriteToFile(std::string name, SymbolDefinition* symbol, Version* version)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if (fd.is_open())
    {
        MgTab tab;
        fd << tab.tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl; // NOXLATE

        SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbol);
        CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbol);

        if (NULL != simpleSymbol)
            IOSimpleSymbolDefinition::Write(fd, simpleSymbol, true, version, tab);
        else if (NULL != compoundSymbol)
            IOCompoundSymbolDefinition::Write(fd, compoundSymbol, true, version, tab);
    }
    fd.close();
}


void SAX2Parser::WriteToFile(std::string name, WatermarkDefinition* watermark, Version* version)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if (fd.is_open())
    {
        MgTab tab;
        fd << tab.tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl; // NOXLATE

        if (NULL != watermark)
            IOWatermarkDefinition::Write(fd, watermark, version, tab);
    }
    fd.close();
}


void SAX2Parser::WriteToFile(std::string name, ProfileResult* profileResult, Version* version)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if (fd.is_open())
    {
        MgTab tab;
        fd << tab.tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl; // NOXLATE

        if (NULL != profileResult)
            IOProfileResult::Write(fd, profileResult, version, tab);
    }
    fd.close();
}


std::string SAX2Parser::SerializeToXML(MapDefinition* map, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != map)
        IOMapDefinition::Write(fd, map, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(PrintLayoutDefinition* printLayout, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != printLayout)
        IOPrintLayoutDefinition::Write(fd, printLayout, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(PrintLayoutElementDefinition* printLayoutElem, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    MapViewportDefinition* mapViewport = dynamic_cast<MapViewportDefinition*>(printLayoutElem);

    if (NULL != mapViewport)
        IOMapViewportDefinition::Write(fd, mapViewport, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(LayerDefinition* layer, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    VectorLayerDefinition* vectorLayer = dynamic_cast<VectorLayerDefinition*>(layer);
    DrawingLayerDefinition* drawingLayer = dynamic_cast<DrawingLayerDefinition*>(layer);
    GridLayerDefinition* gridLayer = dynamic_cast<GridLayerDefinition*>(layer);

    if (NULL != vectorLayer)
        IOVectorLayerDefinition::Write(fd, vectorLayer, version, tab);
    else if (NULL != drawingLayer)
        IODrawingLayerDefinition::Write(fd, drawingLayer, version, tab);
    else if (NULL != gridLayer)
        IOGridLayerDefinition::Write(fd, gridLayer, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(SymbolDefinition* symbol, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbol);
    CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbol);

    if (NULL != simpleSymbol)
        IOSimpleSymbolDefinition::Write(fd, simpleSymbol, true, version, tab);
    else if (NULL != compoundSymbol)
        IOCompoundSymbolDefinition::Write(fd, compoundSymbol, true, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(WatermarkDefinition* watermark, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != watermark)
        IOWatermarkDefinition::Write(fd, watermark, version, tab);

    return fd.str();
}


std::string SAX2Parser::SerializeToXML(ProfileResult* profileResult, Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != profileResult)
        IOProfileResult::Write(fd, profileResult, version, tab);

    return fd.str();
}

std::string SAX2Parser::SerializeToXML(TileSetDefinition* tileset, MdfModel::Version* version)
{
    MdfStringStream fd;
    MgTab tab;

    if (NULL != tileset)
        IOTileSetDefinition::Write(fd, tileset, version, tab);

    return fd.str();
}

void SAX2Parser::startElement(const XMLCh* const uri,
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
        if (str == L"MapDefinition") // NOXLATE
        {
            // set the version
            SetMapDefinitionVersion(attributes);

            _ASSERT(m_map == NULL); // otherwise we leak
            m_map = new MapDefinition(L"", L"");
            IOMapDefinition* IO = new IOMapDefinition(m_map, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"LayerDefinition") // NOXLATE
        {
            // just set the version
            SetLayerDefinitionVersion(attributes);
        }
        else if (str == L"VectorLayerDefinition") // NOXLATE
        {
            _ASSERT(m_vLayer == NULL);  // otherwise we leak
            m_vLayer = new VectorLayerDefinition(L"", L"");
            IOVectorLayerDefinition* IO = new IOVectorLayerDefinition(m_vLayer, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"DrawingLayerDefinition") // NOXLATE
        {
            _ASSERT(m_dLayer == NULL);  // otherwise we leak
            m_dLayer = new DrawingLayerDefinition(L"", L"");
            IODrawingLayerDefinition* IO = new IODrawingLayerDefinition(m_dLayer, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"GridLayerDefinition") // NOXLATE
        {
            _ASSERT(m_gLayer == NULL);  // otherwise we leak
            m_gLayer = new GridLayerDefinition(L"");
            IOGridLayerDefinition* IO = new IOGridLayerDefinition(m_gLayer, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"SimpleSymbolDefinition") // NOXLATE
        {
            // set the version
            SetSymbolDefinitionVersion(attributes);

            _ASSERT(m_sSymbol == NULL); // otherwise we leak
            m_sSymbol = new SimpleSymbolDefinition();
            IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(m_sSymbol, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"CompoundSymbolDefinition") // NOXLATE
        {
            // set the version
            SetSymbolDefinitionVersion(attributes);

            _ASSERT(m_cSymbol == NULL); // otherwise we leak
            m_cSymbol = new CompoundSymbolDefinition();
            IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(m_cSymbol, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"PrintLayoutDefinition") // NOXLATE
        {
            // set the version
            SetPrintLayoutDefinitionVersion(attributes);

            _ASSERT(NULL == m_printLayout); // otherwise we leak
            m_printLayout = new PrintLayoutDefinition();
            IOPrintLayoutDefinition* IO = new IOPrintLayoutDefinition(m_printLayout, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"PrintLayoutElementDefinition") // NOXLATE
        {
            // just set the version
            SetPrintLayoutElementDefinitionVersion(attributes);
        }
        else if (str == L"MapViewportDefinition") // NOXLATE
        {
            _ASSERT(m_mapViewport == NULL); // otherwise we leak
            m_mapViewport = new MapViewportDefinition();
            IOMapViewportDefinition* IO = new IOMapViewportDefinition(m_mapViewport, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"WatermarkDefinition") //NOXLATE
        {
            // set the version
            SetWatermarkDefinitionVersion(attributes);

            _ASSERT(m_watermark == NULL); // otherwise we leak
            m_watermark = new WatermarkDefinition();
            IOWatermarkDefinition* IO = new IOWatermarkDefinition(m_watermark, m_version);
            m_handlerStack->push(IO);
            IO->StartElement(str.c_str(), m_handlerStack);
        }
        else if (str == L"TileSetDefinition") // NOXLATE
        {
            // set the version
            SetTileSetDefinitionVersion(attributes);

            _ASSERT(m_map == NULL); // otherwise we leak
            m_tileset = new TileSetDefinition();
            IOTileSetDefinition* IO = new IOTileSetDefinition(m_tileset, m_version);
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


void SAX2Parser::characters(const XMLCh* const chars,
                            const XMLSize_t length)
{
    // The character data may be split into multiple calls, so just store it for now.
    // Also, do not record text outside of start/end tags
    if (m_tagOpen)
        m_strbuffer += X2W(chars);
}


void SAX2Parser::endElement(const XMLCh* const uri,
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


const MdfString& SAX2Parser::GetErrorMessage()
{
    return m_strParserError;
}


const MdfModel::Version& SAX2Parser::GetVersion()
{
    return m_version;
}


void SAX2Parser::SetMapDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the latest schema map definition elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"1.0.0") == 0)
            m_version = MdfModel::Version(1, 0, 0);
        else if (_wcsicmp(version.c_str(), L"2.3.0") == 0)
            m_version = MdfModel::Version(2, 3, 0);
        else if (_wcsicmp(version.c_str(), L"2.4.0") == 0)
            m_version = MdfModel::Version(2, 4, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 4, 0);
    }
}


void SAX2Parser::SetLayerDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the schema layer definition elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"1.0.0") == 0)
            m_version = MdfModel::Version(1, 0, 0);
        else if (_wcsicmp(version.c_str(), L"1.1.0") == 0)
            m_version = MdfModel::Version(1, 1, 0);
        else if (_wcsicmp(version.c_str(), L"1.2.0") == 0)
            m_version = MdfModel::Version(1, 2, 0);
        else if (_wcsicmp(version.c_str(), L"1.3.0") == 0)
            m_version = MdfModel::Version(1, 3, 0);
        else if (_wcsicmp(version.c_str(), L"2.3.0") == 0)
            m_version = MdfModel::Version(2, 3, 0);
        else if (_wcsicmp(version.c_str(), L"2.4.0") == 0)
            m_version = MdfModel::Version(2, 4, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 4, 0);
    }
}


void SAX2Parser::SetSymbolDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the schema symbol definition root elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"1.0.0") == 0)
            m_version = MdfModel::Version(1, 0, 0);
        else if (_wcsicmp(version.c_str(), L"1.1.0") == 0)
            m_version = MdfModel::Version(1, 1, 0);
        else if (_wcsicmp(version.c_str(), L"2.4.0") == 0)
            m_version = MdfModel::Version(2, 4, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 4, 0);
    }
}


void SAX2Parser::SetPrintLayoutDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    if (verValue)
    {
        std::wstring version = X2W(verValue);
        if (_wcsicmp(version.c_str(), L"2.0.0") == 0)
            m_version = MdfModel::Version(2, 0, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 0, 0);
    }
}


void SAX2Parser::SetPrintLayoutElementDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    if (verValue)
    {
        std::wstring version = X2W(verValue);
        if (_wcsicmp(version.c_str(), L"2.0.0") == 0)
            m_version = MdfModel::Version(2, 0, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 0, 0);
    }
}


void SAX2Parser::SetWatermarkDefinitionVersion(const Attributes& attributes)
{
    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the schema watermark definition elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"2.3.0") == 0)
            m_version = MdfModel::Version(2, 3, 0);
        else if (_wcsicmp(version.c_str(), L"2.4.0") == 0)
            m_version = MdfModel::Version(2, 4, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 4, 0);
    }
}


void SAX2Parser::SetProfileResultVersion(const Attributes& attributes)
{
    // Although right now we only have 2.4.0 here, this function is still
    // needed for future expansion.

    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the schema profile result elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"2.4.0") == 0)
            m_version = MdfModel::Version(2, 4, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(2, 4, 0);
    }
}

void SAX2Parser::SetTileSetDefinitionVersion(const Attributes& attributes)
{
    // Although right now we only have 3.0.0 here, this function is still
    // needed for future expansion.

    // check for a version attribute
    int index = attributes.getIndex(W2X(L"version"));
    const XMLCh* verValue = (index >= 0)? attributes.getValue(index) : NULL;

    // according to the schema profile result elements require a version
    // attribute, but users may generate XML which is missing this attribute
    if (verValue)
    {
        std::wstring version = X2W(verValue);

        if (_wcsicmp(version.c_str(), L"3.0.0") == 0)
            m_version = MdfModel::Version(3, 0, 0);
    }
    else
    {
        // assume the latest version if the attribute is missing
        m_version = MdfModel::Version(3, 0, 0);
    }
}


MapDefinition* SAX2Parser::CreateClone(MapDefinition* map)
{
    _ASSERT(NULL != map);
    if (NULL == map)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfMD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfMD.append(parser.SerializeToXML(map, NULL));
    parser.ParseString(xmlOfMD.c_str(), xmlOfMD.size());

    return parser.DetachMapDefinition();
}


PrintLayoutDefinition* SAX2Parser::CreateClone(PrintLayoutDefinition* printLayout)
{
    _ASSERT(NULL != printLayout);
    if (NULL == printLayout)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfMD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfMD.append(parser.SerializeToXML(printLayout, NULL));
    parser.ParseString(xmlOfMD.c_str(), xmlOfMD.size());

    return parser.DetachPrintLayoutDefinition();
}


PrintLayoutElementDefinition* SAX2Parser::CreateClone(PrintLayoutElementDefinition* printLayoutElem)
{
    _ASSERT(NULL != printLayoutElem);
    if (NULL == printLayoutElem)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfMD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfMD.append(parser.SerializeToXML(printLayoutElem, NULL));
    parser.ParseString(xmlOfMD.c_str(), xmlOfMD.size());

    return parser.DetachPrintLayoutElementDefinition();
}


LayerDefinition* SAX2Parser::CreateClone(LayerDefinition* layer)
{
    _ASSERT(NULL != layer);
    if (NULL == layer)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfLD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfLD.append(parser.SerializeToXML(layer, NULL));
    parser.ParseString(xmlOfLD.c_str(), xmlOfLD.size());

    return parser.DetachLayerDefinition();
}


SymbolDefinition* SAX2Parser::CreateClone(SymbolDefinition* symbol)
{
    _ASSERT(NULL != symbol);
    if (NULL == symbol)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfSD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfSD.append(parser.SerializeToXML(symbol, NULL));
    parser.ParseString(xmlOfSD.c_str(), xmlOfSD.size());

    return parser.DetachSymbolDefinition();
}


WatermarkDefinition* SAX2Parser::CreateClone(WatermarkDefinition* watermark)
{
    _ASSERT(NULL != watermark);
    if (NULL == watermark)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfWD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfWD.append(parser.SerializeToXML(watermark, NULL));
    parser.ParseString(xmlOfWD.c_str(), xmlOfWD.size());

    return parser.DetachWatermarkDefinition();
}


ProfileResult* SAX2Parser::CreateClone(ProfileResult* profileResult)
{
    _ASSERT(NULL != profileResult);
    if (NULL == profileResult)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfWD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfWD.append(parser.SerializeToXML(profileResult, NULL));
    parser.ParseString(xmlOfWD.c_str(), xmlOfWD.size());

    return parser.DetachProfileResult();
}


TileSetDefinition* SAX2Parser::CreateClone(TileSetDefinition* tileset)
{
    _ASSERT(NULL != tileset);
    if (NULL == tileset)
        return NULL;

    SAX2Parser parser;
    std::string xmlOfTD("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");   // NOXLATE
    xmlOfTD.append(parser.SerializeToXML(tileset, NULL));
    parser.ParseString(xmlOfTD.c_str(), xmlOfTD.size());

    return parser.DetachTileSetDefinition();
}
