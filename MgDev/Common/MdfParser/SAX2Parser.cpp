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

#include "stdafx.h"
#include "SAX2Parser.h"
#include <assert.h>
#include "IODrawingLayerDefinition.h"
#include "IOVectorLayerDefinition.h"
#include "IOGridLayerDefinition.h"
#include "IOMapDefinition.h"
#include "UnicodeString.h"

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
    m_Parser       = NULL;
    m_HandlerStack = NULL;
    m_tagOpen      = false;
    m_charsInTag   = false;

    Flush();
    Initialize();
}


SAX2Parser::~SAX2Parser()
{
    if (m_HandlerStack != NULL)
        delete m_HandlerStack;
    if (m_Parser != NULL)
        delete m_Parser;
    if (m_Map != NULL)
        delete m_Map;
    if (m_vLayer != NULL)
        delete m_vLayer;
    if (m_dLayer != NULL)
        delete m_dLayer;
    if (m_gLayer != NULL)
        delete m_gLayer;
}


void SAX2Parser::Flush()
{
    m_Map = NULL;
    m_vLayer = NULL;
    m_dLayer = NULL;
    m_gLayer = NULL;
    m_succeeded = false;
}


void SAX2Parser::Initialize()
{
    //static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    m_HandlerStack = new HandlerStack();
    m_Parser = XMLReaderFactory::createXMLReader();

    m_Parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);
    m_Parser->setFeature(XMLUni::fgXercesSchema, true);
    m_Parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
    m_Parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
    m_Parser->setContentHandler(this);
    m_prevOpenTag = L"";
    m_strbuffer = L"";
}


void SAX2Parser::error(const SAXException &exc)
{
    this->m_strParserError = toMdfString(XMLString::transcode(exc.getMessage()));
}


// Returns a reference to the parser's map definition.
// After this call the parser no longer owns the object.
MapDefinition* SAX2Parser::DetachMapDefinition()
{
    MapDefinition* ret = m_Map;
    m_Map = NULL;
    return ret;
}


// Returns a reference to the parser's feature layer definition
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


// Returns a reference to the parser's grid layer definition
// After this call the parser no longer owns the object.
LayerDefinition* SAX2Parser::DetachLayerDefinition()
{
    LayerDefinition* ldef = NULL;

    if (m_vLayer)
    {
        ldef = m_vLayer;
        m_vLayer = NULL;
    }
    else if (m_gLayer)
    {
        ldef = m_gLayer;
        m_gLayer = NULL;
    }
    else if (m_dLayer)
    {
        ldef = m_dLayer;
        m_dLayer = NULL;
    }

    return ldef;
}


bool SAX2Parser::GetSucceeded()
{
    return m_succeeded;
}


void SAX2Parser::ParseFile(std::string str)
{
    std::string s;
    std::ifstream ifs(str.c_str());
    ifs >> s;
    ifs.close();

    //Need a better test, probably one for the version, but for our current purposes, this one will sufice.
    if (s != std::string("<?xml")) // NOXLATE
    {
       m_succeeded = false;
    }
    else
    {
        try
        {
            m_Parser->parse(str.c_str());
            m_succeeded = true;
        }
        catch (...)
        {
            m_succeeded = false;
        }
    }
}


void SAX2Parser::ParseFile(char* str)
{
    try
    {
        m_Parser->parse(str);
        m_succeeded = true;
    }
    catch (...)
    {
        m_succeeded = false;
    }
}


void SAX2Parser::ParseString(const char* str, int numBytes)
{
    MemBufInputSource memBufIS ((const XMLByte*)(str),
                                numBytes,
                                "MdfParse",  // NOXLATE
                                false);
    try
    {
        m_Parser->parse(memBufIS);
        m_succeeded = true;
    }
    catch (...)
    {
        m_succeeded = false;
    }
}


void SAX2Parser::WriteToFile(std::string name,
                             MapDefinition* map,
                             VectorLayerDefinition* vLayer,
                             DrawingLayerDefinition* dLayer,
                             GridLayerDefinition *gLayer)
{
    std::ofstream fd;
    fd.open(name.c_str());
    if(fd.is_open())
    {
        zerotab();
        fd << tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << std::endl; // NOXLATE
        inctab();
        WriteDefinition(fd, map, vLayer, dLayer, gLayer);
    }
    fd.close();
}


std::string SAX2Parser::SerializeToXML(LayerDefinition *pLayer)
{
    MdfStringStream fd;

    VectorLayerDefinition *pVectorLayer = dynamic_cast<VectorLayerDefinition*>(pLayer);
    DrawingLayerDefinition *pDrawingLayer = dynamic_cast<DrawingLayerDefinition*>(pLayer);
    GridLayerDefinition *pGridLayer = dynamic_cast<GridLayerDefinition*>(pLayer);

    if (NULL != pVectorLayer)
    {
        std::auto_ptr<IOVectorLayerDefinition> spIO(new IOVectorLayerDefinition());
        spIO->Write(fd, pVectorLayer);
    }
    else if (NULL != pDrawingLayer)
    {
        std::auto_ptr<IODrawingLayerDefinition> spIO(new IODrawingLayerDefinition());
        spIO->Write(fd, pDrawingLayer);
    }
    else if (NULL != pGridLayer)
    {
        std::auto_ptr<IOGridLayerDefinition> spIO(new IOGridLayerDefinition());
        spIO->Write(fd, pGridLayer);
    }

    return fd.str();
}

std::string SAX2Parser::SerializeToXML(MapDefinition *pMap)
{
    MdfStringStream fd;

    if (NULL != pMap)
    {
        std::auto_ptr<IOMapDefinition> spIO(new IOMapDefinition());
        spIO->Write(fd, pMap);
    }

    return fd.str();
}

void SAX2Parser::WriteDefinition(MdfStream &fd,
                                 MapDefinition* map,
                                 VectorLayerDefinition* vLayer,
                                 DrawingLayerDefinition* dLayer,
                                 GridLayerDefinition *gLayer)
{
    if (map != NULL)
    {
        IOMapDefinition* IO = new IOMapDefinition();
        IO->Write(fd, map);
        delete IO;
    }
    else if (vLayer != NULL)
    {
        IOVectorLayerDefinition* IO = new IOVectorLayerDefinition();
        IO->Write(fd, vLayer);
        delete IO;
    }
    else if (dLayer != NULL)
    {
        IODrawingLayerDefinition* IO = new IODrawingLayerDefinition();
        IO->Write(fd, dLayer);
        delete IO;
    }
    else if (gLayer != NULL)
    {
        IOGridLayerDefinition * IO = new IOGridLayerDefinition();
        IO->Write(fd, gLayer);
        delete IO;
    }
}


void SAX2Parser::startElement(const   XMLCh* const    uri,
                              const   XMLCh* const    localname,
                              const   XMLCh* const    qname,
                              const   Attributes&     attributes)
{
    std::wstring str = X2W(localname);
    m_openTag = str;
    // If the stack is empty, then check to see if we've encountered the
    // start of one of the below definitions. Allocate space for the object
    // we will be creating, create the appropriate IO object, passing in the reference
    // to the object. Then push that IO object on top of the stack, to handle further
    // parsing events. Then call the StartElement function of that IO object to initialize it.
    if (m_HandlerStack->empty())
    {
        if (str == L"MapDefinition") // NOXLATE
        {
            assert(m_Map == NULL);  // otherwise we leak
            m_Map = new MapDefinition(L"", L""); // NOXLATE
            IOMapDefinition* IO = new IOMapDefinition(m_Map);
            m_HandlerStack->push(IO);
            IO->StartElement(str.c_str(), m_HandlerStack);
        }
        else if (str == L"VectorLayerDefinition") // NOXLATE
        {
            assert(m_vLayer == NULL);   // otherwise we leak
            m_vLayer = new VectorLayerDefinition(L"", L""); // NOXLATE
            IOVectorLayerDefinition* IO = new IOVectorLayerDefinition(m_vLayer);
            m_HandlerStack->push(IO);
            IO->StartElement(str.c_str(), m_HandlerStack); // NOXLATE
        }
        else if (str == L"DrawingLayerDefinition") // NOXLATE
        {
            assert(m_dLayer == NULL);   // otherwise we leak
            m_dLayer = new DrawingLayerDefinition(L"", L"");
            IODrawingLayerDefinition* IO = new IODrawingLayerDefinition(m_dLayer);
            m_HandlerStack->push(IO);
            IO->StartElement(str.c_str(), m_HandlerStack);
        }
        else if (str == L"GridLayerDefinition") // NOXLATE
        {
            m_gLayer = new GridLayerDefinition(L"");
            IOGridLayerDefinition * IO = new IOGridLayerDefinition(m_gLayer);
            m_HandlerStack->push(IO);
            IO->StartElement(str.c_str(), m_HandlerStack);
        }
    }
    // Otherwise, if the stack has items on it, just pass the event through.
    else
    {
        (m_HandlerStack->top())->StartElement(str.c_str(), m_HandlerStack);
    }
    m_tagOpen = true;
    m_charsInTag = false;
}


void SAX2Parser::characters(const XMLCh* const    chars,
                            const unsigned int    length)
{
    if (m_openTag.compare(m_prevOpenTag) != 0)
    {
        if (!m_HandlerStack->empty())
        {
            if (m_charsInTag)
                (m_HandlerStack->top())->ElementChars(m_strbuffer.c_str());
            m_strbuffer = L"";
        }
    }

    m_strbuffer += X2W(chars);

    m_prevOpenTag = m_openTag;

    if (m_tagOpen)
        m_charsInTag = true;
}


void SAX2Parser::endElement(const XMLCh* const uri,
                            const XMLCh* const localname,
                            const XMLCh* const qname)
{
    m_tagOpen = false;
    m_prevOpenTag = L"";
    if (!m_HandlerStack->empty())
    {
        (m_HandlerStack->top())->EndElement(X2W(localname), m_HandlerStack);
    }
}


const MdfString& SAX2Parser::GetErrorMessage()
{
    return this->m_strParserError;
}

LayerDefinition* SAX2Parser::GetLayerDefinition() const
{
    if (NULL != m_vLayer)
        return m_vLayer;
    else if (NULL != m_dLayer)
        return m_dLayer;
    else if (NULL != m_gLayer)
        return m_gLayer;

    return NULL;
}

LayerDefinition* SAX2Parser::CreateClone(LayerDefinition *pSourceLD)
{
    _ASSERT(NULL != pSourceLD);
    if (NULL == pSourceLD)
        return NULL;

    SAX2Parser parser;

    std::string xmlOfLD = parser.SerializeToXML(pSourceLD);
    parser.ParseString(xmlOfLD.c_str(), xmlOfLD.size());

    return parser.DetachLayerDefinition();
}

MapDefinition* SAX2Parser::CreateClone(MapDefinition *pSourceMD)
{
    _ASSERT(NULL != pSourceMD);
    if (NULL == pSourceMD)
        return NULL;

    SAX2Parser parser;

    std::string xmlOfMD = parser.SerializeToXML(pSourceMD);
    parser.ParseString(xmlOfMD.c_str(), xmlOfMD.size());

    return parser.DetachMapDefinition();
}
