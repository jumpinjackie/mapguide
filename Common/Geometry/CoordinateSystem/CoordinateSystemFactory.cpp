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

using namespace CSLibrary;

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system factory.
///</summary>
MgCoordinateSystemFactory::MgCoordinateSystemFactory()
    :m_pCatalog(NULL)
{
    MG_TRY()
    //the catalog opens dictionaries for read by default
    CCoordinateSystemCatalog* pCatalog=new CCoordinateSystemCatalog();
    if (!pCatalog)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.MgCoordinateSystemFactory", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    m_pCatalog = SAFE_ADDREF(pCatalog);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.MgCoordinateSystemFactory")
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystemFactory::~MgCoordinateSystemFactory()
{
    //release cached coord systems
    for (std::map<STRING, MgCoordinateSystem*>::iterator iter = m_mapWktToCsDefinitionCache.begin();
        iter != m_mapWktToCsDefinitionCache.end(); 
        iter++)
    {
        if (NULL != iter->second)
        {
            SAFE_RELEASE(iter->second);
        }
    }
    m_mapWktToCsDefinitionCache.clear();

    SAFE_RELEASE(m_pCatalog);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Creates a coordinate system instance given a definition of the
/// coordinate system in OpenGIS Well-Known Text (WKT) format.
///</summary>
///<param name="srsWkt">
/// A string defining the coordinate system in OpenGIS WKT format.
///</param>
///<returns>
/// An MgCoordinateSystem2 instance that corresponds to the specified WKT
/// definition.
///</returns>
MgCoordinateSystem* MgCoordinateSystemFactory::Create(CREFSTRING srsWkt)
{
    MgCoordinateSystem* pCoordinateSystem = NULL;

    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.Create", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //lock all threads while we create the coordinate system
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    //see if we have this coordinate system in the cache
    pCoordinateSystem = m_mapWktToCsDefinitionCache[srsWkt];

    //if not, create one and also cache it
    if (NULL == pCoordinateSystem)
    {
        //lock all threads while we create the coordinate system
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

        //we have to check for NULL again because we might have been locked out by
        //another thread that just finished initializing the coordinate system
        pCoordinateSystem = m_mapWktToCsDefinitionCache[srsWkt];

        if (NULL == pCoordinateSystem)
        {
            Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
            pCoordinateSystem=pConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Unknown, srsWkt);
            m_mapWktToCsDefinitionCache[srsWkt] = pCoordinateSystem;
        }
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.Create")

    return SAFE_ADDREF(pCoordinateSystem);
}

/////////////////////////////////////////////////////////////////
MgCoordinateSystem* MgCoordinateSystemFactory::CreateFromCode(CREFSTRING sCode)
{
    MgCoordinateSystem* pCoordinateSystem = NULL;

    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.CreateFromCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //lock all threads while we create the coordinate system
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgCoordinateSystemDictionary> pDict=m_pCatalog->GetCoordinateSystemDictionary();
    MgGuardDisposable* pCs=pDict->Get(sCode);
    pCoordinateSystem=dynamic_cast<MgCoordinateSystem*>(pCs);
    assert(pCoordinateSystem);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.CreateFromCode")

    return SAFE_ADDREF(pCoordinateSystem);
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

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.GetCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.GetCatalog")

    return SAFE_ADDREF(m_pCatalog);
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemTransform* MgCoordinateSystemFactory::GetTransform(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)
{
    CCoordinateSystemTransform *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemTransform(pSource, pTarget);
    if (NULL == pNew) 
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFactory.GetTransform", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.GetTransform")

    //And we're done!  Return success.
    return SAFE_ADDREF(pNew);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified wkt string into the corresponding coordinate
/// system code.
///</summary>
///<param name="ogcWkt">
/// The wkt string to convert.
///</param>
///<returns>
/// String of the corresponding coordinate system code
///</returns>
STRING MgCoordinateSystemFactory::ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt)
{
    STRING sCode;

    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sCode=pConverter->WktToCode(MgCoordinateSystemWktFlavor::Unknown, ogcWkt, MgCoordinateSystemCodeFormat::Mentor);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertWktToCoordinateSystemCode")

    return sCode;
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
STRING MgCoordinateSystemFactory::ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode)
{
    STRING sWkt;

    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sWkt=pConverter->CodeToWkt(MgCoordinateSystemCodeFormat::Mentor, csCode, MgCoordinateSystemWktFlavor::Ogc);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertCoordinateSystemCodeToWkt")

    return sWkt;
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
    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ToString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemCategoryDictionary> pCtDict=m_pCatalog->GetCategoryDictionary();
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
    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemCategoryDictionary> pCtDict=m_pCatalog->GetCategoryDictionary();
    if (!pCtDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }
    Ptr<MgGuardDisposable> pCategory=pCtDict->Get(category);
    if (!pCategory)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryInDictionaryException", NULL);
    }
    MgCoordinateSystemCategory* pCategoryDef=dynamic_cast<MgCoordinateSystemCategory*>(pCategory.p);
    assert(pCategoryDef);
    if (!pCategoryDef)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryInDictionaryException", NULL);
    }
    Ptr<MgCoordinateSystemEnum> pEnum=pCategoryDef->GetEnum();
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

    //TODO: MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems")
    MG_CATCH(L"MgCoordinateSystemFactory.EnumerateCoordinateSystems") //TODO: When "Lat Lon" stops throwing an exception, let's go back to the above statement

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
    STRING sWkt;

    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    wchar_t wszEpsg[255];
    #ifdef WIN32
    _itow(code, wszEpsg, 10);
    #else
    swprintf(wszEpsg, 255, L"%d", code);
    #endif
    STRING strEpsgCode(wszEpsg);
    sWkt=pConverter->CodeToWkt(MgCoordinateSystemCodeFormat::Epsg, strEpsgCode, MgCoordinateSystemWktFlavor::Ogc);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.ConvertEpsgCodeToWkt")

    return sWkt;
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

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.ConvertWktToEpsgCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
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
bool MgCoordinateSystemFactory::IsValid(CREFSTRING ogcWkt)
{
    MG_TRY()

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFactory.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystem> pCs=pConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Unknown, ogcWkt);
    if (pCs)
    {
        return true;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFactory.IsValid")

    return false;
}
