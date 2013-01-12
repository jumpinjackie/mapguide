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

#ifndef _COORDSYSDICTIONARYBASE_H_
#define _COORDSYSDICTIONARYBASE_H_

#define BUILD_DICTIONARY_BASE //this is needed to build the content in CoordSysDictionaryBase.cpp

#define MAP_CS_DEFAULT_FILE_NAME(x) L## x //builds the wchar_t default filename from the #define found in cs_map.h, e.g. from [cs_GP_NAME]
#define DICTIONARY_FILE_NAME(x) DefaultDictName##x //defines the name of the extern variable we need to initialize each template class with
#define DECLARE_DICTIONARY_FILE_NAME(x) extern const wchar_t DefaultDictName##x[] //declares(!) the extern variable we need for the template class
#define DEFINE_DICTIONARY_FILE_NAME(x,y) const wchar_t DefaultDictName##x[] = MAP_CS_DEFAULT_FILE_NAME(y) //defines(!) the extern variable we need for the template class

//typedef of the callback that's being invoked, when a dictionary file is being opened; the invoked method
//has to specify the open mode the target dictionary file has to be opened with; the [long] parameter is the magic header number
typedef CsDictionaryOpenMode (*MagicNumberCallback)(long);

template<class T/* Mg API class */, class U /* CS map struct */>
struct DefinitionAccess //carries all method pointers needed by this dictionary base implementation so it can interact with the Mg API and the CS Map API
{
public:
    DefinitionAccess(
        U* (*ReadDefinition)(const char* definitionKey), //reads a definition from a dictionary file; the caller is responsible for freeing the returned object
        int (*UpdateDefinition)(U* definition, int), //updates the definition in the dictionary file
        int (*DeleteDefinition)(U* definition), //deletes the definition in the dictionary file
        bool (T::*ValidateDefinition)(), //validates a definition; this method is called on the Mg interface
        bool (*SetupCsStructFromMgInstance)(T*, U&), //initializes the CS Map API struct (type U) from the Mg API objects (type T)
        void (*FullInitialize)(T*, U*, MgCoordinateSystemCatalog* catalog), //fully initializes the Mg API objects from the CS Map API struct
        const char* (*ReadDefinitionName)(const U& definition), //reads the definition's name; the caller has to copy the string's content
        const char* (*ReadDefinitionDescription)(const U& definition), //reads the definition's description; the caller must copy the string's content (if not NULL)
        int (*ReadAllDefinitions) (U** pDefinition[]), // reads the entire content of a dictionary file(s);
        void (*CsMapTargetFileName)(const char *newFileName), //set the target dictionary file name in the CS Map library
        CsDictionaryOpenMode (*MagicNumberCallback)(long)) //returns the correct open mode for a dictionary file taking into account the magic number read from the file; passed in as long
        :
            readDefinition(ReadDefinition),
            updateDefinition(UpdateDefinition),
            deleteDefinition(DeleteDefinition),

            validateDefinition(ValidateDefinition),

            setupCsStructFromMgInstance(SetupCsStructFromMgInstance),
            fullInitialize(FullInitialize),

            readDefinitionName(ReadDefinitionName),
            readDefinitionDescription(ReadDefinitionDescription),

            readAllDefinitions(ReadAllDefinitions),

            csMapTargetFileName(CsMapTargetFileName),
            magicNumberCallback(MagicNumberCallback)
    {
    }

    U* (*readDefinition)(const char* definitionKey);
    int (*updateDefinition)(U* definition, int);
    int (*deleteDefinition)(U* definition);
    bool (T::*validateDefinition)();
    bool (*setupCsStructFromMgInstance)(T*, U&);
    void (*fullInitialize)(T*, U*, MgCoordinateSystemCatalog* catalog);
    const char* (*readDefinitionName)(const U& definition);
    const char* (*readDefinitionDescription)(const U& definition);
    int (*readAllDefinitions) (U** pDefinitions[]);
    void (*csMapTargetFileName) (const char *newFileName);
    CsDictionaryOpenMode (*magicNumberCallback)(long);
};

#define DICTIONARY_BASE_TEMPLATE template <class T /* dictionary entries */, class U /* CS_MAP C type*/, \
    DefinitionAccess<T, U>* definitionAccess, \
    bool cacheDictionaryEntries, bool checkProtectedOnUpdate, INT32 defaultMagicNumber, const wchar_t* defaultFileName>

#define DICTIONARY_BASE_TEMPLATE_METHOD CCoordinateSystemDictionaryBase<T, U, definitionAccess, cacheDictionaryEntries, checkProtectedOnUpdate, defaultMagicNumber, defaultFileName>
#define DICTIONARY_BASE_TEMPLATE_OBJECT CCoordinateSystemDictionaryBase<T, U, definitionAccess, cacheDictionaryEntries, checkProtectedOnUpdate, defaultMagicNumber, defaultFileName>
#define DICTIONARY_BASE_ENUM_TEMPLATE_METHOD DictionaryBaseEnum<T, U, definitionAccess, cacheDictionaryEntries, checkProtectedOnUpdate, defaultMagicNumber, defaultFileName>

namespace CSLibrary
{

    DICTIONARY_BASE_TEMPLATE
    class CCoordinateSystemDictionaryBase : public MgCoordinateSystemDictionaryBase
    {
    public:

        /* construction / destruction */
        CCoordinateSystemDictionaryBase(MgCoordinateSystemCatalog *pCatalog);
        virtual ~CCoordinateSystemDictionaryBase();

        /* overrides */
        virtual MgCoordinateSystemCatalog* GetCatalog();
        virtual STRING GetDefaultFileName();
        virtual STRING GetFileName();
        virtual void SetFileName(CREFSTRING sFileName);
        virtual STRING GetPath();
        virtual UINT32 GetSize();
        virtual void Add(MgGuardDisposable *pDefinition);
        virtual void Remove(CREFSTRING sName);
        virtual void Modify(MgGuardDisposable *pDefinition);
        virtual MgGuardDisposable* Get(CREFSTRING sName); // TODO - this needs to be removed!
        virtual bool Has(CREFSTRING sName);
        virtual MgCoordinateSystemEnum* GetEnum();

        virtual void Dispose();

        /* implementation */
        T* NewItem();

    protected:
        virtual CCoordinateSystemEnum* GetEnumImp();

    protected:

        Ptr<MgCoordinateSystemCatalog> catalog;
        STRING fileName;
        CSystemNameDescriptionMap* dictionaryItems;
        INT32 dictMagicNumber;
    };

} // End of namespace

#endif //_COORDSYSDICTIONARYBASE_H_
