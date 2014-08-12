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

#ifndef _SAX2PARSER_H
#define _SAX2PARSER_H

#include "MapDefinition.h"
#include "TileSetDefinition.h"
#include "VectorLayerDefinition.h"
#include "DrawingLayerDefinition.h"
#include "GridLayerDefinition.h"
#include "SimpleSymbolDefinition.h"
#include "CompoundSymbolDefinition.h"
#include "PrintLayout/PrintLayoutDefinition.h"
#include "PrintLayout/PrintLayoutElementDefinition.h"
#include "PrintLayout/MapViewportDefinition.h"
#include "WatermarkDefinition.h"
#include "ProfileResult.h"

// disable C4244 of XSerializeEngine about __w64 int assigned to unsigned long
// disable C4267 of XmlScanner about size_t assigned to unsigned int
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>

#pragma warning(pop)

#include "IOUtil.h"
#include "SAX2ElementHandler.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFMODEL
END_NAMESPACE_MDFMODEL

BEGIN_NAMESPACE_MDFPARSER

// SAX2Parser is the main class for the parser. It holds the actual
// Xerces SAX2 Parser and is also the handler for the events generated
// by the parser. It holds the main references to the map, feature layer,
// or drawing layer that will be created by the parse. It handles both
// reading and writing through public functions.
class MDFPARSER_API SAX2Parser : public DefaultHandler
{
    public:
        SAX2Parser();
        ~SAX2Parser();
        void Flush();
        void Initialize();
        void DisableTabs();
        void ParseFile(std::string str);
        void ParseFile(char* str);
        void ParseString(const char* str, size_t numBytes);
        void ParseString(const wchar_t* str, size_t numChars);
        void WriteToFile(std::string name,
                         MapDefinition* map,
                         VectorLayerDefinition* vLayer,
                         DrawingLayerDefinition* dLayer,
                         GridLayerDefinition* gLayer,
                         PrintLayoutDefinition* printLayout,
                         MapViewportDefinition* mapViewport,
                         MdfModel::Version* version);
        void WriteToFile(std::string name, SymbolDefinition* symbol, MdfModel::Version* version);
        void WriteToFile(std::string name, WatermarkDefinition* watermark, MdfModel::Version* version);
        void WriteToFile(std::string name, ProfileResult* profileResult, MdfModel::Version* version);

        // Serialization functions for the various Definitions the parser supports
        std::string SerializeToXML(MapDefinition* map, MdfModel::Version* version);
        std::string SerializeToXML(LayerDefinition* layer, MdfModel::Version* version);
        std::string SerializeToXML(SymbolDefinition* symbol, MdfModel::Version* version);
        std::string SerializeToXML(PrintLayoutDefinition* printLayout, MdfModel::Version* version);
        std::string SerializeToXML(PrintLayoutElementDefinition* printLayoutElem, MdfModel::Version* version);
        std::string SerializeToXML(WatermarkDefinition* watermark, MdfModel::Version* version);
        std::string SerializeToXML(ProfileResult* profileResult, MdfModel::Version* version);
        std::string SerializeToXML(TileSetDefinition* tileset, MdfModel::Version* version);

        // Detaches the most recently parsed object from the parser.
        // The calling method is then responsible for disposing the object,
        // and the parser no longer has access to this object (set to NULL).
        MapDefinition* DetachMapDefinition();
        VectorLayerDefinition* DetachVectorLayerDefinition();
        DrawingLayerDefinition* DetachDrawingLayerDefinition();
        GridLayerDefinition* DetachGridLayerDefinition();
        LayerDefinition* DetachLayerDefinition();
        SimpleSymbolDefinition* DetachSimpleSymbolDefinition();
        CompoundSymbolDefinition* DetachCompoundSymbolDefinition();
        SymbolDefinition* DetachSymbolDefinition();
        PrintLayoutDefinition* DetachPrintLayoutDefinition();
        PrintLayoutElementDefinition* DetachPrintLayoutElementDefinition();
        MapViewportDefinition* DetachMapViewportDefinition();
        WatermarkDefinition* DetachWatermarkDefinition();
        ProfileResult* DetachProfileResult();
        TileSetDefinition* DetachTileSetDefinition();

        // Creates a clone of the given map/layer/symbol/print layout/print layout element definition.
        // The object is serialized and parsed into a new object, which is returned.
        static MapDefinition* CreateClone(MapDefinition* map);
        static LayerDefinition* CreateClone(LayerDefinition* layer);
        static SymbolDefinition* CreateClone(SymbolDefinition* symbol);
        static PrintLayoutDefinition* CreateClone(PrintLayoutDefinition* printLayout);
        static PrintLayoutElementDefinition* CreateClone(PrintLayoutElementDefinition* printLayoutElem);
        static WatermarkDefinition* CreateClone(WatermarkDefinition* watermark);
        static ProfileResult* CreateClone(ProfileResult* profileResult);
        static TileSetDefinition* CreateClone(TileSetDefinition* tileset);

        // Success State
        bool GetSucceeded() const;

        // Occurs when an XML start tag is encountered. 'localname' is the text in
        // the tag to be paid attention to in our schema.
        void startElement(const XMLCh* const uri,
                          const XMLCh* const localname,
                          const XMLCh* const qname,
                          const Attributes&  attributes);

        // Occurs when an XML closing tag is encountered. 'localname' is the text in
        // the tag to be paid attention to in our schema.
        void endElement(const XMLCh* const uri,
                        const XMLCh* const localname,
                        const XMLCh* const qname);

        // Occurs when characters in between XML tags are encountered.
        void characters(const XMLCh* const chars, const XMLSize_t length);

        // Get error info
        const MdfString& GetErrorMessage();

        // Get version info - this only returns meaningful information after parsing
        // a file or string
        const MdfModel::Version& GetVersion();

    protected:
        // Error handling override
        virtual void error(const SAXException& exc);

    private:
        // Helper methods to extract version info
        void SetMapDefinitionVersion(const Attributes& attributes);
        void SetLayerDefinitionVersion(const Attributes& attributes);
        void SetSymbolDefinitionVersion(const Attributes& attributes);
        void SetPrintLayoutDefinitionVersion(const Attributes& attributes);
        void SetPrintLayoutElementDefinitionVersion(const Attributes& attributes);
        void SetWatermarkDefinitionVersion(const Attributes& attributes);
        void SetProfileResultVersion(const Attributes& attributes);
        void SetTileSetDefinitionVersion(const Attributes& attributes);

    protected:
        // The objects to be generated by the parser.
        MapDefinition* m_map;
        VectorLayerDefinition* m_vLayer;
        DrawingLayerDefinition* m_dLayer;
        GridLayerDefinition* m_gLayer;
        SimpleSymbolDefinition* m_sSymbol;
        CompoundSymbolDefinition* m_cSymbol;
        PrintLayoutDefinition* m_printLayout;
        MapViewportDefinition* m_mapViewport;
        WatermarkDefinition* m_watermark;
        TileSetDefinition* m_tileset;
        ProfileResult* m_profileResult;

        // Succeeded is true if the parse has succeeded. As of now,
        // there are very loose constraints on this boolean.
        bool m_succeeded;

        // The implementing SAX2 parser
        SAX2XMLReader* m_parser;

        // handlerStack is a stack of SAX2ElementHandlers (see the
        // typedef in SAX2ElementHandler.h). When the parser receives
        // events they will be routed to the SAX2ElementHandler on the top
        // of the handlerStack, which will be an instance of one of the IO classes,
        // who all implement the SAX2ElementHandler interface.
        HandlerStack* m_handlerStack;

        // Unfortunately, the SAX2 Parser doesn't necessarily send all the
        // characters that appear inside a tag in one call of characters()-
        // for example with escape characters it separates them into separate calls.
        // Therefore, m_strbuffer is used to accumulate the characters.  tagOpen is
        // used in the case when you have characters outside a tag (i.e. <Metadata></Metadata>)
        // to make sure that whitespace or line returns don't get assigned to it instead.
        std::wstring m_strbuffer;
        bool m_tagOpen;

        // Parser Error String
        MdfString m_strParserError;

        // Version of the Definition being parsed
        MdfModel::Version m_version;
};

END_NAMESPACE_MDFPARSER
#endif // _SAX2PARSER_H
