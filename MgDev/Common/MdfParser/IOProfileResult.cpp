//
//  Copyright (C) 2011 by Autodesk, Inc.
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
#include "IOProfileResult.h"
#include "IOProfileRenderMapResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileResult);
ELEM_MAP_ENTRY(2, ExtendedData1);


IOProfileResult::IOProfileResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileResult = NULL;
}


IOProfileResult::IOProfileResult(ProfileResult* profileResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileResult = profileResult;
}


IOProfileResult::~IOProfileResult()
{
}


void IOProfileResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileResult::Write(MdfStream& fd, ProfileResult* profileResult, Version* version, MgTab& tab)
{
    // verify the ProfileResult version
    MdfString strVersion;
    if (version)
    {
        if ((*version >= Version(1, 0, 0)) && (*version <= Version(2, 4, 0)))
        {
            // PRDF in MapGuide 2013 - current
            strVersion = version->ToString();
        }
        else
        {
            // unsupported ProfileResult version
            // TODO - need a way to return error information
            _ASSERT(false);
            return;
        }
    }
    else
    {
        // use the current highest version
        strVersion = L"2.4.0"; // NOXLATE
    }

    fd << tab.tab() << "<ProfileResult xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ProfileResult-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    tab.inctab();

    // Property: ProfileResult
    switch (profileResult->GetProfileResultType())
    {
        case ProfileResult::ProfileRenderMap:
        case ProfileResult::ProfileRenderDynamicOverlay:
            {
                ProfileRenderMapResult* profileRenderMapResult = dynamic_cast<ProfileRenderMapResult*>(profileResult);
                if(NULL != profileRenderMapResult)
                {
                    IOProfileRenderMapResult::Write(fd, profileRenderMapResult, version, tab);
                }
                break;
            }

        // need default?
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</ProfileResult>" << std::endl; // NOXLATE
}
