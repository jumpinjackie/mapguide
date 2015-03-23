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

#ifndef _CCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_
#define _CCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_


DECLARE_DICTIONARY_FILE_NAME(GeodeticPath);

//typedef cs_GeodeticPath_* (*ReadPathDefinition)(const char* definitionKey, INT32 dictionaryMagicNumber);
//typedef const char* (*ReadPathName)(const cs_GeodeticPath_* definition);
//typedef const char* (*ReadPathDescription)(const cs_GeodeticPath_* definition);

extern DefinitionAccess<CSLibrary::CCoordinateSystemGeodeticPath, cs_GeodeticPath_> definitionAccess;

namespace CSLibrary
{
    class CCoordinateSystemGeodeticPathDictionary : public MgCoordinateSystemGeodeticPathDictionary
    {
    public:
        CCoordinateSystemGeodeticPathDictionary(MgCoordinateSystemCatalog *pCatalog);
        virtual ~CCoordinateSystemGeodeticPathDictionary();

        //create new path
        virtual MgCoordinateSystemGeodeticPath* NewGeodeticPath();
        virtual MgCoordinateSystemGeodeticPath* GetGeodeticPath(CREFSTRING pathName);

        //querying the dictionary
        virtual MgGuardDisposable* Get(CREFSTRING sName);
        virtual MgCoordinateSystemEnum* GetEnum();
        virtual bool Has(CREFSTRING sName);
        virtual STRING GetPath();
        virtual UINT32 GetSize();

        //CS_Map library support stuff
        virtual MgCoordinateSystemCatalog* GetCatalog();
        virtual STRING GetDefaultFileName();
        virtual STRING GetFileName();
        virtual void SetFileName(CREFSTRING sFileName);

        //modify the dictionary
        virtual void Add(MgGuardDisposable *pDefinition);
        virtual void Remove(CREFSTRING sName);
        virtual void Modify(MgGuardDisposable *pDefinition);
        //virtual void Rename(CREFSTRING sOldName, CREFSTRING sNewName);

    protected:
        //MgDisposable
        virtual void Dispose();

    protected:

        CCoordinateSystemDictionaryBase<CCoordinateSystemGeodeticPath, cs_GeodeticPath_, &definitionAccess, true, true, cs_GPDEF_MAGIC, DICTIONARY_FILE_NAME(GeodeticPath)>* m_pDictionary;

    public:

        static const char* ReadName(const cs_GeodeticPath_& definition);
        static const char* ReadDescription(const cs_GeodeticPath_& definition);

        static int UpdateGeodeticPath(cs_GeodeticPath_* csPath, int /*unused*/);

        static bool SetupCsGeodeticPathStruct(CCoordinateSystemGeodeticPath* mgGeodeticPath, cs_GeodeticPath_& csPath);
        static void FullInitialize(CCoordinateSystemGeodeticPath* mgGeodeticPath, cs_GeodeticPath_* csPath, MgCoordinateSystemCatalog* catalog);

        static CsDictionaryOpenMode GetFileOpenMode(long magicNumberFromFile);
    };

} // End of namespace

#endif //_CCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_
