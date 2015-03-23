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

#ifdef BUILD_DICTIONARY_BASE //set this define before including CoordSysDictionaryBase.cpp; otherwise nothing will be built

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
DICTIONARY_BASE_TEMPLATE_METHOD::CCoordinateSystemDictionaryBase(MgCoordinateSystemCatalog *pCatalog)
    : dictMagicNumber(0) /* no yet initialized - will be read in [SetFileName] */, dictionaryItems(NULL)
{
    CHECKARGUMENTNULL(pCatalog, L"CCoordinateSystemDictionaryBase.ctor()");

    this->catalog = SAFE_ADDREF(pCatalog); // 'Ptr = ' does not increment the ref counter
    this->fileName = defaultFileName; //template default parameter
}

DICTIONARY_BASE_TEMPLATE
DICTIONARY_BASE_TEMPLATE_METHOD::~CCoordinateSystemDictionaryBase()
{
    this->catalog = NULL;

    delete this->dictionaryItems;
    this->dictionaryItems = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
void DICTIONARY_BASE_TEMPLATE_METHOD::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
MgCoordinateSystemCatalog* DICTIONARY_BASE_TEMPLATE_METHOD::GetCatalog()
{
    return SAFE_ADDREF(this->catalog.p);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
STRING DICTIONARY_BASE_TEMPLATE_METHOD::GetDefaultFileName()
{
    return defaultFileName; /* from template definition; *NOT* this->fileName ! */
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
STRING DICTIONARY_BASE_TEMPLATE_METHOD::GetFileName()
{
    return this->fileName; /* changeable via SetFileName*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
void DICTIONARY_BASE_TEMPLATE_METHOD::SetFileName(CREFSTRING sFileName)
{
    ::SetDictionaryFileName(
        sFileName,
        this->catalog->GetDictionaryDir(),
        this->dictMagicNumber, //will be set
        definitionAccess->magicNumberCallback, //open mode
        definitionAccess->csMapTargetFileName, //CS Map file target
        L"CoordSysDictionaryBase.SetFileName"); //context

    //the filename has been set for CS library; now set it for this object...
    this->fileName = sFileName;

    //...and clean up the list of entries we might have already read from the old dictionary file
    delete this->dictionaryItems;
    this->dictionaryItems = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
STRING DICTIONARY_BASE_TEMPLATE_METHOD::GetPath() /* the full path to the file we're currently using */
{
    MG_TRY()

    return (this->catalog->GetDictionaryDir() + this->fileName);

    MG_CATCH_AND_THROW(L"CCoordinateSystemDictionaryBase.GetPath")

    _ASSERT(false); //OOM - unable to create the exception object?
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
UINT32 DICTIONARY_BASE_TEMPLATE_METHOD::GetSize()
{
    //If we have a summary available, we can just return its size.
    if (NULL != this->dictionaryItems)
    {
        return static_cast<UINT32>(this->dictionaryItems->size());
    }

    SmartCriticalClass critical(true);

    STRING strPath = this->GetPath();
    csFILE *pFile = MentorDictionary::Open(this->dictMagicNumber, definitionAccess->magicNumberCallback, strPath.c_str(), Read);

    _ASSERT(NULL != pFile);
    UINT32 nSize;

    MG_TRY()

    //We're open, but no summary is available.  We need to examine
    //the file itself to find out how big it is.
    INT32 pos = CS_ftell(pFile);
    CS_fseek(pFile, 0, SEEK_END);
    UINT32 ulFileBytes = 1 + CS_ftell(pFile);
    CS_fseek(pFile, pos, SEEK_SET);
    UINT32 nRecSize = sizeof(U);

    //take the complete file size; subtract the "header" and divide by the size of the structs contained
    nSize = (ulFileBytes - sizeof(this->dictMagicNumber)) / nRecSize;

    MG_CATCH(L"CCoordinateSystemDictionaryBase.GetSize")

    if(NULL != pFile && (0 != CS_fclose(pFile)))
    {
        throw new MgFileIoException(L"CCoordinateSystemDictionaryBase.GetSize", __LINE__, __WFILE__, NULL, NULL, NULL);
    }

    MG_THROW()

    return nSize;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
void DICTIONARY_BASE_TEMPLATE_METHOD::Add(MgGuardDisposable* pDefinition)
{
    //we won't take ownership of [pDefinition]; i.e. it can be deleted by the caller after we're done here
    CHECKARGUMENTNULL(pDefinition, L"CCoordinateSystemDictionaryBase.Add");

    T* pDictionaryDef = dynamic_cast<T*>(pDefinition);
    if (!pDictionaryDef)
        throw new MgInvalidArgumentException(L"CCoordinateSystemDictionaryBase.Add", __LINE__, __WFILE__, NULL, L"CCoordinateSystemDictionaryBase", NULL);

    MG_TRY()

    //::UpdateDef will lock the (file) resource via [SmartCriticalClass critical(true);]
    MentorDictionary::UpdateDef<U, T>(
       this->dictionaryItems, /* can be null in case no enumerator has been created yet or this dictionary isn't set up for caching */
        definitionAccess->readDefinitionName,
        definitionAccess->readDefinitionDescription,
        definitionAccess->validateDefinition,
        definitionAccess->readDefinition,
        definitionAccess->updateDefinition,
        definitionAccess->setupCsStructFromMgInstance,
        pDictionaryDef,
        false); /* bAlreadyExists - we want to create a new one, i.e. the definition must not exist yet */

    MG_CATCH_AND_THROW(L"CCoordinateSystemDictionaryBase.Add")
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
void DICTIONARY_BASE_TEMPLATE_METHOD::Remove(CREFSTRING sName)
{
    MG_TRY()

    MentorDictionary::RemoveDef<U>(
        this->dictionaryItems, /* can be null in case no enumerator has been created yet or this dictionary isn't set up for caching */
        definitionAccess->readDefinition,
        definitionAccess->deleteDefinition,
        sName.c_str());

    MG_CATCH_AND_THROW(L"CCoordinateSystemDictionaryBase.Remove")
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
void DICTIONARY_BASE_TEMPLATE_METHOD::Modify(MgGuardDisposable *pDefinition)
{
    CHECKARGUMENTNULL(pDefinition, L"CCoordinateSystemDictionaryBase.Modify");

    T* pDictionaryDef = dynamic_cast<T*>(pDefinition);
    if (!pDictionaryDef)
        throw new MgInvalidArgumentException(L"CCoordinateSystemDictionaryBase.Modify", __LINE__, __WFILE__, NULL, L"CCoordinateSystemDictionaryBase", NULL);

    MG_TRY()

    //::UpdateDef will lock the (file) resource via [SmartCriticalClass critical(true);]

    //we don't pass our [this->items] here, as the dictionary is accessed anyway;
    //if the caller passes us an unknown definition, we'll fail anyway
    //with an exception what is per se slower than having success here
    MentorDictionary::UpdateDef<U, T>(
        this->dictionaryItems, /* can be null in case no enumerator has been created yet or this dictionary isn't set up for caching */
        definitionAccess->readDefinitionName,
        definitionAccess->readDefinitionDescription,
        definitionAccess->validateDefinition,
        definitionAccess->readDefinition,
        definitionAccess->updateDefinition,
        definitionAccess->setupCsStructFromMgInstance,
        pDictionaryDef,
        true, //the definition has to exist
        checkProtectedOnUpdate); 

    MG_CATCH_AND_THROW(L"CCoordinateSystemDictionaryBase.Modify")
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
MgGuardDisposable* DICTIONARY_BASE_TEMPLATE_METHOD::Get(CREFSTRING sName)
{
    U *pDef = NULL;
    char *pName = NULL;

    //Get the name to search for
    pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName
    Ptr<T> pDefinition;

    MG_TRY()

    //make sure, we've exclusive access here
    SmartCriticalClass critical(true);

    //Look in the dictionary
    pDef = definitionAccess->readDefinition(pName);
    if (NULL == pDef)
    {
        MgStringCollection whatArguments;
        whatArguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"CCoordinateSystemDictionaryBase.Get", __LINE__, __WFILE__, &whatArguments, L"", NULL);
    }

    pDefinition = this->NewItem(); //no AddRef; release if needed
    if (NULL == pDefinition.p) //ABA ???
        throw new MgOutOfMemoryException(L"CCoordinateSystemDictionaryBase.Get", __LINE__, __WFILE__, NULL, L"", NULL);

    //we've created the Mg instance, we got the CS_Map definition; now let the Mg instance initialize itself from the
    //struct;
    //NOTE: [fullInitialize] doesn't take ownership of [pDef] - we've to release it when done
    definitionAccess->fullInitialize(pDefinition, pDef, this->catalog);

    MG_CATCH(L"CCoordinateSystemDictionaryBase.Get")

    /* cleanup - MG_CATCH() catches every exception; i.e. we'll go through here regardless of whether an exception occurred or not */

    CS_free(pDef);
    delete [] pName;

    MG_THROW()

    return pDefinition.Detach(); //done - detach the pointer so the caller can take ownership
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
bool DICTIONARY_BASE_TEMPLATE_METHOD::Has(CREFSTRING sName)
{
    MG_TRY()

    try
    {
        Ptr<MgGuardDisposable> definition = this->Get(sName);
        _ASSERT(NULL != definition);

        return NULL != definition;
    }
    catch(MgCoordinateSystemLoadFailedException* loadFailedException)
    {
        loadFailedException->Release();
        return false;
    }

    MG_CATCH_AND_THROW(L"CCoordinateSystemDictionaryBase.Has")

    _ASSERT(false);
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
MgCoordinateSystemEnum* DICTIONARY_BASE_TEMPLATE_METHOD::GetEnum()
{
    //we don't take ownership; the caller has to free it
    return this->GetEnumImp();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
CCoordinateSystemEnum* DICTIONARY_BASE_TEMPLATE_METHOD::GetEnumImp()
{
    Ptr<CCoordinateSystemEnum> pNew;

    csFILE *dictionaryFile = NULL;
    SmartCriticalClass critical(true);

    CSystemNameDescriptionMap* allDictionaryEntries = NULL;

    MG_TRY()

    //do we have to cache the entries read from the dictionary file and have we already read them before?
    if (!cacheDictionaryEntries || NULL == this->dictionaryItems)
    {
        //Not read yet - try to build the map from the entries found in the dictionary file
        //
        STRING strPath = this->GetPath();

        //[Open] throws an exception, if the dictionary file couldn't be opened
        dictionaryFile = MentorDictionary::Open(this->dictMagicNumber, definitionAccess->magicNumberCallback, strPath.c_str(), Read);

        allDictionaryEntries = MentorDictionary::GenerateSystemNameDescriptionMap<U>(
            definitionAccess->readDefinitionName,
            definitionAccess->readDefinitionDescription,
            definitionAccess->readAllDefinitions);

        if (NULL == allDictionaryEntries) //whatever happend here - the dictionay file seems to be invalid
            throw new MgCoordinateSystemLoadFailedException(L"CCoordinateSystemDictionaryBase.GetEnumImp", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        allDictionaryEntries = this->dictionaryItems;
    }

    //create our enumerator we're returning to the caller
    pNew = new CCoordinateSystemEnum;
    pNew->Initialize(this, allDictionaryEntries /* might point to [this->dictionaryItems] */);

    MG_CATCH(L"CCoordinateSystemDictionaryBase.GetEnumImp")

    //in either case, try to close the dictionary file
    if (NULL != dictionaryFile && 0 != CS_fclose(dictionaryFile) //closing went through?
        && NULL == mgException.p)
        //if there hasn't yet an exception occurred we now have one
        //if there was one already we (unfortunately) do now swallow the MgFileIoException
        mgException = new MgFileIoException(L"CCoordinateSystemDictionaryBase.GetEnumImp", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

    MG_THROW()

    //cache the map of entries if we did succeed
    if (cacheDictionaryEntries)
    {
        //this will be false, if this dictionary object is shared between multiple threads; however, the dictionary must not be shared between threads
        //without proper precautions being taken
        _ASSERT(NULL != allDictionaryEntries);
        _ASSERT(NULL == this->dictionaryItems || (void*)this->dictionaryItems == (void*)allDictionaryEntries);

        this->dictionaryItems = allDictionaryEntries; //cache the map now
    }

    return pNew.Detach();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
DICTIONARY_BASE_TEMPLATE
T* DICTIONARY_BASE_TEMPLATE_METHOD::NewItem()
{
    //new() throws an exception in case allocation fails; we don't take ownership
    return new T(this->catalog); //the callee has to incr. the ref counter, if it needs the catalog object
}

#endif //BUILD_DICTIONARY_BASE
