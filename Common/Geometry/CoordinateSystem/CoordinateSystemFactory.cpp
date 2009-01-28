//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"             //for COORDINATE_SYSTEM_API
#include "CoordinateSystem.h"           //for MgCoordinateSystem
#include "CoordSys.h"                   //for CCoordinateSystem
#include "CoordSysEnum.h"               //for CCoordinateSystemEnum
#include "CoordSysEnum.h"               //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"          //for CCoordinateSystemEnumDatum
#include "CoordSysEnumEllipsoid.h"      //for CCoordinateSystemEnumEllipsoid
#include "CoordSysDictionary.h"         //for CCoordinateSystemDictionary
#include "CoordSysDatumDictionary.h"    //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"//for CCoordinateSystemEllipsoidDictionary
#include "CoordSysUtil.h"               //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "CoordSysCategoryDictionary.h" //for CCoordinateSystemCategoryDictionary
#include "CoordSysCatalog.h"            //for CCoordinateSystemCatalog
#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordinateSystemCatalog.h"    //for MgCoordinateSystemCatalog
#include "CoordinateSystemFactory.h"    //for MgCoordinateSystemFactory
#include "CoordinateSystemCache.h"

using namespace CSLibrary;

// Dummy class used to automate initialization/uninitialization of the CoordinateSystemCatalog.
class CInitCSC
{
public:
    ~CInitCSC()
    {
        MgCoordinateSystemFactory::DeleteCatalog();
    }
};

static CInitCSC s_InitCSC;


MgCoordinateSystemCatalog* MgCoordinateSystemFactory::sm_pCatalog = NULL;

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system factory.
///</summary>
MgCoordinateSystemFactory::MgCoordinateSystemFactory()
{
    MG_TRY()

    // Set the single copy of the catalog
    if (sm_pCatalog == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance()));
        if (sm_pCatalog == NULL)
        {
            //the catalog opens dictionaries for read by default
            sm_pCatalog = new CCoordinateSystemCatalog();
        }
    }

    if (!sm_pCatalog)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.MgCoordinateSystemFactory", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.MgCoordinateSystemFactory")
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystemFactory::~MgCoordinateSystemFactory()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an MgCoordinateSystem instance given a definition of the
/// coordinate system in OpenGIS Well-Known Text (WKT) format.
///
MgCoordinateSystem* MgCoordinateSystemFactory::Create(CREFSTRING wkt)
{
    Ptr<MgCoordinateSystem> coordinateSystem;

    MG_TRY()

    if (NULL == sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(
            L"MgCoordinateSystemFactory.Create",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgCoordinateSystemCache* coordinateSystemCache = MgCoordinateSystemCache::GetInstance();

    coordinateSystem = coordinateSystemCache->Get(wkt);

    if (NULL == coordinateSystem.p)
    {
        Ptr<MgCoordinateSystemFormatConverter> converter = sm_pCatalog->GetFormatConverter();

        if (NULL != converter.p)
        {
            coordinateSystem = converter->WktToDefinition(MgCoordinateSystemWktFlavor::Unknown, wkt);
        }

        coordinateSystemCache->Set(wkt, coordinateSystem.p);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.Create")

    return coordinateSystem.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an MgCoordinateSystem instance given a coordinate system code.
///
MgCoordinateSystem* MgCoordinateSystemFactory::CreateFromCode(CREFSTRING code)
{
    Ptr<MgCoordinateSystem> coordinateSystem;

    MG_TRY()

    if (NULL == sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(
            L"MgCoordinateSystemFactory.CreateFromCode",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgCoordinateSystemCache* coordinateSystemCache = MgCoordinateSystemCache::GetInstance();

    coordinateSystem = coordinateSystemCache->Get(code);

    if (NULL == coordinateSystem.p)
    {
        Ptr<MgCoordinateSystemDictionary> dict = sm_pCatalog->GetCoordinateSystemDictionary();
        Ptr<MgGuardDisposable> disposableObj = dict->Get(code);

        coordinateSystem = SAFE_ADDREF(dynamic_cast<MgCoordinateSystem*>(disposableObj.p));
        coordinateSystemCache->Set(code, coordinateSystem.p);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.CreateFromCode")

    return coordinateSystem.Detach();
}

//--------------------------------------------------------------------------------------------
// <summary>Deletes the static catalog.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemFactory::DeleteCatalog()
{
    ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance()));

    if (sm_pCatalog)
    {
        sm_pCatalog->PrepareForDispose();
        sm_pCatalog->Release();
        sm_pCatalog = NULL;
    }
}


//--------------------------------------------------------------------------------------------
// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemFactory::Dispose()
{
    delete this;
}


//--------------------------------------------------------------------------------------------
//The value for nOpenMode is one of the values defined in the class CsDictionaryOpenMode
//
MgCoordinateSystemCatalog* MgCoordinateSystemFactory::GetCatalog()
{
    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.GetCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.GetCatalog")

    return SAFE_ADDREF((MgCoordinateSystemCatalog*)sm_pCatalog);
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemTransform* MgCoordinateSystemFactory::GetTransform(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)
{
    Ptr<CCoordinateSystemTransform> pNew;
    MG_TRY()
    pNew=new CCoordinateSystemTransform(pSource, pTarget);
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.GetTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.GetTransform")

    //And we're done!  Return success.
    return pNew.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified wkt string into the corresponding coordinate
/// system code.
///</summary>
///<param name="wkt">
/// The wkt string to convert.
///</param>
///<returns>
/// String of the corresponding coordinate system code
///</returns>
STRING MgCoordinateSystemFactory::ConvertWktToCoordinateSystemCode(CREFSTRING wkt)
{
    STRING code;

    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=sm_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    code=pConverter->WktToCode(MgCoordinateSystemWktFlavor::Unknown, wkt, MgCoordinateSystemCodeFormat::Mentor);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode")

    return code;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate system code into the corresponding
/// wkt string.
///</summary>
///<param name="code">
/// The coordinate system code string to convert.
///</param>
///<returns>
/// String of the corresponding wkt
///</returns>
STRING MgCoordinateSystemFactory::ConvertCoordinateSystemCodeToWkt(CREFSTRING code)
{
    STRING wkt;

    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=sm_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Check to see if this is an EPSG code. ie: "EPSG:4326"
    // Make code uppercase
    STRING ucCode = ToUpper(code);
    size_t position = ucCode.find(L"EPSG:", 0); // NOXLATE
    if(position != string::npos)
    {
        // This is an EPSG code
        wkt=pConverter->CodeToWkt(MgCoordinateSystemCodeFormat::Epsg, code, MgCoordinateSystemWktFlavor::Ogc);
    }
    else
    {
        wkt=pConverter->CodeToWkt(MgCoordinateSystemCodeFormat::Mentor, code, MgCoordinateSystemWktFlavor::Ogc);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt")

    return wkt;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets a list of the available coordinate system categories.
///</summary>
///<returns>
/// A list of the available catetories.
///</returns>
MgStringCollection* MgCoordinateSystemFactory::EnumerateCategories()
{
    Ptr<MgStringCollection> categoryNames;

    MG_TRY()

    categoryNames = new MgStringCollection();
    if (!categoryNames)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.EnumerateCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ToString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemCategoryDictionary> pCtDict=sm_pCatalog->GetCategoryDictionary();
    if (!pCtDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCategories", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }
    Ptr<MgCoordinateSystemEnum> pEnum=pCtDict->GetEnum();
    if (!pEnum)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.EnumerateCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    UINT32 nNbCategories=pCtDict->GetSize();
    categoryNames=pEnum->NextName(nNbCategories);
    if (categoryNames->GetCount()!=nNbCategories)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.EnumerateCategories")

    return categoryNames.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets a list of available coordinate systems in the specified category.
///</summary>
///<param name="category">
/// The category to retrieve the coordinate systems for.
///</param>
///<returns>
/// A batch property collection describing the available coordinate systems
/// and their properties.
///</returns>
MgBatchPropertyCollection* MgCoordinateSystemFactory::EnumerateCoordinateSystems(CREFSTRING category)
{
    Ptr<MgBatchPropertyCollection> pCoordinateSystems;

    MG_TRY()

    pCoordinateSystems = new MgBatchPropertyCollection();
    if (!pCoordinateSystems)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemCategoryDictionary> pCtDict=sm_pCatalog->GetCategoryDictionary();
    if (!pCtDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }
    Ptr<MgCoordinateSystemCategory> pCategory=pCtDict->GetCategory(category);
    if (!pCategory)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryInDictionaryException", NULL);
    }
    Ptr<MgCoordinateSystemEnum> pEnum=pCategory->GetEnum();
    if (!pEnum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for ( ; ; )
    {
        Ptr<MgDisposableCollection> pCsColl=pEnum->Next(1);

        if (1 != pCsColl->GetCount()) break;
        Ptr<MgDisposable> pCs=pCsColl->GetItem(0);
        MgCoordinateSystem* pCsDef=dynamic_cast<MgCoordinateSystem*>(pCs.p);
        assert(pCsDef);

        // Add the coodinate system properties to the collection
        Ptr<MgProperty> pProperty;
        Ptr<MgPropertyCollection> pCoordSysProperties = new MgPropertyCollection();

        // Coordinate system properties
        // Add the Code
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemCode, pCsDef->GetCode());
        pCoordSysProperties->Add(pProperty);

        // Add the Description
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemDescription, pCsDef->GetDescription());
        pCoordSysProperties->Add(pProperty);

        // Add the Projection Name
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemProjection, pCsDef->GetProjection());
        pCoordSysProperties->Add(pProperty);

        // Add the Projection Description
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemProjectionDescription, pCsDef->GetProjectionDescription());
        pCoordSysProperties->Add(pProperty);

        // Add the Datum Name
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemDatum, pCsDef->GetDatum());
        pCoordSysProperties->Add(pProperty);

        // Add the Datum description
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemDatumDescription, pCsDef->GetDatumDescription());
        pCoordSysProperties->Add(pProperty);

        // Add the Ellipsoid Name
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemEllipsoid, pCsDef->GetEllipsoid());
        pCoordSysProperties->Add(pProperty);

        // Add the Ellipsoid description
        pProperty = new MgStringProperty(CCoordinateSystem::CoordinateSystemEllipsoidDescription, pCsDef->GetEllipsoidDescription());
        pCoordSysProperties->Add(pProperty);

        // Add the coordinate system properties to the collection of coordinate systems
        pCoordinateSystems->Add(pCoordSysProperties);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems")

    return pCoordinateSystems.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Converts the specified EPSG coordinate system code into the corresponding
/// wkt string.
///
/// \param code
/// The EPSG coordinate system code to convert.
///
/// \return
/// String of the corresponding wkt
///
STRING MgCoordinateSystemFactory::ConvertEpsgCodeToWkt(INT32 code)
{
    STRING wkt;

    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=sm_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    wchar_t wszEpsg[255];
    #ifdef _WIN32
    _itow(code, wszEpsg, 10);
    #else
    swprintf(wszEpsg, 255, L"%d", code);
    #endif
    STRING strEpsgCode(wszEpsg);
    wkt=pConverter->CodeToWkt(MgCoordinateSystemCodeFormat::Epsg, strEpsgCode, MgCoordinateSystemWktFlavor::Ogc);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt")

    return wkt;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Converts the specified wkt string into the corresponding
/// EPSG code string.
///
/// \param wkt
/// The wkt string to convert.
///
/// \return
/// Value of the corresponding EPSG code
///
INT32 MgCoordinateSystemFactory::ConvertWktToEpsgCode(CREFSTRING wkt)
{
    INT32 nEpsg=-1;

    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToEpsgCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=sm_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToEpsgCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING strEpsgCode=pConverter->WktToCode(MgCoordinateSystemWktFlavor::Ogc, wkt, MgCoordinateSystemCodeFormat::Epsg);
    nEpsg = (INT32)wcstol(strEpsgCode.c_str(), NULL, 10);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertWktToEpsgCode")

    return nEpsg;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system category that this coordinate
/// system belongs to.
/// </summary>
///<returns>
/// coordinate system category string
///</returns>
STRING MgCoordinateSystemFactory::GetBaseLibrary()
{
    return CCoordinateSystem::BaseLibrary;
}

///////////////////////////////////////////////////////////////////////////
bool MgCoordinateSystemFactory::IsValid(CREFSTRING wkt)
{
    MG_TRY()

    if (!sm_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgCoordinateSystemFormatConverter> pConverter=sm_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        Ptr<MgCoordinateSystem> pCs=pConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Unknown, wkt);
        if (pCs)
        {
            return true;
        }
    }
    catch(MgException *pE)
    {
        pE->Release();
        return false;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.IsValid")

    return false;
}
