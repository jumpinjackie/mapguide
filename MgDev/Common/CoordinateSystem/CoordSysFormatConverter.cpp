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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysWktFailureCache.h"
#include "CoordSysFormatConverter.h"    //for CCoordinateSystemFormatConverter
#include "CoordSysUtil.h"               //for CCsNumericDotLocale
#include "MentorUtil.h"                 //for IsLegalMentorName()
#include "ArbitraryCoordsys.h"          //for CCsArbitraryCoordinateSystemUtil
#include "CoordSys.h"                   //for CCoordinateSystem

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
    STRING sWkt;
    cs_Csdef_ *pCsDef=NULL;
    cs_Dtdef_ *pDtDef=NULL;
    cs_Eldef_ *pElDef=NULL;

    MG_TRY()

    //numbers in WKT string always use a dot a separator but the locale might say otherwise
    CCsNumericDotLocale dotLocale;

    CHECKARGUMENTNULL(pSource, L"MgCoordinateSystemFormatConverter.DefinitionToWkt");

    if (wktFlvrUnknown==GetWktFlavor(nWktFlavor))
    {
        //invalid argument
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.DefinitionToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool bResult = BuildDefsFromInterface(pSource, pCsDef, pDtDef, pElDef);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFormatConverter.DefinitionToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (ProjectionIsNerthType (pCsDef->prj_knm))
    {
        //Get the coordinate system definition.
        ConvertArbitraryToWkt(pSource, sWkt);
    }
    else
    {
        char csWktBufr [2048];
        SmartCriticalClass critical(true);
        if (0==CScs2WktEx(csWktBufr,sizeof(csWktBufr),GetWktFlavor(nWktFlavor),pCsDef, pDtDef, pElDef,cs_WKTFLG_MAPNAMES))
        {
            wchar_t* pwszWkt=Convert_UTF8_To_Wide(csWktBufr);
            if (!pwszWkt)
            {
                throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.DefinitionToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            sWkt=pwszWkt;
            delete[] pwszWkt;
        }
    }
    MG_CATCH(L"MgCoordinateSystemFormatConverter.DefinitionToWkt")
    CS_free(pCsDef);
    CS_free(pDtDef);
    CS_free(pElDef);
    MG_THROW()

    return sWkt;
}

//*****************************************************************************
MgCoordinateSystem* CCoordinateSystemFormatConverter::WktToDefinition(INT32 nWktFlavor, CREFSTRING sWkt)
{
    Ptr<MgCoordinateSystem> pCsDef;
    char *pszWkt = NULL;

    // If we have recently failed on this very same sWkt string, we simply throw
    // the same exception that was generated when it failed the first time.
    CCoordinateSystemWktFailureCache* wktFailurePtr = CCoordinateSystemWktFailureCache::GetInstance ();
    if (wktFailurePtr != NULL)
    {
        if (wktFailurePtr->Has (sWkt))
        {
            MgException* mgExPtr = wktFailurePtr->Get (sWkt);
            if (mgExPtr != NULL)
            {
                // Note, the Get method of the failure cache does the AddRef.
                throw mgExPtr;
            }
        }
    }
    // OK, we haven't failed on this sWkt before; so we give it our best shot.

    MG_TRY()

    //numbers in WKT string always use a dot a separator but the locale might say otherwise
    CCsNumericDotLocale dotLocale;

    //NERTH projection stuff
    if (CCsArbitraryCoordinateSystemUtil::IsArbitrary(sWkt.c_str()))
    {
        cs_Csprm_ csprm;
        STRING error;
        bool bIsConverted=CCsArbitraryCoordinateSystemUtil::WktToCsmapCoordSys(sWkt.c_str(), &csprm, error);
        if (bIsConverted)
        {
            //if, by any chance, this system is in our dictionary, get the description from there
            //we do this for the arbitrary systems to get the correct description
            //that is not correct when it comes from the WKT itself
            std::string strArbitraryName(csprm.csdef.key_nm);
            ReformatOldArbitraryName(strArbitraryName);
            strcpy(csprm.csdef.key_nm, strArbitraryName.c_str());

            char* pCsDescription=NULL;
            bool bIsCoordinateSystem=IsCoordinateSystem(csprm.csdef.key_nm, &pCsDescription);
            if (NULL != pCsDescription)
            {
                if (bIsCoordinateSystem)
                {
                    strcpy(csprm.csdef.desc_nm, pCsDescription);
                }

                delete[] pCsDescription;
            }

            //Construct our object
            Ptr<CCoordinateSystem> pNew = new CCoordinateSystem(m_pCatalog);
            if (NULL == pNew.p)
            {
                throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            else if (!pNew->InitArbitrary(csprm))
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            return pNew.Detach();
        }
    }

    pszWkt = Convert_Wide_To_UTF8(sWkt.c_str());
    if (NULL == pszWkt)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    struct cs_Csdef_ csDef;
    struct cs_Dtdef_ dtDef;
    struct cs_Eldef_ elDef;

    //if wktFlvrNone, the Mentor API will determine the flavor automatically
    ErcWktFlavor flavor=GetWktFlavor(nWktFlavor);
    if (wktFlvrUnknown==flavor)
    {
        flavor=wktFlvrNone;
    }

    //IMPORTANT
    //bRunBinaryFallback could be false, and make the code run faster, to not run the
    //binary fallback against coordsys.csd
    //because in this API we just care about having a valid CS definition
    //we don't care about having an actual defintion inside the Mentor dictionary
    //But we set the value to true because we need that fallback
    //The reason is we might manage to generate a CS definition that has some so called
    //Mentor illigal character in the names of the EL, DT and/or CS
    //Such character is a parenthesis for instance
    //like the following WKT would generate a datum name that would be "Bellevue (IGN)" is the fallback is not applied
    //GEOGCS [ "Longitude / Latitude (Bellevue)", DATUM ["Bellevue (IGN)", SPHEROID ["International 1924", 6378388, 297]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]]
    //The datum name "Bellevue (IGN)" would then fail in CCoordinateSystem::SetDatumDefinition
    //when CS_nampp is called with that name and we then fail to create the CS definition
    //One might care about this kind of restriction because we might end-up with a system we are interested in writing to coordsys.csd
    //and that's when Mentor would clash
    //Try to work on this in the future to allow a higher rate of success concerning WKT -> CS
    //that would mean commenting out calls to CS_nampp in InitGeodetic and InitNonGeodetic
    //that would also mean we might fail later on if we want to save that system
    //but we would at least have a usable custom system
    int nMentorAPI = -1;
    CriticalClass.Enter();
    bool bRunBinaryFallback=true;
    nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,flavor,pszWkt,bRunBinaryFallback);

    //sometimes the flavor detector sucks so we have to manually set the flavor
    if (wktFlvrNone==flavor
        && 0!=nMentorAPI)
    {
        nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrOgc,pszWkt,bRunBinaryFallback);
        if (0!=nMentorAPI)
        {
            nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrEsri,pszWkt,bRunBinaryFallback);
            if (0!=nMentorAPI)
            {
                nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrOracle,pszWkt,bRunBinaryFallback);
                if (0!=nMentorAPI)
                {
                    nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrGeoTiff,pszWkt,bRunBinaryFallback);
                    if (0!=nMentorAPI)
                    {
                        nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrGeoTools,pszWkt,bRunBinaryFallback);
                        if (0!=nMentorAPI)
                        {
                            nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrEpsg,pszWkt,bRunBinaryFallback);
                        }
                    }
                }
            }
        }
    }
    CriticalClass.Leave();

    if (0==nMentorAPI)
    {
        //make the names legal if they are not
        //might might very well have a mathematically valid custom system
        //one that is not in our dictiornaries
        //but its name has been generated from was found in the WKT string
        //this name might have charcaters not allowed in Mentor
        //In this case we'll failt just because of the name
        //The system is custom anyway and its name is unknown to us so lets just make it valid
        if (!IsLegalMentorName(csDef.key_nm))
        {
            MakeLegalMentorName(csDef.key_nm);
        }
        if (!IsLegalMentorName(dtDef.key_nm))
        {
            MakeLegalMentorName(dtDef.key_nm);
            //if (csDef.dat_knm[0] != '\0') //don't check that, if we have a datum, it is a geodetic system
            CS_stncp (csDef.dat_knm, dtDef.key_nm, cs_KEYNM_DEF);
        }
        if (!IsLegalMentorName(elDef.key_nm))
        {
            MakeLegalMentorName(elDef.key_nm);
            if (dtDef.key_nm[0] != '\0')
            {
                //geodetic system
                CS_stncp (dtDef.ell_knm, elDef.key_nm, cs_KEYNM_DEF);
            }
            else //if (csDef.elp_knm[0] != '\0')
            {
                //non-geodetic system
                CS_stncp (csDef.elp_knm, elDef.key_nm, cs_KEYNM_DEF);
            }
        }

        //it is possible at this stage that the system that we have is one system from our
        //dictionary files. The Mentor API will have generated a valid mathematical definitions
        //but parameters like description or group might not be initialized as they would be if
        //the system had been loaded from the dictionary file
        //So, if one expects to have a description that matches the one in our dictionaries we need
        //to updat it here
        wchar_t* pswCsName=Convert_UTF8_To_Wide(csDef.key_nm);
        wchar_t* pswDtName=NULL;
        wchar_t* pswElName=NULL;
        STRING sCsNameFromDict, sDtNameFromDict, sElNameFromDict;

        if (dtDef.key_nm[0] != '\0')
        {
            pswDtName=Convert_UTF8_To_Wide(dtDef.key_nm);
        }
        if (elDef.key_nm[0] != '\0')
        {
            pswElName=Convert_UTF8_To_Wide(elDef.key_nm);
        }
        try
        {
            Ptr<MgCoordinateSystem> pCsFromDict=GetCoordinateSystem(pswCsName);

            if (NULL != pCsFromDict.p)
            {
                sCsNameFromDict=pCsFromDict->GetCsCode();
                sDtNameFromDict=pCsFromDict->GetDatum();
                sElNameFromDict=pCsFromDict->GetEllipsoid();

                if (sCsNameFromDict==pswCsName &&
                    (pswDtName && sDtNameFromDict==pswDtName) &&
                    (pswElName && sElNameFromDict==pswElName))
                {
                    //we just loaded it so it could be a protected system
                    //remove that protection to make it behave like if
                    //it was coming from the Mentor API as a custom system
                    //In any case if we try later on to add this system to
                    //the dictionary we will fail because it is protected
                    //in the dictionary file
                    pCsFromDict->SetProtectMode(false);
                    delete[] pswCsName;
                    delete[] pswDtName;
                    delete[] pswElName;

                    delete[] pszWkt;

                    //this is the one from the dictionary, so return it
                    return pCsFromDict.Detach();
                }
            }
        }
        catch (MgException* pEFromDict)
        {
            //end up here if we could not load the cs, dt or el from our dictionaries
            //that would mean the system generated from the WKT is a custom system
            //and in that case we cannot initialized the group, description and other
            //text information parameters
            pEFromDict->Release();
        }

        delete[] pswCsName;
        delete[] pswDtName;
        delete[] pswElName;

        //creates the coordsys defintion
        //don't ask the function to load the datum from the catalog, we'll set it later
        pCsDef=BuildInterfaceFromCsDef(csDef, NULL);

        //ellipsoid defintion
        Ptr<MgCoordinateSystemEllipsoid> pElDef=BuildInterfaceFromElDef(elDef, m_pCatalog);

        //make a coordsys object
        if (!pCsDef || !pElDef)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //do we have a datum?
        if (CsdefIsGeodetic(csDef))
        {
            //don't ask the function to load the ellipsoid from the catalog, we'll set it later
            Ptr<MgCoordinateSystemDatum> pDtDef=BuildInterfaceFromDtDef(dtDef, NULL);
            if (!pDtDef)
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            pDtDef->SetEllipsoidDefinition(pElDef);
            pCsDef->SetDatumDefinition(pDtDef);

            //Okay, got it!  We're all set.  Return the coordsys
            //object.  Caller is responsible for freeing it via
            //Release().
        }
        //We just have an ellipsoid
        //Looking at the code of the method
        //int EXP_LVL1 CS_cs2Wkt (char *bufr,size_t bufrSize,const char *csKeyName,int flavor)
        //in the file cscs2wkt.c in Mentor, all non geodetic systems fail to be converted to WKT
        //returning the error cs_WKT_NODTREF
        //Since I don't know if this is a first version or a fact that these systems can't be converted
        //I add the following code to treat the non-geodetic systems if it becomes possible in the future
        else
        {
            pCsDef->SetEllipsoidDefinition(pElDef);
            //Okay, got it!  We're all set.  Return the coordsys
            //object.  Caller is responsible for freeing it via
            //Release().
        }
    }

    CCoordinateSystem * pCoordSys = dynamic_cast<CCoordinateSystem*>(pCsDef.p);
    if (NULL == pCoordSys)
    {
        // Could not create the coordinate system from the WKT
        MgStringCollection arguments;
        arguments.Add(sWkt);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemFormatConverter.WktToDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemFailedToCreateCoordinateSystemFromWkt", &arguments);
    }
    else
    {
        pCoordSys->SetCatalog(m_pCatalog);
    }

    MG_CATCH(L"MgCoordinateSystemFormatConverter.WktToDefinition")

    delete[] pszWkt;

    // Note: the MG_TRY macro at the top of this method includes the following declaration:
    //      Ptr<MgException> mgException;
    // The MG_CATCH macro immediately above catches all exceptions and manufactures a MgException
    // and essentially sets mgException to the manufactured exception.  Thus, the following applies
    // to any and all exceptions thrown during the processing of this WKT string.
    if (mgException != NULL && wktFailurePtr != NULL)
    {
        // Note, the Set method of the failure cache does the AddRef.  We should
        // never get here is the sWkt has already been added to the failure cache,
        // but if this sWkt is already in the cache, the Set function will release
        // the old exception and replace it with the current exception.
        wktFailurePtr->Set (sWkt,mgException);
    }
    // OK, the failure has been cached, we throw the exception in the normal manner
    // (if such exception exists).

    MG_THROW()

    return pCsDef.Detach();
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::CodeToWkt(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nWktFlavor)
{
    STRING sWkt;

    MG_TRY()

    //numbers in WKT string always use a dot a separator but the locale might say otherwise
    CCsNumericDotLocale dotLocale;

    if (wktFlvrUnknown==GetWktFlavor(nWktFlavor))
    {
        //invalid argument
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    std::string szMsiName;
    std::string strName;

    //if input is EPSG
    if (MgCoordinateSystemCodeFormat::Epsg==nFormatSource)
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

        long lEpsg=wcstol(tempCode.c_str(), NULL, 10);
        if (0==lEpsg)
        {
            return L"";
        }
        else
        {
            //output format is Msi
            //It is the Adsk name but is really the Msi name
            //viewed from the outside of public API
            //The mapping from Adsk name to Msi is done internally inside
            //the method CSepsg2adskCS
            CriticalClass.Enter();
            const char* szMsiNameTemp=NULL;
            szMsiNameTemp=CSepsg2adskCS(lEpsg);
            if(NULL == szMsiNameTemp)
            {
                szMsiName = "";
            }
            else
            {
                szMsiName = szMsiNameTemp;
            }
            CriticalClass.Leave();
            if (szMsiName.empty())
            {
                return L"";
            }
        }
    }
    else if (MgCoordinateSystemCodeFormat::Mentor==nFormatSource)
    {
        char *pszCsSource = Convert_Wide_To_UTF8(sCodeSource.c_str());
        if (!pszCsSource)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        strName=pszCsSource;
        delete[] pszCsSource;
        ReformatOldArbitraryName(strName);
        szMsiName=strName;
        if (szMsiName.empty())
        {
            return L"";
        }
    }
    else
    {
        //invalid source format
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //now get the WKT
    char csWktBufr [2048];

    //is it an arbitrary system, one that uses NERTH projection?
    wchar_t* wszMsiName=Convert_UTF8_To_Wide(szMsiName.c_str());
    if (!wszMsiName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING strMsiName(wszMsiName);
    delete[] wszMsiName;
    Ptr<MgCoordinateSystem> pCsCoordinateSystemDef=GetCoordinateSystem(strMsiName);
    if (!pCsCoordinateSystemDef)
    {
        MgStringCollection arguments;
        arguments.Add(strMsiName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT32 prj=pCsCoordinateSystemDef->GetProjectionCode();
    if (ProjectionIsNerthType (prj))
    {
        ConvertArbitraryToWkt(pCsCoordinateSystemDef, sWkt);
    }
    else
    {
        SmartCriticalClass critical(true);
        if (0==CS_cs2Wkt(csWktBufr,sizeof(csWktBufr),szMsiName.c_str(),GetWktFlavor(nWktFlavor)))
        {
            wchar_t* wszWkt=Convert_UTF8_To_Wide(csWktBufr);
            if (!wszWkt)
            {
                throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            sWkt=wszWkt;
            delete[] wszWkt;
        }
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemFormatConverter.CodeToWkt")

    return sWkt;
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::WktToCode(INT32 nWktFlavor, CREFSTRING sWkt, INT32 nFormatDestination)
{
    STRING sCsCodeDestination;
    char *pszWkt = NULL;

    // If we have recently failed on this very same sWkt string, we simply throw
    // the same exception that was generated when it failed the first time.
    CCoordinateSystemWktFailureCache* wktFailurePtr = CCoordinateSystemWktFailureCache::GetInstance ();
    if (wktFailurePtr != NULL)
    {
        if (wktFailurePtr->Has (sWkt))
        {
            MgException* mgExPtr = wktFailurePtr->Get (sWkt);
            if (mgExPtr != NULL)
            {
                // Note, the Get method of the failure cache does the AddRef.
                throw mgExPtr;
            }
        }
    }
    // OK, we haven't failed on this sWkt before; so we give it our best shot.

    MG_TRY()

    int nMentorAPI=0;
    const char* szMsiName=NULL;
    cs_Csprm_ csprm;
    struct cs_Csdef_ csDef;

    pszWkt = Convert_Wide_To_UTF8(sWkt.c_str());
    if (NULL == pszWkt)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.WktToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //numbers in WKT string always use a dot a separator but the locale might say otherwise
    CCsNumericDotLocale dotLocale;

    //NERTH projection stuff
    std::string strName;
    bool bIsArbitrary=CCsArbitraryCoordinateSystemUtil::IsArbitrary(sWkt.c_str());
    if (bIsArbitrary)
    {
        STRING error;
        bool bIsConverted=CCsArbitraryCoordinateSystemUtil::WktToCsmapCoordSys(sWkt.c_str(), &csprm, error);
        if (bIsConverted)
        {
            //old MapGuide code supported arbitrary names with the format *XY-unit*
            //we droppepd the stars because Mentor was screaming but we still have
            //files using the old name and we need to convert those old formats
            strName=csprm.csdef.key_nm;
            ReformatOldArbitraryName(strName);
            szMsiName=strName.c_str();
        }
    }
    else
    {
        //if wktFlvrNone, the Mentor API will determine the flavor automatically
        ErcWktFlavor flavor=GetWktFlavor(nWktFlavor);
        if (wktFlvrUnknown==flavor)
        {
            flavor=wktFlvrNone;
        }

        struct cs_Dtdef_ dtDef;
        struct cs_Eldef_ elDef;

        //bRunBinaryFallback is true to run the binary fallback against coordsys.csd
        //in this API we DO care about having a valid CS definition
        //inside the Mentor dictionary
        bool bRunBinaryFallback=true;
        CriticalClass.Enter();
        nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,flavor,pszWkt,bRunBinaryFallback);

        //sometimes the flavor detector sucks so we have to manually set the flavor
        if (wktFlvrNone==flavor
            && 0!=nMentorAPI)
        {
            nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrOgc,pszWkt,bRunBinaryFallback);
            if (0!=nMentorAPI)
            {
                nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrEsri,pszWkt,bRunBinaryFallback);
                if (0!=nMentorAPI)
                {
                    nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrOracle,pszWkt,bRunBinaryFallback);
                    if (0!=nMentorAPI)
                    {
                        nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrGeoTiff,pszWkt,bRunBinaryFallback);
                        if (0!=nMentorAPI)
                        {
                            nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrGeoTools,pszWkt,bRunBinaryFallback);
                            if (0!=nMentorAPI)
                            {
                                nMentorAPI=CS_wktToCsEx (&csDef,&dtDef,&elDef,wktFlvrEpsg,pszWkt,bRunBinaryFallback);
                            }
                        }
                    }
                }
            }
        }
        CriticalClass.Leave();

        if (0==nMentorAPI)
        {
            szMsiName=csDef.key_nm;
        }
    }

    if (0==nMentorAPI)
    {
        //need to check if the system is in our dictionary
        bool bIsCoordinateSystem=IsCoordinateSystem(const_cast<char*>(szMsiName), NULL);
        if (bIsCoordinateSystem)
        {
            const char* pszCsDestination=szMsiName;

            //treat various output formats
            if (MgCoordinateSystemCodeFormat::Mentor==nFormatDestination)
            {
                wchar_t* pwszCsDestination=Convert_UTF8_To_Wide(pszCsDestination);
                if (pwszCsDestination)
                {
                    sCsCodeDestination=pwszCsDestination;
                    delete[] pwszCsDestination;
                }
            }
            else if (MgCoordinateSystemCodeFormat::Epsg==nFormatDestination)
            {
                //the input is a coordinate system name
                //It is the Adsk name but is really the Msi name
                //viewed from the outside of the public API
                //The mapping from Adsk name to Msi is done internally inside
                //the method CSadsk2epsgCS
                long lEpsg=CSadsk2epsgCS(pszCsDestination);
                if (0L!=lEpsg)
                {
                    wchar_t szEpsg[100];
                    swprintf(szEpsg, 100, L"%ld", lEpsg);
                    sCsCodeDestination=szEpsg;
                }
            }
            else
            {
                //invalid format
                throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.WktToCode", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }
    MG_CATCH(L"MgCoordinateSystemFormatConverter.WktToCode")
    delete [] pszWkt;

    // Note: the MG_TRY macro at the top of this method includes the following declaration:
    //      Ptr<MgException> mgException;
    // The MG_CATCH macro immediately above catches all exceptions and manufactures a MgException
    // and essentially sets mgException to the manufactured exception.  Thus, the following applies
    // to any and all exceptions thrown during the processing of this WKT string.
    if (mgException != NULL && wktFailurePtr != NULL)
    {
        // Note, the Set method of the failure cache does the AddRef.  We should
        // never get here is the sWkt has already been added to the failure cache,
        // but if this sWkt is already in the cache, the Set function will release
        // the old exception and replace it with the current exception.
        wktFailurePtr->Set (sWkt,mgException);
    }
    // OK, the failure has been cached, we throw the exception in the normal manner
    // (if such exception exists).
    MG_THROW()

    return sCsCodeDestination;
}

//*****************************************************************************
//code format conversion
STRING CCoordinateSystemFormatConverter::DefinitionToCode(MgCoordinateSystem* pSource, INT32 nFormatDestination)
{
    STRING sCsCodeDestination;
    char *pszCsSource = NULL;

    MG_TRY()

    CHECKARGUMENTNULL(pSource, L"MgCoordinateSystemFormatConverter.DefinitionToCode");

    //get the name of the source system
    STRING sCsSource=pSource->GetCsCode();
    if (sCsSource.empty())
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.DefinitionToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pszCsSource = Convert_Wide_To_UTF8(sCsSource.c_str());
    if (NULL == pszCsSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.DefinitionToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //need to check if the system is in our dictionary
    bool bIsCoordinateSystem=IsCoordinateSystem(pszCsSource, NULL);
    if (!bIsCoordinateSystem)
    {
        delete [] pszCsSource;
        return L"";
    }

    //treat various output formats
    if (MgCoordinateSystemCodeFormat::Mentor==nFormatDestination)
    {
        //stupid case but better than returning E_INVALIDARG
        wchar_t* pwszCsDestination=Convert_UTF8_To_Wide(pszCsSource);
        if (pwszCsDestination)
        {
            sCsCodeDestination=pwszCsDestination;
            delete[] pwszCsDestination;
        }
    }
    else if (MgCoordinateSystemCodeFormat::Epsg==nFormatDestination)
    {
        //the input is a coordinate system name
        //It is the Adsk name but is really the Msi name
        //viewed from the outside of the public API
        //The mapping from Adsk name to Msi is done internally inside
        //the method CSadsk2epsgCS
        long lEpsg=CSadsk2epsgCS(pszCsSource);
        if (0L!=lEpsg)
        {
            wchar_t szEpsg[100];
            swprintf(szEpsg, 100, L"%ld", lEpsg);

            sCsCodeDestination=szEpsg;
        }
    }
    else
    {
        //invalid format
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.DefinitionToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH(L"MgCoordinateSystemFormatConverter.DefinitionToCode")
    delete [] pszCsSource;
    MG_THROW()

    return sCsCodeDestination;
}

//*****************************************************************************
MgCoordinateSystem* CCoordinateSystemFormatConverter::CodeToDefinition(INT32 nFormatSource, CREFSTRING sCodeSource)
{
    char *pszCsSource = NULL;
    Ptr<MgCoordinateSystem> pCsDestination;

    MG_TRY()

    pszCsSource = Convert_Wide_To_UTF8(sCodeSource.c_str());
    if (NULL == pszCsSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    std::string szMsiName;

    //if input is EPSG
    bool bResult=false;
    if (MgCoordinateSystemCodeFormat::Epsg==nFormatSource)
    {
        long lEpsg=wcstol(sCodeSource.c_str(), NULL, 10);
        if (0!=lEpsg)
        {
            //output format is Msi
            //It is the Adsk name but is really the Msi name
            //viewed from the outside of the public API
            //The mapping from Adsk name to Msi is done internally inside
            //the method CSepsg2adskCS
            CriticalClass.Enter();
            const char* szMsiNameTemp=NULL;
            szMsiNameTemp=CSepsg2adskCS(lEpsg);
            if(NULL == szMsiNameTemp)
            {
                szMsiName = "";
            }
            else
            {
                szMsiName = szMsiNameTemp;
            }
            CriticalClass.Leave();
            if (!szMsiName.empty())
            {
                bResult=true;
            }
        }
    }
    else if (MgCoordinateSystemCodeFormat::Mentor==nFormatSource)
    {
        szMsiName=pszCsSource;
        bResult=true;
    }
    else
    {
        //invalid format
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.CodeToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //now get the definition
    if (bResult)
    {
        assert(!szMsiName.empty());

        wchar_t* wszMsiName=Convert_UTF8_To_Wide(szMsiName.c_str());
        if (!wszMsiName)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING sMsiName(wszMsiName);
        delete[] wszMsiName;
        pCsDestination=GetCoordinateSystem(sMsiName);
    }

    if (!pCsDestination)
    {
        // Could not create the coordinate system from the code
        MgStringCollection arguments;
        arguments.Add(sCodeSource);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemFormatConverter.CodeToDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemFailedToCreateCoordinateSystemFromCode", &arguments);
    }

    MG_CATCH(L"MgCoordinateSystemFormatConverter.CodeToDefinition")
    delete [] pszCsSource;
    MG_THROW()

    return pCsDestination.Detach();
}

//*****************************************************************************
STRING CCoordinateSystemFormatConverter::CodeToCode(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nFormatDestination)
{
    char *pszCsSource = NULL;
    STRING sCsCodeDestination;

    MG_TRY()

    pszCsSource = Convert_Wide_To_UTF8(sCodeSource.c_str());
    if (NULL == pszCsSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //if input is EPSG
    if (MgCoordinateSystemCodeFormat::Epsg==nFormatSource)
    {
        long lEpsg=wcstol(sCodeSource.c_str(), NULL, 10);
        if (0!=lEpsg)
        {
            //treat various output formats
            if (MgCoordinateSystemCodeFormat::Mentor==nFormatDestination)
            {
                //the output is a coordinate system name
                //It is the Adsk name but is really the Msi name
                //viewed from the outside of the public API
                //The mapping from Adsk name to Msi is done internally inside
                //the method CSepsg2adskCS
                std::string szMsiName;
                CriticalClass.Enter();
                const char* szMsiNameTemp=NULL;
                szMsiNameTemp=CSepsg2adskCS(lEpsg);
                if(NULL == szMsiNameTemp)
                {
                    szMsiName = "";
                }
                else
                {
                    szMsiName = szMsiNameTemp;
                }
                CriticalClass.Leave();
                if (!szMsiName.empty())
                {
                    //need to check if the system is in our dictionary
                    bool bIsCoordinateSystem=IsCoordinateSystem(const_cast<char*>(szMsiName.c_str()), NULL);
                    if (bIsCoordinateSystem)
                    {
                        wchar_t *pwszCsDestination=Convert_UTF8_To_Wide(szMsiName.c_str());
                        if (!pwszCsDestination)
                        {
                            throw new MgOutOfMemoryException(L"MgCoordinateSystemFormatConverter.CodeToCode", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                        sCsCodeDestination=pwszCsDestination;
                        delete[] pwszCsDestination;
                    }
                }
            }
            //stupid case, the destination is the same as the source
            else if (MgCoordinateSystemCodeFormat::Epsg==nFormatDestination)
            {
                sCsCodeDestination=sCodeSource;
            }
            else
            {
                //invalid format
                throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.CodeToCode", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }
    else if (MgCoordinateSystemCodeFormat::Mentor==nFormatSource)
    {
        //need to check if the system is in our dictionary
        bool bIsCoordinateSystem=IsCoordinateSystem(pszCsSource, NULL);
        if (bIsCoordinateSystem)
        {
            //treat various output formats
            if (MgCoordinateSystemCodeFormat::Epsg==nFormatDestination)
            {
                //the input is a coordinate system name
                //It is the Adsk name but is really the Msi name
                //viewed from the outside of the public API
                //The mapping from Adsk name to Msi is done internally inside
                //the method CSadsk2epsgCS
                long lEpsg=CSadsk2epsgCS(pszCsSource);
                if (0L!=lEpsg)
                {
                    wchar_t szEpsg[100];
                    swprintf(szEpsg, 100, L"%ld", lEpsg);
                    sCsCodeDestination=szEpsg;
                }
            }
            //stupid case, the destination is the same as the source
            else if (MgCoordinateSystemCodeFormat::Mentor==nFormatDestination)
            {
                sCsCodeDestination=sCodeSource;
            }
        }
    }
    else
    {
        //invalid format
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.CodeToCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH(L"MgCoordinateSystemFormatConverter.CodeToCode")
    delete [] pszCsSource;
    MG_THROW()

    return sCsCodeDestination;
}

//*****************************************************************************
bool CCoordinateSystemFormatConverter::IsCodeInDictionary(INT32 nFormat, CREFSTRING sCode)
{
    bool bIsPresent=false;

    MG_TRY()

    //if input is EPSG
    if (MgCoordinateSystemCodeFormat::Epsg==nFormat)
    {
        long lEpsg=wcstol(sCode.c_str(), NULL, 10);
        if (0!=lEpsg)
        {
            //the output is a coordinate system name
            //It is the Adsk name but is really the Msi name
            //viewed from the outside of the public API
            //The mapping from Adsk name to Msi is done internally inside
            //the method CSepsg2adskCS
            std::string szMsiName;
            CriticalClass.Enter();
            const char* szMsiNameTemp=NULL;
            szMsiNameTemp=CSepsg2adskCS(lEpsg);
            if(NULL == szMsiNameTemp)
            {
                szMsiName = "";
            }
            else
            {
                szMsiName = szMsiNameTemp;
            }
            CriticalClass.Leave();
            if (!szMsiName.empty())
            {
                //we may have found an Msi name for a system
                //but we still have to check if the system is in the dicionary
                //in CS_epsg.c I found some systems that were present in the mapping
                //but not in the dictionary
                //Normally CSepsg2adskCS should return a result only
                //if the system is in the dictionary since it takes care
                //of mapping an MSI name to a valid ADSK name
                //We'll check the existence of the system for additional security
                bIsPresent=IsCoordinateSystem(const_cast<char*>(szMsiName.c_str()), NULL);
            }
        }
    }
    else if (MgCoordinateSystemCodeFormat::Mentor==nFormat)
    {
        bIsPresent=IsCoordinateSystem(sCode, NULL);
    }
    else
    {
        //invalid format
        throw new MgInvalidArgumentException(L"MgCoordinateSystemFormatConverter.IsCodeInDictionary", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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
            __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDictionaryException", NULL);
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
    //numbers in WKT string always use a dot a separator but the locale might say otherwise
    CCsNumericDotLocale dotLocale;

    cs_Csdef_ def;
    bool bResult=BuildCsDefFromInterface(pCsDef, def);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemFormatConverter.ConvertArbitraryToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    char* szWkt=CCsArbitraryCoordinateSystemUtil::ConvertCoordinateSystemToWkt(&def);
    if (szWkt)
    {
        wchar_t* pwszWkt=Convert_UTF8_To_Wide(szWkt);
        sWkt=pwszWkt;
        delete[] szWkt;
        delete[] pwszWkt;
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
        *ppCsDescription = Convert_Wide_To_UTF8(sDescription.c_str());
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
    wchar_t* wszName=Convert_UTF8_To_Wide(kpCsName);
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
