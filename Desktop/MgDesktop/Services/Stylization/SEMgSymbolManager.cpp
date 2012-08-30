#include "SEMgSymbolManager.h"
#include "SAX2Parser.h"
#include "PlatformBase.h"

#define SYMBOL_ERROR (SymbolDefinition*)1
#define IMAGE_ERROR (unsigned char*)1


SEMgdSymbolManager::SEMgdSymbolManager(MgResourceService* svc)
{
    m_svcResource = SAFE_ADDREF(svc);
}


SEMgdSymbolManager::~SEMgdSymbolManager()
{
    SAFE_RELEASE(m_svcResource);

    // free up cached symbols
    for (std::map<STRING, SymbolDefinition*>::iterator iter = m_mSymbolCache.begin();
        iter != m_mSymbolCache.end(); iter++)
    {
        if (iter->second != SYMBOL_ERROR)
            delete iter->second;
    }

    // free up cached images
    for (std::map<STRING, ImageData>::iterator iter = m_mImageCache.begin();
        iter != m_mImageCache.end(); iter++)
    {
        if (iter->second.data != IMAGE_ERROR)
            delete[] iter->second.data;
    }
}


SymbolDefinition* SEMgdSymbolManager::GetSymbolDefinition(const wchar_t* resourceId)
{
    if (!resourceId)
        resourceId = L"";

    // see if the named symbol already exists in the cache
    STRING uniqueName(resourceId);
    SymbolDefinition* symbol = m_mSymbolCache[uniqueName];

    // check if we errored on this symbol before and don't try again
    if (symbol == SYMBOL_ERROR)
        return NULL;

    if (!symbol)
    {
        try
        {
            Ptr<MgByteReader> sdReader;

#ifdef _DEBUG
            if (wcsncmp(uniqueName.c_str(), L"Library://", 10) == 0)
            {
                // get and parse the symboldef
                MgResourceIdentifier resId(uniqueName);
                sdReader = m_svcResource->GetResourceContent(&resId, L"");
            }
            else
            {
                sdReader = new MgByteReader(uniqueName, MgMimeType::Xml, false);
            }
#else
            // get and parse the symboldef
            MgResourceIdentifier resId(uniqueName);
            sdReader = m_svcResource->GetResourceContent(&resId, L"");
#endif

            Ptr<MgByteSink> sink = new MgByteSink(sdReader);
            Ptr<MgByte> bytes = sink->ToBuffer();
            assert(bytes->GetLength() > 0);

            MdfParser::SAX2Parser parser;
            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

            if (parser.GetSucceeded())
            {
                // detach the symbol definition from the parser - it's
                // now the caller's responsibility to delete it
                symbol = parser.DetachSymbolDefinition();
                assert(symbol);

                m_mSymbolCache[uniqueName] = symbol;
            }
        }
        catch (MgException* e)
        {
            e->Release();
        }

        if (!symbol)
        {
            // either the symbol was invalid or the symbol resource id was not
            // found - set it to something else that's invalid in the cache so
            // we know there was an error and don't try to get it again
            m_mSymbolCache[uniqueName] = SYMBOL_ERROR;
        }
    }

    return symbol;
}


bool SEMgdSymbolManager::GetImageData(const wchar_t* resourceId, const wchar_t* resourceName, ImageData& imageData)
{
    if (!resourceId)
        resourceId = L"";
    if (!resourceName)
        resourceName = L"";

    STRING uniqueName(resourceId);
    uniqueName.append(resourceName);
    imageData = m_mImageCache[uniqueName];

    if (imageData.data == IMAGE_ERROR)
        return false;

    if (imageData.data)
        return true;

    bool ret = false;
    try
    {
        Ptr<MgByteReader> sdReader;

#ifdef _DEBUG
        if (wcsncmp(resourceId, L"Library://", 10) == 0)
        {
            // get the image named "resourceName" attached to resource "resId"
            MgResourceIdentifier resId(resourceId);
            sdReader = m_svcResource->GetResourceData(&resId, resourceName);
        }
        else
        {
            sdReader = new MgByteReader(resourceName, MgMimeType::Png, false);
        }
#else
        // get the image named "resourceName" attached to resource "resId"
        MgResourceIdentifier resId(resourceId);
        sdReader = m_svcResource->GetResourceData(&resId, resourceName);
#endif

        int dataSize = (int)sdReader->GetLength();
        if (dataSize > 0 && dataSize < 16*1024*1024) // draw the line at 16 MB
        {
            imageData.size = dataSize;
            imageData.data = new unsigned char[dataSize];
            sdReader->Read(imageData.data, dataSize);

            // only PNG image data is supported
            imageData.format = RS_ImageFormat_PNG;
            imageData.width  = -1;
            imageData.height = -1;

            ret = true;
        }
    }
    catch (MgException* e)
    {
        e->Release();
    }

    if (!ret)
    {
        // either the image or image resource id was not found - set it to
        // something else that's invalid in the cache so we know there was
        // an error and don't try to get it again.
        imageData.size = 0;
        imageData.data = IMAGE_ERROR;
        imageData.format = RS_ImageFormat_Unknown;
    }

    m_mImageCache[uniqueName] = imageData;

    return ret;
}
