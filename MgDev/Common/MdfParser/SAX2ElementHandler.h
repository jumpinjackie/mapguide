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

#ifndef _SAX2ELEMENTHANDLER_H
#define _SAX2ELEMENTHANDLER_H

#include <string>
#include <stack>
#include <vector>
#include "MdfParser.h"
#include "UnicodeString.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// ignore warning about needing to have dll-interface to be used by clients export
#pragma warning(disable: 4251)

BEGIN_NAMESPACE_MDFPARSER

// SAX2ElementHandler is the custom interface that all the IO classes
// implement. It is a cutomized and extremely simplified version of the
// DefaultHandler interface for the SAX2 Parser itself. Rather than have
// them all implement DefaultHandler and pass unnecessary arguments that
// never get used, plus handle the unnecessary overhead of constantly
// switching the handler to the parser, the handler for the parser stays
// as SAX2Parser and a stack of SAX2ElementHandler objects is maintained
// (the handlerStack) where the event calls get passed to the top
// SAX2ElementHandler on that stack. This is identical procedure-wise
// to how the MwfToolkit SAX parser was designed.
class SAX2ElementHandler;
typedef std::stack<SAX2ElementHandler*> HandlerStack;

class MDFPARSER_API SAX2ElementHandler
{
    public:
        SAX2ElementHandler(MdfModel::Version& version);
        virtual ~SAX2ElementHandler();

        virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack) = 0;
        virtual void ElementChars(const wchar_t* ch) = 0;
        virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack) = 0;

    protected:
        void ParseUnknownXml(const wchar_t* name, HandlerStack* handlerStack);

    protected:
        std::wstring m_unknownXml;

        // stores the name of the XML tag that initiated the
        // creation of this SAX2ElementHandler object
        std::wstring m_startElemName;

        // stores the name of the last XML start tag
        std::wstring m_currElemName;

        int m_currElemId;

        // flag indicating whether we're processing extended data for this element
        bool m_procExtData;

        // stores a version - used when deserializing
        MdfModel::Version m_version;
};


// For each element type, the following macros define a method, _ElementIdFromName()
// and two variables, eElementName - an integer ID which can be used in case
// statements, and sElementName - a std::string which can be used in serialization.
// CREATE_ELEMENT_MAP is called first, and then ELEM_MAP_ENTRY for each element
// type.  The IDs provided to ELEM_MAP_ENTRY must be consecutive, starting from 1.
#define CREATE_ELEMENT_MAP                                                      \
    static std::vector<std::wstring> _elementMap;                               \
    static std::string _CreateMapEntry(const wchar_t* wName, const char* sName) \
    {                                                                           \
        _elementMap.push_back(wName);                                           \
        return sName;                                                           \
    }                                                                           \
    static int _ElementIdFromName(const wchar_t* name)                          \
    {                                                                           \
        std::vector<std::wstring>::const_iterator iter = _elementMap.begin();   \
        int id = 1;                                                             \
        for (++iter; iter != _elementMap.end(); ++iter)                         \
        {                                                                       \
            if (0 == ::wcscmp(iter->c_str(), name))                             \
                return id;                                                      \
            id++;                                                               \
        }                                                                       \
        return 0;                                                               \
    }                                                                           \
    ELEM_MAP_ENTRY(0, Unknown)

#define ELEM_MAP_ENTRY(ID, NAME)                                                \
    static const std::string s##NAME = _CreateMapEntry(L###NAME, #NAME);        \
    static const int e##NAME = ID;

END_NAMESPACE_MDFPARSER
#endif // _SAX2ELEMENTHANDLER_H
