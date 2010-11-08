//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
// file MentorDictionary.h
//
// the public API operations involving ellipsoid, datum, and coordinate system
// dictionaries are nearly identical-- the only differences among the
// three file types are the magic number stored at the start, and the
// type of struct which is written to the file.  To avoid massive
// duplication of code, this file lays out some functions which will
// work with all three.  The functions are made generic through the
// use of template parameters and function pointers.
//

#ifndef MG_MENTORDICTIONARY_H
#define MG_MENTORDICTIONARY_H
#pragma once

#include "MentorUtil.h"
#include "Foundation.h"

#define DICTIONARY_SYS_DEF 1

namespace MentorDictionary
{
    void SetFileName(INT32& lMagic, CsDictionaryOpenMode (*ValidMagic)(long), CREFSTRING sDirectory, CREFSTRING sFileName, REFSTRING sFileNameSet, const wchar_t* kpMethodName);
    csFILE* Open(INT32& lMagic, CsDictionaryOpenMode (*ValidMagic)(long), const wchar_t* kpPath, CsDictionaryOpenMode nOpenMode);

    //Template function which generates a summary from a dictionary
    //file.  The caller is responsible for deleting the returned map.
    //Function returns NULL if it runs out of memory.  Note that this
    //is a fairly expensive function, timewise, since it involves
    //doing a complete scan of the entire file, which may be over
    //a megabyte in size.  Template parameter T is a Mentor struct
    //(cs_Csdef_ et al).
    //
    template <class T>
    CSystemNameDescriptionMap *
    GenerateSystemNameDescriptionMap(
        csFILE *pFile,
        const char* (*CS_Tkey)(const T&),
        const char * (*description)(const T&),
        int (*CS_Trd)(csFILE*, T *, int *))
    {
        //Make sure our file is open
        assert(pFile && CS_ftell(pFile)>=0);

        //Position the file pointer to the start of the file,
        //just past the magic number
        CS_fseek(pFile, sizeof(cs_magic_t), SEEK_SET);
        assert(!ferror(pFile));

        //Make a collection to return.
        CSystemNameDescriptionMap *pmapSystemNameDescription = new CSystemNameDescriptionMap;
        if (NULL == pmapSystemNameDescription) return NULL;

        //Scan through the file, inserting a summary entry
        //for each def we encounter.
        int nResult, nCrypt;
        T def;
        try
        {
            while ((nResult = CS_Trd(pFile, &def, &nCrypt)) > 0)
            {
                const char* keyName = CS_Tkey(def);

                pmapSystemNameDescription->insert(
                    CSystemNameDescriptionPair(
                        CSystemName(keyName),
                        CSystemDescription(description(def))
                    )
                );
            }    //for each def in the file
        }
        catch (std::bad_alloc)
        {
            delete pmapSystemNameDescription;
            return NULL;
        }
        if (nResult < 0)
        {
            //An error occurred while reading the file.
            delete pmapSystemNameDescription;
            return NULL;
        }

        //And we're done!  Return the collection we've built.
        return pmapSystemNameDescription;
    }

    #define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))

    //Template function for updating a def in a dictionary.
    //Works for ellipsoids, datums, and coordinate systems.
    //The bAlreadyExists parameter specifies whether there
    //must or must not already exist a def by that name.
    //T is a Mentor struct (cs_Csdef_ et al).  Tinterface
    //is the corresponding public API interface (MgCoordinateSystem
    //et al).
    //
    template <class T, class Tinterface>
    void UpdateDef(
        CSystemNameDescriptionMap *pmapSystemNameDescription,
        const char * (*key)(const T&),
        const char * (*description)(const T&),
        bool (Tinterface::*isValid)(),
        T * (*CS_Tdef)(const char *),
        int (*CS_Tupd)(T *, int),
        bool (*BuildDefFromInterface)(Tinterface *, T&),
        Tinterface *kpDef,
        bool bAlreadyExists)
    {
        return UpdateDef(pmapSystemNameDescription, key, description, isValid, CS_Tdef, CS_Tupd, BuildDefFromInterface, kpDef, bAlreadyExists, true);
    }

    template <class T, class Tinterface>
    void UpdateDef(
        CSystemNameDescriptionMap *pmapSystemNameDescription,
        const char * (*key)(const T&),
        const char * (*description)(const T&),
        bool (Tinterface::*isValid)(),
        T * (*CS_Tdef)(const char *),
        int (*CS_Tupd)(T *, int),
        bool (*BuildDefFromInterface)(Tinterface *, T&),
        Tinterface *kpDef,
        bool bAlreadyExists,
        bool verifyNotProtected)
    {
        assert(NULL != kpDef);
        if (NULL == kpDef)
        {
            throw new MgNullArgumentException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Make sure the def they've given us is valid
        if (NULL != isValid && !CALL_MEMBER_FN(kpDef, isValid)())
        {
            throw new MgInvalidArgumentException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Build a definition from the interface
        T def;
        bool bResult = BuildDefFromInterface(kpDef, def);
        if (!bResult)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const char* keyName = key(def);

        //Look in the dictionary
        bool bActuallyExists = false;
        INT16 sProtect = 0;

        //make sure, we've exclusive access to the file(s)
        SmartCriticalClass critical(true);

        T *pDef = CS_Tdef(keyName);
        if (NULL != pDef)
        {
            bActuallyExists = true;
            sProtect = pDef->protect;
            CS_free(pDef);
        }
        //Find out whether it's already present.  If we're requiring
        //that the object not already exist, we can simply check our
        //summary, if present.
        CSystemNameDescriptionMap::iterator iter;
        if (NULL != pmapSystemNameDescription)
        {
            iter = pmapSystemNameDescription->find(CSystemName(keyName));
            assert(bActuallyExists == (iter != pmapSystemNameDescription->end()));
        }

        //Make sure that whether it exists or not agrees
        //with what the caller specified
        if (bActuallyExists && !bAlreadyExists)
        {
            //It already exists, but it's not supposed to.
            throw new MgInvalidArgumentException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDuplicateException", NULL);
        }
        else if (!bActuallyExists && bAlreadyExists)
        {
            //It doesn't exist, but it's supposed to.
            STRING message = MgUtil::MultiByteToWideChar(string(keyName));
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgCoordinateSystemLoadFailedException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        if (verifyNotProtected)
        {
            //If it already exists, make sure it's not protected.
            if ((bAlreadyExists) && (IsReallyProtected(sProtect)))
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
            }
        }

        //Find out whether it's encrypted
        bool bEncrypted = kpDef->IsEncrypted();

        //Try to add it to the catalog.  The CS_Tupd function pointer
        //mimics the behavior of Mentor's CS_csupd(), CS_dtupd(), and
        //CS_elupd() functions, returning -1, 0, or 1 (see comments
        //below for meaning of return value).
        int nResult = CS_Tupd(&def, bEncrypted);
        switch (nResult)
        {
        case -1:
            //Some sort of Mentor error occurred.
            throw new MgCoordinateSystemInitializationFailedException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
            return;
        case 0:
            //The new object was added to the dictionary.
            //We need to add it to our summary, if present.
            assert(!bAlreadyExists);
            if (NULL != pmapSystemNameDescription)
            {
                try
                {
                    pmapSystemNameDescription->insert(
                        CSystemNameDescriptionPair(
                            CSystemName(keyName),
                            CSystemDescription(description(def))
                        )
                    );
                }
                catch (std::bad_alloc)
                {
                    throw new MgOutOfMemoryException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
                }
            }    //if summary is present
            break;
        case 1:
            //The object already existed in the dictionary,
            //and its definition was updated/replaced.
            //We need to update it in our summary, if present,
            //because the description may have changed.
            assert(bAlreadyExists);
            if (NULL != pmapSystemNameDescription)
            {
                assert(iter != pmapSystemNameDescription->end());
                //It's possible that the key name of the item itself
                //has changed, since key names are case-insensitive
                //and the caller could have (for example) changed it
                //from uppercase to lowercase without affecting its
                //presence or sort order.  If they've changed it,
                //we need to remove it from the list and re-insert
                //it.  (We can't just change the key in-place by
                //modifying (*iter).first, since the key in a std::map
                //pair is a const object.)
                if (0 == strcmp(keyName, (*iter).first.Name()))
                {
                    //The key name is unchanged; we can just update
                    //the summary.
                    (*iter).second = CSystemDescription(description(def));
                }
                else
                {
                    //The name changed (case changed only).  We need
                    //to delete the item and re-insert it.
                    assert(0 == CS_stricmp(keyName, (*iter).first.Name()));
                    try
                    {
                        pmapSystemNameDescription->erase(iter);
                        pmapSystemNameDescription->insert(
                            CSystemNameDescriptionPair(
                                CSystemName(keyName),
                                CSystemDescription(description(def))
                            )
                        );
                    }
                    catch (std::bad_alloc)
                    {
                        throw new MgOutOfMemoryException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }    //if the key name changed case
            }    //if a summary exists
            break;
        default:
            //This should never happen.
            assert(0);
            throw new MgUnclassifiedException(L"MentorDictionary.UpdateDef", __LINE__, __WFILE__, NULL, L"", NULL);
        }    //for each possible result of the update
    }


    //Template function for removing a def from a dictionary.
    //Works for ellipsoids, datums, and coordinate systems.
    //
    template <class T>
    void RemoveDef(
        CSystemNameDescriptionMap *pmapSystemNameDescription,
        T * (*CS_Tdef)(const char *),
        int (*CS_Tdel)(T *),
        const wchar_t* kpName)
    {
        //Make sure the specified definition is present
        char *pName = Convert_Wide_To_Ascii(kpName);    //need to delete [] pName;
        if (NULL == pName)
        {
            throw new MgOutOfMemoryException(L"MentorDictionary.RemoveDef", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        //Check our summary first, if present
        CSystemNameDescriptionMap::iterator iter;
        if (NULL != pmapSystemNameDescription)
        {
            iter = pmapSystemNameDescription->find(CSystemName(pName));
            if (iter == pmapSystemNameDescription->end())
            {
                STRING message = MgUtil::MultiByteToWideChar(string(pName));
                MgStringCollection arguments;
                arguments.Add(message);
                //Nope, it's not in our summary, so it ain't
                //there.  We can return failure without the
                //overhead of searching the disk file.
                delete [] pName;
                throw new MgCoordinateSystemLoadFailedException(L"MentorDictionary.RemoveDef", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
            }
        }
        //Get def from disk file.  We need to do this even if
        //we've verified its existence via the summary, above,
        //because we need to get its protect flag.
        SmartCriticalClass critical(true);
        T *pDef = CS_Tdef(pName);
        delete [] pName;
        if (NULL == pDef)
        {
            STRING message = MgUtil::MultiByteToWideChar(string(pName));
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgCoordinateSystemLoadFailedException(L"MentorDictionary.RemoveDef", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        //Make sure it's not protected
        if (IsReallyProtected(pDef->protect))
        {
            CS_free(pDef);
            throw new MgCoordinateSystemInitializationFailedException(L"MentorDictionary.RemoveDef", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
        }

        //Go ahead and attempt to delete it
        int nResult = CS_Tdel(pDef);
        CS_free(pDef);
        if (0 == nResult)
        {
            //Success!  Delete it from the summary as well,
            //if we have it
            if (NULL != pmapSystemNameDescription)
            {
                pmapSystemNameDescription->erase(iter);
            }
        }
        else
        {
            //Failed.
            throw new MgCoordinateSystemInitializationFailedException(L"MentorDictionary.RemoveDef", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
        }
    }



};    //namespace MentorDictionary


#endif //MG_MENTORDICTIONARY_H
