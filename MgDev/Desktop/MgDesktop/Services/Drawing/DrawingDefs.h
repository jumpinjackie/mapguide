#ifndef DESKTOP_DRAWING_DEFS_H
#define DESKTOP_DRAWING_DEFS_H

#include "dwfcore/Core.h"
#include "dwfcore/Exception.h"

using namespace DWFCore;

#define MG_SERVER_DRAWING_SERVICE_TRY()                                       \
    MG_TRY()                                                                  \

#define MG_SERVER_DRAWING_SERVICE_CATCH(methodName)                           \
    }                                                                         \
    catch (DWFException& e)                                                   \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(STRING(e.message()));                                   \
        mgException = new MgdDwfException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_SERVER_DRAWING_SERVICE_THROW()                                     \
    MG_THROW()                                                                \

#define MG_SERVER_DRAWING_SERVICE_CATCH_AND_THROW(methodName)                 \
    MG_SERVER_DRAWING_SERVICE_CATCH(methodName)                               \
                                                                              \
    MG_SERVER_DRAWING_SERVICE_THROW()                                         \

const STRING ELEMENT_SOURCENAME         = L"SourceName";          // NOXLATE
const STRING ELEMENT_PASSWORD           = L"Password";            // NOXLATE
const STRING ELEMENT_COORDINATE_SPACE   = L"CoordinateSpace";     // NOXLATE
const STRING MANIFEST_XML               = L"manifest.xml";        // NOXLATE
const STRING PROPERTY_CREATOR           = L"creator";             // NOXLATE
const STRING PROPERTY_CREATOR_VALUE     = L"Drawing Service";     // NOXLATE
const STRING DWF_AUTHOR                 = L"Autodesk, Inc.";      // NOXLATE
const STRING DWF_SOURCE_PRODUCT_VENDOR  = L"Autodesk";            // NOXLATE
const STRING DWF_SOURCE_PRODUCT_NAME    = L"MapGuide Server";     // NOXLATE
const STRING DWF_SOURCE_PRODUCT_VERSION = L"7.0";                 // NOXLATE
const STRING DWF_PRODUCT_VENDOR         = L"Autodesk";            // NOXLATE
const STRING RESOURCENAME_SEPARATOR     = L"\\";                  // NOXLATE
const STRING DEFAULT_COORDINATE_SPACE   = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",1]]";                // NOXLATE


#endif