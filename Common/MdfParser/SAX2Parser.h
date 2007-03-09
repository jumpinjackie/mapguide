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

#ifndef _SAX2PARSER_H
#define _SAX2PARSER_H

#include "MapDefinition.h"
#include "VectorLayerDefinition.h"
#include "DrawingLayerDefinition.h"
#include "GridLayerDefinition.h"
#include "SimpleSymbolDefinition.h"
#include "CompoundSymbolDefinition.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include "IOUtil.h"
#include "SAX2ElementHandler.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

/* SAX2Parser is the main class for the parser. It holds the actual
 * Xerces SAX2 Parser and is also the handler for the events generated
 * by the parser. It holds the main references to the map, feature layer,
 * or drawing layer that will be created by the parse. It handles both
 * reading and writing through public functions.
*/
class MDFPARSER_API SAX2Parser : public DefaultHandler
{
    protected:
        // The objects to be generated by the parse.
        MapDefinition* m_Map;
        VectorLayerDefinition* m_vLayer;
        DrawingLayerDefinition* m_dLayer;
        GridLayerDefinition* m_gLayer;
        SimpleSymbolDefinition* m_sSymbol;
        CompoundSymbolDefinition* m_cSymbol;

        // Succeeded is true if the parse has succeeded. As of now,
        // there are very loose constraints on this boolean.
        bool m_succeeded;

        SAX2XMLReader* m_Parser;

        // handlerStack is a stack of SAX2ElementHandlers (see the
        // typedef in SAX2ElementHandler.h). When the parser receives
        // events they will be routed to the SAX2ElementHandler on the top
        // of the handlerStack, which will be an instance of one of the IO classes,
        // who all implement the SAX2ElementHandler interface.
        HandlerStack* m_HandlerStack;

        // Unfortunately, the SAX2 Parser doesn't necessarily send all the
        // characters that appear inside a tag in one call of characters()-
        // for example with escape characters it separates them into separate calls.
        // Therefore, m_strbuffer is used to accumulate the characters.  tagOpen is
        // used in the case when you have characters outside a tag (i.e. <Metadata></Metadata>)
        // to make sure that whitespace or line returns don't get assigned to it instead.
        std::wstring m_strbuffer;
        bool m_tagOpen;

        MdfString m_strParserError;

        void WriteDefinition(MdfStream &fd,
                             MapDefinition* map,
                             VectorLayerDefinition* vLayer,
                             DrawingLayerDefinition* dLayer,
                             GridLayerDefinition *gLayer);

        // error handling override
        virtual void error(const SAXException &exc);

    public:
        SAX2Parser();
        ~SAX2Parser();
        void Flush();
        void Initialize();
        void DisableTabs();
        void ParseFile(std::string str);
        void ParseFile(char* str);
        void ParseString(const char* str, int numBytes);
        void WriteToFile(std::string name,
                         MapDefinition *map,
                         VectorLayerDefinition *vLayer,
                         DrawingLayerDefinition *dLayer,
                         GridLayerDefinition *gLayer);
        void WriteToFile(std::string name, SymbolDefinition* pSymbol);

        std::string SerializeToXML(MapDefinition *pMap);
        std::string SerializeToXML(LayerDefinition *pLayer);
        std::string SerializeToXML(LayerDefinition *pLayer, MdfModel::Version *version);
        std::string SerializeToXML(SymbolDefinition *pSymbol);

        // there are two modes for retrieving objects created by parsing:

        // detaches the most recently parsed object from the parser.
        // the calling method is then responsible for disposing the object.
        // the parser no longer has access to this object (set to NULL).
        MapDefinition* DetachMapDefinition();
        VectorLayerDefinition* DetachVectorLayerDefinition();
        DrawingLayerDefinition* DetachDrawingLayerDefinition();
        GridLayerDefinition* DetachGridLayerDefinition();
        LayerDefinition* DetachLayerDefinition();
        SimpleSymbolDefinition* DetachSimpleSymbolDefinition();
        CompoundSymbolDefinition* DetachCompoundSymbolDefinition();
        SymbolDefinition* DetachSymbolDefinition();

        // DEPRECATED - these methods should not be used and should be removed.
        // they return the last parsed object of a given type, but ownership of
        // the object is retained by the parser (it should not be disposed by other code).
        MapDefinition *GetMapDefinition() { return this->m_Map; }
        LayerDefinition *GetLayerDefinition() const;
        VectorLayerDefinition *GetVectorLayerDefinition() { return this->m_vLayer; }
        DrawingLayerDefinition *GetDrawingLayerDefinition() { return this->m_dLayer; }
        GridLayerDefinition *GetGridLayerDefinition() { return this->m_gLayer; }

        // creates a clone of the given map/layer/symbol definition.
        // the definition is serialized and parsed into a new object, which is returned.
        static MapDefinition* CreateClone(MapDefinition *pSourceMD);
        static LayerDefinition* CreateClone(LayerDefinition *pSourceLD);
        static SymbolDefinition* CreateClone(SymbolDefinition *pSourceSD);

        bool GetSucceeded();

        // Occurs when an XML start tag is encountered. 'localname' is the text in
        // the tag to be paid attention to in our schema.
        void startElement( const   XMLCh* const    uri,
                           const   XMLCh* const    localname,
                           const   XMLCh* const    qname,
                           const   Attributes&     attributes);

        // Occurs when an XML closing tag is encountered. 'localname' is the text in
        // the tag to be paid attention to in our schema.
        void endElement( const XMLCh* const uri,
                         const XMLCh* const localname,
                         const XMLCh* const qname);

        // Occurs when characters in between XML tags are encountered.
        void characters(const XMLCh* const chars, const unsigned int length);

        // get error info
        const MdfString& GetErrorMessage();
};

END_NAMESPACE_MDFPARSER
#endif // _SAX2PARSER_H
