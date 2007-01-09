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
#include "FSDSAX2Parser.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

#include "IOFeatureSource.h"
#include "UnicodeString.h"

namespace MdfParser
{
    FSDSAX2Parser::FSDSAX2Parser()
    {
        m_tagOpen = false;

        Flush();
        Initialize();
    }

    FSDSAX2Parser::~FSDSAX2Parser()
    {
        if (m_HandlerStack != NULL)
            delete m_HandlerStack;
        if (this->m_Parser != NULL)
            delete this->m_Parser;
        if (this->m_pFeatureSource)
            delete this->m_pFeatureSource;
    }

    void FSDSAX2Parser::Flush()
    {
        this->m_pFeatureSource = NULL;
        this->succeeded = 0;
    }


    void FSDSAX2Parser::Initialize()
    {
        //static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
        m_HandlerStack = new HandlerStack();
        this->m_Parser = XMLReaderFactory::createXMLReader();

        m_Parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);
        m_Parser->setFeature(XMLUni::fgXercesSchema, true);
        m_Parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
        m_Parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
        m_Parser->setContentHandler(this);
        m_Parser->setErrorHandler(this);
        m_strbuffer = L"";
    }

    void FSDSAX2Parser::DisableTabs()
    {
        disableTabs();
    }

    void FSDSAX2Parser::error(const SAXException &exc)
    {
        this->m_strParserError = toMdfString(XMLString::transcode(exc.getMessage()));
    }

    void FSDSAX2Parser::ParseFile(std::string str)
    {
        std::string s;
        std::ifstream ifs(str.c_str());
        ifs >> s;
        ifs.close();

        //Need a better test, probably one for the version, but for our current purposes, this one will sufice.
        if(s != std::string("<?xml")) // NOXLATE
        {
            this->succeeded = false;
        }
        else
        {
            try
            {
                m_Parser->parse(str.c_str());
                this->succeeded = true;
            }
            catch (const SAXException &exc)
            {
                error(exc);
                this->succeeded = false;
            }
        }
    }

    void FSDSAX2Parser::ParseFile(char* str)
    {
        try
        {
            m_Parser->parse(str);
            this->succeeded = true;
        }
        catch (const SAXException &exc)
        {
            error(exc);
            this->succeeded = false;
        }
    }

    void FSDSAX2Parser::ParseString(const char* str, unsigned int numBytes)
    {
        MemBufInputSource memBufIS((const XMLByte*)(str),
                                   numBytes,
                                   "MdfParse",  // NOXLATE
                                   false);
        try
        {
            m_Parser->parse(memBufIS);
            this->succeeded = true;
        }
        catch (const SAXException &exc)
        {
            error(exc);
            this->succeeded = false;
        }
    }

    void FSDSAX2Parser::WriteToFile(std::string name, FeatureSource *pFeatureSource)
    {
        std::ofstream fd;
        fd.open(name.c_str());
        if(fd.is_open())
        {
            zerotab();
            fd << tab() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << std::endl; // NOXLATE
            inctab();
            WriteDefinition(pFeatureSource, fd);
        }
        fd.close();
    }

    std::string FSDSAX2Parser::SerializeToXML(FeatureSource *pFeatureSource)
    {
        MdfStringStream fd;
        WriteDefinition(pFeatureSource, fd);

        return fd.str();
    }

    void FSDSAX2Parser::WriteDefinition(FeatureSource *pFeatureSource, MdfStream &fd)
    {
        if(NULL != pFeatureSource)
        {
            std::auto_ptr<IOFeatureSource> spIO(new IOFeatureSource());
            spIO->Write(fd, pFeatureSource);
        }
    }

    void FSDSAX2Parser::startElement(const   XMLCh* const    uri,
                                     const   XMLCh* const    localname,
                                     const   XMLCh* const    qname,
                                     const   Attributes&        attributes)
    {
        std::wstring str = X2W(localname);
        m_strbuffer = L"";   // discard any text between start tags
        m_tagOpen = true;

        // If the stack is empty, then check to see if we've encountered the
        // start of one of the below definitions. Allocate space for the object
        // we will be creating, create the appropriate IO object, passing in the reference
        // to the object. Then push that IO object on top of the stack, to handle further 
        // parsing events. Then call the StartElement function of that IO object to initialize it.
        if (m_HandlerStack->empty())
        {
            if (str == L"FeatureSource") // NOXLATE
            {
                assert(NULL == this->m_pFeatureSource);
                this->m_pFeatureSource = new FeatureSource(); // NOXLATE
                IOFeatureSource *IO = new IOFeatureSource(this->m_pFeatureSource);
                m_HandlerStack->push(IO);
                IO->StartElement(str.c_str(), m_HandlerStack);
            }
        }
        // Otherwise, if the stack has items on it, just pass the event through.
        else
        {
            (m_HandlerStack->top())->StartElement(str.c_str(), m_HandlerStack);
        }
    }

    void FSDSAX2Parser::characters(const     XMLCh* const    chars
                                    , const   unsigned int    length)
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
        if (!m_HandlerStack->empty())
        {
            // Now is the time to make the ElementChars() call.
            if (!m_strbuffer.empty())
            {
                (m_HandlerStack->top())->ElementChars(m_strbuffer.c_str());
                 m_strbuffer = L"";
            }

            (m_HandlerStack->top())->EndElement(X2W(localname), m_HandlerStack);
        }
    }

    const MdfString& FSDSAX2Parser::GetErrorMessage()
    {
        return this->m_strParserError;
    }

    FeatureSource* FSDSAX2Parser::CreateClone(FeatureSource *pFeatureSource)
    {
        _ASSERT(NULL != pFeatureSource);
        if (NULL == pFeatureSource)
            return NULL;

        FSDSAX2Parser parser;
        std::string xmlOfFS = parser.SerializeToXML(pFeatureSource);
        parser.ParseString(xmlOfFS.c_str(), (unsigned int)xmlOfFS.size());

        return parser.DetachFeatureSource();
    }
    
    FeatureSource* FSDSAX2Parser::DetachFeatureSource()
    {
        FeatureSource* ret = m_pFeatureSource;
        m_pFeatureSource = NULL;

        return ret;
    }
}
