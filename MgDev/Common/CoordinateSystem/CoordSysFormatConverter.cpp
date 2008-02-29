//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "CoordSysCommon.h"

#include "CoordSys.h"                   //for CCoordinateSystem
#include "CoordSysFormatConverter.h"    //for CCoordinateSystemFormatConverter
#include "CoordSysUtil.h"               //for CCsNumericDotLocale
#include "ArbitraryCoordsys.h"          //for CCsArbitraryCoordinateSystemUtil


using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemFormatConverter::CCoordinateSystemFormatConverter(MgCoordinateSystemCatalog *pCatalog)
{
    SetCatalog(pCatalog);
}

CCoordinateSystemFormatConverter::~CCoordinateSystemFormatConverter()
{
}

//MgDisposable
void CCoordinateSystemFormatConverter::Dispose()
{
    delete this;
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::DefinitionToWkt(MgCoordinateSystem* pSource, INT32 nWktFlavor)
{
    return pSource->ToString();
}

//*****************************************************************************
MgCoordinateSystem* CCoordinateSystemFormatConverter::WktToDefinition(INT32 nWktFlavor, CREFSTRING sWkt)
{
    Ptr<MgCoordinateSystem> pCsDef = new CCoordinateSystem(sWkt);

    return pCsDef.Detach();
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::CodeToWkt(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nWktFlavor)
{
    STRING sWkt;

    MG_TRY()

    if(MgCoordinateSystemCodeFormat::Epsg == nFormatSource)
    {
        // Make code uppercase
        STRING tempCode = ToUpper(sCodeSource);

        // If the code contains "EPSG:" it must be removed
        size_t position = tempCode.find(L"EPSG:", 0); // NOXLATE
        if(position != string::npos)
        {
            // Remove "EPSG:"
            tempCode = tempCode.erase(0, position+5);
        }

        long epsgCode = MgUtil::StringToInt32(tempCode);
        sWkt = CCoordinateSystem::ConvertEpsgCodeToWkt(epsgCode);
    }
    else
    {
        sWkt = CCoordinateSystem::ConvertCoordinateSystemCodeToWkt(sCodeSource);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.CodeToWkt")

    return sWkt;
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::WktToCode(INT32 nWktFlavor, CREFSTRING sWkt, INT32 nFormatDestination)
{
    STRING sCsCodeDestination;

    MG_TRY()

    if(MgCoordinateSystemCodeFormat::Epsg == nFormatDestination)
    {
        long epsgCode = CCoordinateSystem::ConvertWktToEpsgCode(sWkt);

        char buffer[255] = { 0 };
        #ifdef _WIN32
        itoa(epsgCode, buffer, 10);
        #else
        snprintf(buffer, 255, "%d", epsgCode);
        #endif
        wchar_t* strCode = Convert_Ascii_To_Wide(buffer);
        sCsCodeDestination = strCode;
        delete [] strCode;
    }
    else
    {
        sCsCodeDestination = CCoordinateSystem::ConvertWktToCoordinateSystemCode(sWkt);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.DefinitionToCode")

    return sCsCodeDestination;
}

//*****************************************************************************
//code format conversion
STRING CCoordinateSystemFormatConverter::DefinitionToCode(MgCoordinateSystem* pSource, INT32 nFormatDestination)
{
    return pSource->GetCode();
}

//*****************************************************************************
MgCoordinateSystem* CCoordinateSystemFormatConverter::CodeToDefinition(INT32 nFormatSource, CREFSTRING sCodeSource)
{
    Ptr<MgCoordinateSystem> pCsDestination;

    MG_TRY()

    // TODO - This roundtrip fails as it cannot identify some code, such as, EPSG:4012. 
    //        This code comes back with a blank code, description and category in the pCsDestination which is different then before!
    STRING wkt = CCoordinateSystem::ConvertCoordinateSystemCodeToWkt(sCodeSource);
    pCsDestination = new CCoordinateSystem(wkt);

    // TODO - Reset the code that can come back blank.
    if(NULL != pCsDestination.p)
    {
        pCsDestination->SetCode(sCodeSource);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.CodeToDefinition")

    return pCsDestination.Detach();
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::CodeToCode(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nFormatDestination)
{
    STRING sCsCodeDestination;

    MG_TRY()

    // TODO

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.CodeToCode")

    return sCsCodeDestination;
}

//*****************************************************************************
bool CCoordinateSystemFormatConverter::IsCodeInDictionary(INT32 nFormat, CREFSTRING sCode)
{
    bool bIsPresent=false;

    MG_TRY()

    // TODO

    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.IsCodeInDictionary")

    return bIsPresent;
}

//---------------------------------private stuff --------------------------------

enum ErcWktFlavor CCoordinateSystemFormatConverter::GetWktFlavor(INT32 nWktFlavor)
{
    enum ErcWktFlavor flavor=wktFlvrUnknown;
    switch(nWktFlavor)
    {
        case MgCoordinateSystemWktFlavor::Ogc:
            flavor=wktFlvrOgc;
            break;
        case MgCoordinateSystemWktFlavor::Geotiff:
            flavor=wktFlvrGeoTiff;
            break;
        case MgCoordinateSystemWktFlavor::Esri:
            flavor=wktFlvrEsri;
            break;
        case MgCoordinateSystemWktFlavor::Oracle:
            flavor=wktFlvrOracle;
            break;
        case MgCoordinateSystemWktFlavor::Geotools:
            flavor=wktFlvrGeoTools;
            break;
        case MgCoordinateSystemWktFlavor::Epsg:
            flavor=wktFlvrEpsg;
            break;
    }

    return flavor;
}

//*****************************************************************************
MgCoordinateSystem* CCoordinateSystemFormatConverter::GetCoordinateSystem(CREFSTRING sCsName)
{
    //get the appropriate def set interface
    Ptr<MgCoordinateSystemDictionary> pCsDict=m_pCatalog->GetCoordinateSystemDictionary();

    if (NULL == pCsDict.p)
    {
        throw new MgCoordinateSystemInitializationFailedException(
            L"MgCoordinateSystemFormatConverter.GetCoordinateSystem",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgGuardDisposable> disposableObj;
    Ptr<MgCoordinateSystem> coordinateSystem;

    //here we want to return NULL if we do not find it
    MG_TRY()

    disposableObj = pCsDict->Get(sCsName);
    coordinateSystem = SAFE_ADDREF(dynamic_cast<MgCoordinateSystem*>(disposableObj.p));

    MG_CATCH_AND_RELEASE()

    return coordinateSystem.Detach();
}

//*****************************************************************************
void CCoordinateSystemFormatConverter::ConvertArbitraryToWkt(MgCoordinateSystem* pCsDef, REFSTRING sWkt)
{
    if (NULL == pCsDef)
    {
        throw new MgNullArgumentException(
            L"CCoordinateSystemFormatConverter.ConvertArbitraryToWkt",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // This is an arbitrary XY system
    STRING csUnits = pCsDef->GetUnits();
    STRING units = L"";
    double unitsValue = 1.0;

    // Lookup the code in the arbitrary XY table and return the Ogc WKT
    for (int j = 0; j < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; j++)
    {
        if(_wcsicmp(csUnits.c_str(), CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].code) == 0)
        {
            units = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].unitsCode;
            unitsValue = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].conversionToMeters;
            break;
        }
    }

    if(units.length() > 0)
    {
        STRING unitsValueString;

        char buffer[255] = { 0 };
        sprintf(buffer, "%.10g", unitsValue);

        wchar_t* wBuffer = Convert_Ascii_To_Wide(buffer);
        if(wBuffer)
        {
            unitsValueString = wBuffer;
            delete [] wBuffer;
            wBuffer = NULL;
        }
        else
        {
            unitsValueString = L"1.0";
        }

        STRING datum;
        datum = L"LOCAL_DATUM[\"Local Datum\",0]";

        STRING axisX;
        axisX = L"AXIS[\"X\",EAST]";

        STRING axisY;
        axisY = L"AXIS[\"Y\",NORTH]";

        sWkt = L"LOCAL_CS";
        sWkt += L"[\"Non-Earth (";
        sWkt += units;
        sWkt += L")\",";
        sWkt += datum;
        sWkt += L",UNIT[\"";
        sWkt += units;
        sWkt += L"\", ";
        sWkt += unitsValueString;
        sWkt += L"],";
        sWkt += axisX;
        sWkt += L",";
        sWkt += axisY;
        sWkt += L"]";
    }
}

//*****************************************************************************
bool CCoordinateSystemFormatConverter::IsCoordinateSystem(CREFSTRING sCsName, char** ppCsDescription)
{
    if (ppCsDescription)
    {
        *ppCsDescription = NULL;
    }

    Ptr<MgCoordinateSystem> pCsDef=GetCoordinateSystem(sCsName);
    if (!pCsDef)
    {
        return false;
    }
    if (ppCsDescription)
    {
        //get the description
        STRING sDescription=pCsDef->GetDescription();
        *ppCsDescription = Convert_Wide_To_Ascii(sDescription.c_str());
    }
    return true;
}

//*****************************************************************************
bool CCoordinateSystemFormatConverter::IsCoordinateSystem(char *kpCsName, char** ppCsDescription)
{
    if (!kpCsName)
    {
        return false;
    }
    wchar_t* wszName=Convert_Ascii_To_Wide(kpCsName);
    STRING sCsName(wszName);
    delete[] wszName;
    return IsCoordinateSystem(sCsName, ppCsDescription);
}

//*****************************************************************************
void CCoordinateSystemFormatConverter::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//*****************************************************************************
void CCoordinateSystemFormatConverter::ReformatOldArbitraryName(
    std::string& strName)
{
    if (strName.c_str() != NULL && strName.length() > 0)
    {
        //remove the first character if it is a star
        if (strName[0]=='*')
        {
            strName.erase(0, 1);
        }
        //remove the last character if it is a star
        if (strName[strName.length()-1]=='*')
        {
            strName.erase(strName.length()-1, 1);
        }
    }
}

//*****************************************************************************
void CCoordinateSystemFormatConverter::ReformatOldArbitraryName(REFSTRING strName)
{
    if (strName.c_str() != NULL && strName.length() > 0)
    {
        //remove the first character if it is a star
        if (strName[0]==L'*')
        {
            strName.erase(0, 1);
        }
        //remove the last character if it is a star
        if (strName[strName.length()-1]==L'*')
        {
            strName.erase(strName.length()-1, 1);
        }
    }
}

//End of file.
