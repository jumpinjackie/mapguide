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

#ifndef _CCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_
#define _CCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_


DECLARE_DICTIONARY_FILE_NAME(GeodeticTransformation);

extern DefinitionAccess<CSLibrary::CCoordinateSystemGeodeticTransformDef, cs_GeodeticTransform_> transformDefinitionAccess;

namespace CSLibrary
{
    class CCoordinateSystemGeodeticTransformDefDictionary : public MgCoordinateSystemGeodeticTransformDefDictionary
    {
    public:
        CCoordinateSystemGeodeticTransformDefDictionary(MgCoordinateSystemCatalog *pCatalog);
        virtual ~CCoordinateSystemGeodeticTransformDefDictionary();

        virtual MgCoordinateSystemGeodeticTransformDef* NewGeodeticTransformationDef(INT32 transformationDefType);
        virtual MgCoordinateSystemGeodeticTransformDef* GetGeodeticTransformationDef(CREFSTRING pathName);


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

    protected:
        //MgDisposable
        virtual void Dispose();

    protected:

        CCoordinateSystemDictionaryBase<CCoordinateSystemGeodeticTransformDef, cs_GeodeticTransform_, &transformDefinitionAccess, true, false, cs_GXDEF_MAGIC, DICTIONARY_FILE_NAME(GeodeticTransformation)>* m_pDictionary;

    public:

        static int UpdateGeodeticTransformDef(cs_GeodeticTransform_* csTransformDef, int /*unused*/);
        static bool SetupCsStructFromMgTransformDef(CCoordinateSystemGeodeticTransformDef* mgGeodeticTransformDef, cs_GeodeticTransform_& csTransformDef);

        static void FullInitialize(CCoordinateSystemGeodeticTransformDef* mgGeodeticTransformDef, cs_GeodeticTransform_* csTransformDef, MgCoordinateSystemCatalog* catalog);

        static const char* ReadName(const cs_GeodeticTransform_& definition);
        static const char* ReadDescription(const cs_GeodeticTransform_& definition);

        static CsDictionaryOpenMode GetFileOpenMode(long magicNumberFromFile);
    };

} // namespace CSLibrary

#endif //_CCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_
