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

#ifndef _CCOORDINATESYSTEMFORMATCONVERTER_H_
#define _CCOORDINATESYSTEMFORMATCONVERTER_H_

namespace CSLibrary
{

class CCoordinateSystemFormatConverter : public MgCoordinateSystemFormatConverter
{
public:
    CCoordinateSystemFormatConverter(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemFormatConverter();

    virtual STRING DefinitionToWkt(MgCoordinateSystem* pSource, INT32 nWktFlavor);
    virtual MgCoordinateSystem* WktToDefinition(INT32 nWktFlavor, CREFSTRING sWkt);
    virtual STRING CodeToWkt(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nWktFlavor);
    virtual STRING WktToCode(INT32 nWktFlavor, CREFSTRING sWkt, INT32 nFormatDestination);

    //code format conversion
    virtual STRING DefinitionToCode(MgCoordinateSystem* pSource, INT32 nFormatDestination);
    virtual MgCoordinateSystem* CodeToDefinition(INT32 nFormatSource, CREFSTRING sCodeSource);
    virtual STRING CodeToCode(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nFormatDestination);
    virtual bool IsCodeInDictionary(INT32 nFormat, CREFSTRING sCode);

protected:
    virtual void Dispose();

protected:
    MgCoordinateSystem* GetCoordinateSystem(CREFSTRING sCsName);
    void ConvertArbitraryToWkt(MgCoordinateSystem* pCsDef, REFSTRING sWkt);
    bool IsCoordinateSystem(char* pszCsName, char** ppCsDescription);
    bool IsCoordinateSystem(CREFSTRING sCsName, char** ppCsDescription);
    enum ErcWktFlavor GetWktFlavor(INT32 nWktFlavor);
    void SetCatalog(MgCoordinateSystemCatalog* pCatalog);
    void ReformatOldArbitraryName(REFSTRING sName);
    void ReformatOldArbitraryName(std::string& sName);

protected:
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

private:
    CCoordinateSystemFormatConverter();
    CCoordinateSystemFormatConverter(const CCoordinateSystemFormatConverter&);
    CCoordinateSystemFormatConverter& operator=(const CCoordinateSystemFormatConverter&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMFORMATCONVERTER_H_
