#ifndef MGDESKTOP_H
#define MGDESKTOP_H

#include "Foundation.h"
#include "PlatformBase.h"

#ifdef _WIN32
#ifdef DESKTOP_EXPORTS
#    define MG_DESKTOP_API __declspec(dllexport)
#else
#    define MG_DESKTOP_API __declspec(dllimport)
#endif
#else
#define MG_DESKTOP_API
#endif

#if !defined(DWFTK_USE_DWFCORE_ZLIB)
    #define DWFTK_USE_DWFCORE_ZLIB
#endif
#pragma warning( disable : 4290 )

#include "System/ClassId.h"
#include "System/ConfigProperties.h"
#include "System/PlatformInit.h"

#include "Exception/AliasNotFoundException.h"
#include "Exception/ConnectionFailedException.h"
#include "Exception/DuplicateParameterException.h"
#include "Exception/DwfException.h"
#include "Exception/DwfSectionNotFoundException.h"
#include "Exception/DwfSectionResourceNotFoundException.h"
#include "Exception/InvalidDwfPackageException.h"
#include "Exception/InvalidDwfSectionException.h"
#include "Exception/InvalidFeatureSourceException.h"
#include "Exception/InvalidLogEntryException.h"
#include "Exception/InvalidPrintLayoutFontSizeUnitsException.h"
#include "Exception/InvalidPrintLayoutPositionUnitsException.h"
#include "Exception/InvalidPrintLayoutSizeUnitsException.h"
#include "Exception/ParameterNotFoundException.h"
#include "Exception/StylizeLayerFailedException.h"

#include "MapLayer/Layer.h"
#include "MapLayer/Map.h"
#include "MapLayer/Selection.h"

#include "Services/DataReader.h"
#include "Services/DrawingService.h"
#include "Services/FeatureReader.h"
#include "Services/ScrollableFeatureReader.h"
#include "Services/FeatureService.h"
#include "Services/ImageFormats.h"
#include "Services/MappingService.h"
#include "Services/ProfilingService.h"
#include "Services/RenderingService.h"
#include "Services/ResourceService.h"
#include "Services/ServiceFactory.h"
#include "Services/SqlReader.h"
#include "Services/TileService.h"
#include "Services/Transaction.h"

#include "Services/Feature/RdbmsFeatureSourceParams.h"

#include "Services/Rendering/CustomLogoInfo.h"
#include "Services/Rendering/CustomTextInfo.h"
#include "Services/Rendering/DwfVersion.h"
#include "Services/Rendering/FeatureInformation.h"
#include "Services/Rendering/Layout.h"
#include "Services/Rendering/MapPlot.h"
#include "Services/Rendering/MapPlotCollection.h"
#include "Services/Rendering/MapPlotInstruction.h"
#include "Services/Rendering/PlotSpecification.h"
#include "Services/Rendering/PrintLayout.h"
#include "Services/Rendering/RenderingOptions.h"
#include "Services/Rendering/UnitType.h"

#include "Services/Resource/UnmanagedDataType.h"

#define NOT_IMPLEMENTED(method) \
	throw new MgNotImplementedException(method, __LINE__, __WFILE__, NULL, L"", NULL)

//Uncomment to debug memory leaks. This is the header for Visual Leak Detector
//See: http://vld.codeplex.com for download and setup
#ifdef _WIN32
#ifdef _DEBUG
#define USING_VLD
#include <vld.h>
#endif
#endif

//Uncomment to get extra FDO join chatter when running unit tests
//#define DEBUG_FDOJOIN
//Uncomment to get extra connection pool chatter
//#define DEBUG_FDO_CONNECTION_POOL

#endif