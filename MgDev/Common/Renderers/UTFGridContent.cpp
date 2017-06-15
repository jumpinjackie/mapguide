#include "UTFGridContent.h"
#include "UnicodeString.h"
#include "agg_utfgrid_context.h"
#include "KeyEncode.h"

//Uncomment this line below to render UTFGrid content with extra debugging information attached to each
//feature
//#define DEBUG_UTFGRID

#define EMPTY_ENCODED_CHAR 32

UTFGridContent::UTFGridContent()
    : m_currentRemappedColor(0)
{
}

UTFGridContent::~UTFGridContent()
{
}

std::string UTFGridContent::GetString()
{
    std::string mbContent;
    UnicodeString::WideCharToMultiByte(m_content.str().c_str(), mbContent);
    return mbContent;
}

void UTFGridContent::EscapeJsonString(const std::wstring& input, std::wstring& output)
{
    std::wostringstream ss;
    for (std::wstring::const_iterator iter = input.begin(); iter != input.end(); iter++)
    {
        switch (*iter)
        {
        case '\\': ss << L"\\\\"; break;
        case '"': ss << L"\\\""; break;
        case '/': ss << L"\\/"; break;
        case '\b': ss << L"\\b"; break;
        case '\f': ss << L"\\f"; break;
        case '\n': ss << L"\\n"; break;
        case '\r': ss << L"\\r"; break;
        case '\t': ss << L"\\t"; break;
        default: ss << *iter; break;
        }
    }
    output.assign(ss.str());
}

bool UTFGridContent::AddFeature(RS_LayerUIInfo* layer, unsigned int decodedColor, RS_FeatureReader* feature, const RS_String * tooltip, const RS_String * url)
{
    // For this initial implementation of UTFGrid rendering, we will honor the original intent that UTFGrids in MapGuide
    // are meant to serve: As pre-rendered tooltip data for layers with tooltips enabled.
    if (!layer->hastooltips() && !layer->hashyperlinks())
    {
        return false;
    }

    std::string key = m_keyEncode.EncodeKey(feature);
    //No key, not selectable. Not selectable, don't bother encoding.
    if (!key.empty() && m_features.find(key) == m_features.end())
    {
        std::wstring wVal = L"{";

        wVal.append(L"\"MG_TOOLTIP\": ");
        if (NULL != tooltip)
        {
            std::wstring oPropVal;
            std::wstring iPropVal = *tooltip;
            EscapeJsonString(iPropVal, oPropVal);

            wVal.append(L"\"");
            wVal.append(oPropVal);
            wVal.append(L"\"");
        }
        else 
        {
            wVal.append(L"null");
        }
        wVal.append(L",\"MG_URL\": ");
        if (NULL != url)
        {
            std::wstring oPropVal;
            std::wstring iPropVal = *url;
            EscapeJsonString(iPropVal, oPropVal);

            wVal.append(L"\"");
            wVal.append(oPropVal);
            wVal.append(L"\"");
        }
        else
        {
            wVal.append(L"null");
        }

        /*
        const wchar_t * geomName = feature->GetGeomPropName();
        const wchar_t * rasterName = feature->GetRasterPropName();

        bool bFirst = true;
        int count = 0;
        const wchar_t* const* propNames = feature->GetPropNames(count);
        for (int i = 0; i < count; i++)
        {
            const wchar_t* propName = propNames[i];
            //Skip geometry/raster
            if ((geomName && wcscmp(propName, geomName) == 0) ||
                (rasterName && wcscmp(propName, rasterName) == 0))
            {
                continue;
            }

            if (!bFirst)
            {
                wVal.append(L",");
            }
            const wchar_t* propVal = feature->GetAsString(propName);

            wVal.append(L"\"");
            wVal.append(propName);
            wVal.append(L"\": ");

            int propType = feature->GetPropertyType(propName);
            if (propType == FdoDataType_String || propType == FdoDataType_DateTime)
            {
                if (propType == FdoDataType_String)
                {
                    std::wstring oPropVal;
                    std::wstring iPropVal = propVal;
                    EscapeJsonString(iPropVal, oPropVal);

                    wVal.append(L"\"");
                    wVal.append(oPropVal);
                    wVal.append(L"\"");
                }
                else 
                {
                    wVal.append(L"\"");
                    wVal.append(propVal);
                    wVal.append(L"\"");
                }
            }
            else 
            {
                wVal.append(propVal);
            }

            bFirst = false;
        }
        */
#ifdef DEBUG_UTFGRID
        wVal.append(L",\"_MGLAYER_NAME\": ");
        wVal.append(L"\"");
        wVal.append(layer->name());
        wVal.append(L"\"");
        wVal.append(L",\"_MGLAYER_ID\": ");
        wVal.append(L"\"");
        wVal.append(layer->layerdef());
        wVal.append(L"\"");
        wVal.append(L",\"_MGLAYER_TOOLTIP\": ");
        if (NULL != tooltip) 
        {
            wVal.append(L"\"");
            wVal.append(*tooltip);
            wVal.append(L"\"");
        }
        else 
        {
            wVal.append(L"null");
        }
        wVal.append(L",\"_MGLAYER_URL\": ");
        if (NULL != url)
        {
            wVal.append(L"\"");
            wVal.append(*url);
            wVal.append(L"\"");
        }
        else
        {
            wVal.append(L"null");
        }
        wVal.append(L",\"_MGLAYER_SELECTABLE\": ");
        wVal.append(layer->selectable() ? L"true" : L"false");
        wVal.append(L",\"_MGLAYER_HASTOOLTIPS\": ");
        wVal.append(layer->hastooltips() ? L"true" : L"false");
#endif

        wVal += L"}";

        m_features.insert(std::make_pair(key, wVal));
        m_pixels.insert(std::make_pair(decodedColor, key));
        return true;
    }
    return false;
}

void UTFGridContent::StartGrid()
{
    m_content << "{" << std::endl << "  \"grid\": [" << std::endl;
}

void UTFGridContent::StartGridRow(bool bFirst)
{
    if (bFirst)
        m_content << "    \"";
    else
        m_content << "," << std::endl << "    \"";
}

void UTFGridContent::BeginTracking()
{
    _ASSERT(m_currentRemappedColor == 0);
    // Pre-register the "empty space" pixel
    m_trackedEncodedColors.insert(EMPTY_ENCODED_CHAR);
    m_remappedEncodedColors.insert(std::make_pair(EMPTY_ENCODED_CHAR, EncodeChar(m_currentRemappedColor)));
    m_remappedEncodedColorsInv.insert(std::make_pair(m_currentRemappedColor, EMPTY_ENCODED_CHAR));
}

void UTFGridContent::TrackPixel(unsigned int pixel)
{
    // To maximize our utf character "budget", start from 0 again and for each rendered
    // "pixel" we encounter, map it to our re-mapped and pre-encoded offset pixel. When it comes time to render
    // the actual content, use the re-mapped offset pixel instead of what was rendered to the AGG backend, but
    // retain the use of the original pixel for accessing the keys and data to be rendered out
    if (m_trackedEncodedColors.find(pixel) == m_trackedEncodedColors.end())
    {
        m_currentRemappedColor++;
        m_remappedEncodedColors.insert(std::make_pair(pixel, EncodeChar(m_currentRemappedColor)));
        m_remappedEncodedColorsInv.insert(std::make_pair(m_currentRemappedColor, pixel));
        m_trackedEncodedColors.insert(pixel);
    }
}

void UTFGridContent::AppendRowPixel(unsigned int pixel)
{
    // This pixel should've been already re-mapped
    _ASSERT(m_remappedEncodedColors.find(pixel) != m_remappedEncodedColors.end());

    // Don't write the original pixel, write its re-mapped one
    m_content << (wchar_t)m_remappedEncodedColors[pixel];
}

void UTFGridContent::EndGridRow()
{
    m_content << "\"";
}

void UTFGridContent::EndGrid()
{
    m_content << std::endl << "  ]";
}

void UTFGridContent::WriteKeys()
{
    m_content << "," << std::endl;
    m_content << "  \"keys\": [" << std::endl;
    m_content << "    \"\""; //This is the "empty space" pixel
    for (unsigned int i = 1; i <= m_currentRemappedColor; i++)
    {
        //Every remapped pixel in this "ramp" should be accounted for
        _ASSERT(m_remappedEncodedColorsInv.find(i) != m_remappedEncodedColorsInv.end());

        m_content << "," << std::endl;
        unsigned int decodedKey = DecodeChar(m_remappedEncodedColorsInv[i]);
        m_content << "    \"" << decodedKey << "\"";
    }
    m_content << std::endl << "  ]";
}

void UTFGridContent::WriteData()
{
    m_content << "," << std::endl;
    m_content << "  \"data\": {" << std::endl;
    bool bFirst = true;
    for (ColorSet::iterator it = m_trackedEncodedColors.begin(); it != m_trackedEncodedColors.end(); it++)
    {
        if (*it == EMPTY_ENCODED_CHAR) // This is the "empty space" pixel
        {
            continue;
        }

        unsigned int colorKey = DecodeChar(*it);
        if (m_pixels.find(colorKey) != m_pixels.end())
        {
            std::string featKey = m_pixels[colorKey];
            std::wstring featData = m_features[featKey];
            if (!bFirst)
                m_content << "," << std::endl;

            m_content << "    \"" << colorKey << "\": " << featData;
            bFirst = false;
        }
    }
    m_content << std::endl << "  }" << std::endl; //End data
    m_content << "}"; //End UTFGrid
}

unsigned int UTFGridContent::EncodeChar(unsigned int toEncode)
{
    // Encoding IDs: https://github.com/mapbox/utfgrid-spec/blob/master/1.3/utfgrid.md#encoding-ids

    // Add 32
    unsigned int encoded = toEncode + 32;
    // If result is >= 34, add 1
    if (encoded >= 34)
    {
        encoded += 1;
    }
    // If result is >= 92, add 1
    if (encoded >= 92)
    {
        encoded += 1;
    }
    return encoded;
}

unsigned int UTFGridContent::DecodeChar(unsigned int toDecode)
{
    // Decoding IDs: https://github.com/mapbox/utfgrid-spec/blob/master/1.3/utfgrid.md#encoding-ids

    // If codepoint is >= 93, subtract 1
    unsigned int decoded = toDecode;
    if (decoded >= 93)
    {
        decoded -= 1;
    }
    // If codepoint is >= 35, subtract 1
    if (decoded >= 35)
    {
        decoded -= 1;
    }
    // Subtract 32
    return decoded - 32;
}