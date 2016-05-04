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

#include <memory>

#include "MentorUtil.h"
#include "Foundation.h"

#define DICTIONARY_SYS_DEF 1

#define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))

namespace MentorDictionary
{
    template<class TCsMap, class TContainer >
    class TContainerPtr : public auto_ptr<TContainer>
    {
    public:
        TContainerPtr()
            : auto_ptr<TContainer>()
        {
        }

        TContainerPtr(TContainer* pContainer)
            : auto_ptr<TContainer>(pContainer)
        {
        }

        ~TContainerPtr()
        {
            TContainer* pContainer = auto_ptr<TContainer>::get();
            if (NULL == pContainer)
                return;

            for(typename TContainer::iterator iter = pContainer->begin();
                iter != pContainer->end(); ++iter)
            {
                if (NULL != *iter)
                    CS_free(*iter);
                *iter = NULL;
            }
        }
    };


    void SetFileName(INT32& lMagic, CsDictionaryOpenMode (*ValidMagic)(long), CREFSTRING sDirectory, CREFSTRING sFileName, REFSTRING sFileNameSet, const wchar_t* kpMethodName);
    csFILE* Open(INT32& lMagic, CsDictionaryOpenMode (*ValidMagic)(long), const wchar_t* kpPath, CsDictionaryOpenMode nOpenMode);
    CsDictionaryOpenMode OpenReadDictionaryOpenCallback(long dictMagicNumber);

    //reads the next 'CsMap' entry from the file passed in by [pFile]
    //the file is assumed to have been opened already
    //returns:
    // [true]: if an entry has been read
    // [false]: if no entry has been read, i.e. EOF has been reached
    //
    //throws an [MgFileIoException] for any other condition, e.g. CsMap has encountered unexpected content
    template <class T>
    bool GetNextEntry(csFILE *pFile, T& def, int& nCrypt, int (*CS_Trd)(csFILE*, T *, int *))
    {
        int nResult = CS_Trd(pFile, &def, &nCrypt);
        if (nResult > 0) //read successfull
            return true;

        if (0 == nResult) //we've reached the file's end
            return false;

        //i.e. nResult < 0
        throw new MgFileIoException(L"MentorDictionary.GetNextEntry", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //iterates through all entries return a vector of definitions in the same order as they've been returned by CsMap
    //return NULL, if the file couldn't be read successfully
    template <class T>
    vector<T*>*
    ReadDictionaryEntries(
        csFILE *pFile,
        int (*CS_Trd)(csFILE*, T *, int *))
    {
        //Scan through the file - add an entry for each definition we find
        try
        {
            TContainerPtr<T, vector<T*> > allDictEntries;
            allDictEntries.reset(new vector<T*>);

            int nCrypt;
            T* def = NULL;

            def = (T*) CS_malc(sizeof(T));
            if (NULL == def)
                throw new MgOutOfMemoryException(L"MentorDictionary.ReadDictionaryEntries", __LINE__, __WFILE__, NULL, L"", NULL);

            while (MentorDictionary::GetNextEntry(pFile, *def, nCrypt, CS_Trd))
            {
                allDictEntries->push_back(def);

            } //for each def in the file

            return allDictEntries.release();
        }
        catch(MgFileIoException* ioException)
        {
            ioException->Release();
        }
        catch (std::bad_alloc&)
        {
        }

        return NULL;
    }

    //opens a dictionary file for reading and then iterates over all entries and returns them as a vector; the order is the same as returned by CsMap
    //before accessing the file, a global lock is placed through an [SmartCriticalClass] object
    template <class T>
    vector<T*>* ReadDictionaryEntries(
        /*IN*/ MgCoordinateSystemDictionaryBase* targetDictionary,
        /*IN*/ int (*CS_Trd)(csFILE*, T *, int *))
    {
    typedef vector<T*> CsMapDefPtrVector;
        
    TContainerPtr<T, CsMapDefPtrVector> allDefs;
        csFILE *pFile = NULL;

        MG_TRY()

        INT32 dictionaryMagicNumber;

        SmartCriticalClass critical(true);

        STRING strPath = targetDictionary->GetPath();
        pFile = MentorDictionary::Open(dictionaryMagicNumber, OpenReadDictionaryOpenCallback, strPath.c_str(), Read);

        allDefs.reset(MentorDictionary::ReadDictionaryEntries<T>(pFile, CS_Trd));
        if (NULL == allDefs.get())
            throw new MgInvalidArgumentException(L"MentorDictionary.ReadDictionaryEntries", __LINE__, __WFILE__, NULL, L"", NULL);

        MG_CATCH(L"MentorDictionary.ReadDictionaryEntries")

        bool fileClosed = (0 == CS_fclose(pFile));
        pFile = NULL;

        if (!fileClosed && NULL == mgException)
            throw new MgFileIoException(L"MentorDictionary.ReadDictionaryEntries", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

        MG_THROW()

        return allDefs.release();
    }

    //Template function(s) which generates a summary from a dictionary
    //file.  The caller is responsible for deleting the returned map.
    //Function returns NULL if it runs out of memory.  Note that this
    //is a fairly expensive function, timewise, since it involves
    //doing a complete scan of the entire file, which may be over
    //a megabyte in size.  Template parameter T is a Mentor struct
    //(cs_Csdef_ et al).
    //

    template <class T>
    CSystemNameDescriptionMap*
    GenerateSystemNameDescriptionMap(
        vector<T*> const* csMapDefs,
        const char* (*CS_Tkey)(const T&),
        const char * (*description)(const T&))
    {
        try
        {
            //Make a collection to return - we first read all entries from the dictionary file
            //and use those to fill up the dictionary we're returning
            auto_ptr<CSystemNameDescriptionMap> pmapSystemNameDescription(new CSystemNameDescriptionMap);
            for(typename vector<T>::size_type i = 0; i < csMapDefs->size(); ++i)
            {
                T* def = csMapDefs->at(i);

                const char* keyName = CS_Tkey(*def);
                pmapSystemNameDescription->insert(
                    CSystemNameDescriptionPair(
                        CSystemName(keyName),
                        CSystemDescription(description(*def))
                    )
                );
            }

            return pmapSystemNameDescription.release();
        }
        catch(std::bad_alloc&)
        {
        }

        return NULL;
    }

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

        try
        {
            //Make a collection to return - we first read all entries from the dictionary file
            //and use those to fill up the dictionary we're returning
            auto_ptr<CSystemNameDescriptionMap> pmapSystemNameDescription(new CSystemNameDescriptionMap);
            TContainerPtr<T, vector<T*> > allDictEntries(MentorDictionary::ReadDictionaryEntries(pFile, CS_Trd));
            if (NULL == allDictEntries.get())
                return NULL;

            return GenerateSystemNameDescriptionMap(allDictEntries.get(), CS_Tkey, description);
        }
        catch(std::bad_alloc&)
        {
        }

        return NULL;
    }

    template <class T>
    CSystemNameDescriptionMap *
    GenerateSystemNameDescriptionMap(
        const char* (*CS_Tkey)(const T&),
        const char * (*description)(const T&),
        int (*CS_TrdAll)(T **[]))
    {
    typedef vector<T*> CsMapDefPtrVector;

        TContainerPtr<T*, CsMapDefPtrVector > definitions;
        definitions.reset(new vector<T*>);

        T**  pAllDefs = NULL;
        int readStatus = CS_TrdAll(&pAllDefs);
        if (readStatus < 0)
            return NULL;

        for(int i = 0; i < readStatus; ++i)
            definitions->push_back(pAllDefs[i]);

        //free the pointer array only - not the items;
        //these are now owned by [definitions]
        CS_free(pAllDefs);

        return GenerateSystemNameDescriptionMap(definitions.get(), CS_Tkey, description);
    }

    //Reads all Mg definitions from the dictionary [targetDictionary] and returns them
    //in the std::map [definitions], where the code is mapped to the MgDisposable
    template <class T>
    void ReadAllDefinitions(
        /*IN, required*/ MgCoordinateSystemDictionaryBase* targetDictionary,
        /*IN, required*/ STRING (T::*GetCode)(),
        /*IN/OUT, required*/ std::map<STRING, Ptr<MgDisposable> >& definitions)
    {
        CHECKARGUMENTNULL(targetDictionary, L"MentorDictionary.ReadAllDefinitions");
        CHECKARGUMENTNULL(GetCode, L"MentorDictionary.ReadAllDefinitions");
        if (0 != definitions.size())
            throw new MgInvalidArgumentException(L"MentorDictionary.ReadAllDefinitions", __LINE__, __WFILE__, NULL, L"", NULL);

        const UINT32 entryCount = targetDictionary->GetSize();;
        
        Ptr<MgCoordinateSystemEnum> entryEnum = targetDictionary->GetEnum();
        Ptr<MgDisposableCollection> entryList = entryEnum->Next(entryCount);
        
        for(INT32 entryIndex = 0; entryIndex < entryList->GetCount(); entryIndex++)
        {
            Ptr<T> entry = static_cast<T*>(entryList->GetItem(entryIndex));
            STRING entryCode = CALL_MEMBER_FN(entry, GetCode)();
            STRING entryCodeLc = ::ToLower(entryCode);
            definitions.insert(pair<STRING, Ptr<MgDisposable> >(entryCodeLc, entry));
        }
    }

    //takes the [toBeFiltered] MgDisposableCollection which is assumed to contain only items of type
    //[T] and runs the [MgCoordinateSystemFilter]s passed in by [filters] on it (optional)
    //the caller is responsible for deleting the collection being returned
    //if [filters] is NULL or empty, the same(!) collection as passed by [toBeFiltered]
    //is returned - but still the caller has to release it ([toBeFiltered] had been ADDREFed then)
    template <class T>
    MgDisposableCollection* FilterDefinitions(
        /*IN, required*/MgDisposableCollection* toBeFiltered,
        /*IN, optional*/const std::vector<MgCoordinateSystemFilter*>* const filters)
    {
        CHECKARGUMENTNULL(toBeFiltered, L"MentorDictionary.FilterDefinitions");

        if (NULL == filters)
            return SAFE_ADDREF(toBeFiltered); //see coment - the caller does always have to release the collection being returned
        
        const std::vector<MgCoordinateSystemFilter*>::size_type filterCount = filters->size();
        if (0 == filterCount)
            return SAFE_ADDREF(toBeFiltered); //see coment - the caller does always have to release the collection being returned

        Ptr<MgDisposableCollection> filteredCollection = new MgDisposableCollection();
        for(std::vector<MgCoordinateSystemFilter*>::size_type j = 0; j < filterCount; j++)
        {
            Ptr<MgDisposable> disposableItem = toBeFiltered->GetItem(j);
            T* mgCsItem = dynamic_cast<T*>(disposableItem.p);
            if (NULL == mgCsItem)
                throw new MgInvalidArgumentException(L"MentorDictionary.FilterDefinitions", __LINE__, __WFILE__, NULL, L"", NULL);

            //exclude the [mgCsItem], if it's not accepted by the filter we got
            if (filters->at(j)->IsFilteredOut(mgCsItem))
                continue;

            filteredCollection->Add(disposableItem);
        }

        return filteredCollection.Detach();
    }

    //Reads all definitions from the dictionary and returns them as an [MgDisposableCollection] the
    //caller has to release
    template <class T /* The MgCS type [primaryDictionary] contains*/, class U /* The CsMap struct type*/, class V /* The dictionary type that will return items of type T*/>
    MgDisposableCollection* ReadAllDefinitions(
        /*IN, required*/ V* primaryDictionary, //the primary MgCS dictionary to read the items of type T from
        /*IN, required*/ int (*CS_TrdAll)(U **pDefArray[]), //The CsMap method to read all entries
        /*IN, optional*/ void (V::*PostProcess)(U*), //A method that's being called with the CsMap definition struct just read from the file
        /*IN, required*/ T* (V::*GetMgItem)(const U*, const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const), //the method on the [primaryDictionary] to read the actual MgCS item from
        /*IN, optional*/ const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const secondaryDictionaryInfos, //infos that have been read from the dictionaries before; will be passed to [V::*GetMgItem]
        /*IN, optional*/ const std::vector<MgCoordinateSystemFilter*>* const filters) //a list of filters - if passed in, the [MgDisposableCollection] will be filtered before being returned
    {
        //'true' input parameter check
        CHECKARGUMENTNULL(primaryDictionary, L"MentorDictionary.ReadAllDefinitionsCascaded");

        //method pointer checks
        CHECKARGUMENTNULL(GetMgItem, L"MentorDictionary.ReadAllDefinitionsCascaded");
        CHECKARGUMENTNULL(CS_TrdAll, L"MentorDictionary.ReadAllDefinitionsCascaded");

        U** pDefArray = NULL;
        const int readStatus = CS_TrdAll(&pDefArray);
        if (readStatus < 0)
        {
            _ASSERT(NULL == pDefArray);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemDictionary.ReadAllCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const bool doPostProcess = (NULL != PostProcess);
        Ptr<MgDisposableCollection> filteredDefinitions = new MgDisposableCollection();

        MG_TRY()

            //go through all defs as we've read them from CsMap, do some post processing and finally
            //retrieve the MgCs item we'll later return to the caller
            for(int i = 0; i < readStatus; ++i)
            {
                U* csMapDef = pDefArray[i];
                if (doPostProcess)
                    CALL_MEMBER_FN(primaryDictionary, PostProcess)(csMapDef);

                Ptr<T> mgItem = CALL_MEMBER_FN(primaryDictionary, GetMgItem)(csMapDef, secondaryDictionaryInfos); //throws an exception, if the entry hasn't been found
                CS_free(csMapDef);
                pDefArray[i] = NULL;

                filteredDefinitions->Add(mgItem);
            }

        MG_CATCH(L"MentorDictionary.ReadAllDefinitions")

            if (NULL != mgException) //the array's elements have been cleaned up otherwise already
            {
                for(int i = 0; i < readStatus; ++i)
                    CS_free(pDefArray[i]);
            }

            CS_free(pDefArray);

        MG_THROW()

        //we might get the same reference back - what isn't a problem as we're using the Ptr<> here
        filteredDefinitions = MentorDictionary::FilterDefinitions<MgCoordinateSystem>(filteredDefinitions, filters);
        return filteredDefinitions.Detach();
    }

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
        bool bAlreadyExists,
        bool verifyNotProtected)
    {
        assert(NULL != kpDef);
        CHECKARGUMENTNULL(kpDef, L"MentorDictionary.UpdateDef");

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
        UpdateDef<T, Tinterface>(pmapSystemNameDescription, key, description, isValid, CS_Tdef, CS_Tupd, BuildDefFromInterface, kpDef, bAlreadyExists, true);
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
        char *pName = Convert_Wide_To_UTF8(kpName);    //need to delete [] pName;
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
