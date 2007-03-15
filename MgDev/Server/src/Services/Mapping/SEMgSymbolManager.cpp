//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#include "ServerMappingServiceDefs.h"
#include "SEMgSymbolManager.h"
#include "SAX2Parser.h"
#include "MapGuideCommon.h"

#define SYMBOL_ERROR (SymbolDefinition*)1
#define IMAGE_ERROR (unsigned char*)1

SEMgSymbolManager::SEMgSymbolManager(MgResourceService* svc)
{
    m_svcResource = svc;
    SAFE_ADDREF(m_svcResource);
}

SEMgSymbolManager::~SEMgSymbolManager()
{
    SAFE_RELEASE(m_svcResource);

    //free up cached symbols
    for (std::map<STRING, SymbolDefinition*>::iterator iter = m_mSymbolCache.begin();
        iter != m_mSymbolCache.end(); iter++)
    {
        if (iter->second != SYMBOL_ERROR)
            delete (SymbolDefinition*)(iter->second);
    }

    for (std::map<STRING, unsigned char*>::iterator iter = m_mImageCache.begin();
        iter != m_mImageCache.end(); iter++)
    {
        if (iter->second != IMAGE_ERROR)
            delete[] (unsigned char*)(iter->second);
    }
}

SymbolDefinition* SEMgSymbolManager::GetSymbolDefinition(const wchar_t* resource)
{
    STRING uniqueName = STRING(resource);
    SymbolDefinition* ret = m_mSymbolCache[uniqueName];

    //check if we errored on this symbol before and
    //don't try again
    if (ret == SYMBOL_ERROR)
        return NULL;

    if (!ret)
    {
        try
        {
            Ptr<MgByteReader> sdReader;

            if (wcsncmp(uniqueName.c_str(), L"Library://", 10) == 0)
            {
                //get and parse the mapdef
                MgResourceIdentifier resId(uniqueName);
                sdReader = m_svcResource->GetResourceContent(&resId, L"");
            }
            else
            {
                sdReader = new MgByteReader(uniqueName, MgMimeType::Xml, false);
            }

            Ptr<MgByteSink> sink = new MgByteSink(sdReader);
            Ptr<MgByte> bytes = sink->ToBuffer();

            assert(bytes->GetLength() > 0);

            MdfParser::SAX2Parser parser;
            parser.ParseString((const char *)bytes->Bytes(), bytes->GetLength());

            assert(parser.GetSucceeded());

            // detach the feature layer definition from the parser - it's
            // now the caller's responsibility to delete it
            ret = parser.DetachSymbolDefinition();

            assert(ret);

            m_mSymbolCache[uniqueName] = ret;
        }
        catch (MgException* e)
        {
            //either symbol or symbol library was not found
            //Set it to something else that's invalid (like 1) in the cache so that
            //we know there was an error and don't try to get it again.
            e->Release();
            ret = NULL;
            m_mSymbolCache[uniqueName] = SYMBOL_ERROR;
        }
    }

    return ret;
}

const unsigned char* SEMgSymbolManager::GetImageData(const wchar_t* resource, int& length)
{
    STRING uniqueName = STRING(resource);
    unsigned char* ret = m_mImageCache[uniqueName];
    length = 0;

    if (ret == IMAGE_ERROR)
        return NULL;

    if (!ret)
    {
        try
        {
            Ptr<MgByteReader> sdReader;

            if (wcsncmp(uniqueName.c_str(), L"Library://", 10) == 0)
            {
                MgResourceIdentifier resId(uniqueName);

                //get and parse the mapdef
                sdReader = m_svcResource->GetResourceContent(&resId, L"");
            }
            else
            {
                sdReader = new MgByteReader(uniqueName, MgMimeType::Png, false);
            }

            INT64 len = sdReader->GetLength();
            if (len > 0 && len < 16*1024*1024) // draw the line at 16 MB
            {
                length = (int)len;
                ret = new unsigned char[length];
                sdReader->Read(ret, length);
                m_mImageCache[uniqueName] = ret;
            }
        }
        catch (MgException* e)
        {
            //either symbol or symbol library was not found
            //Set it to something else that's invalid (like 1) in the cache so that
            //we know there was an error and don't try to get it again.
            e->Release();
            ret = NULL;
            m_mImageCache[uniqueName] = IMAGE_ERROR;
        }
    }

    return ret;
}
