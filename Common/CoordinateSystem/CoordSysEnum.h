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

#ifndef _CCOORDINATESYSTEMENUM_H_
#define _CCOORDINATESYSTEMENUM_H_

namespace CSLibrary
{
    //defines the callback that's being invoked, when *all* definitions are to be read from the dictionary
    //as passed in by [targetDictionary]
    //
    //param targetDictionary: the dictionary to read all MgCs objects from
    //param filters: a list of filters to determine, whether an MgCs object found should be added to the list being returned; can be null or empty
    //
    //return: MgDisposableCollection that contains the filtered MgCs objects from the dictionary. The caller has to release it
    typedef MgDisposableCollection* (*ReadAllDictionaryDefinitionsCallback)(MgCoordinateSystemDictionaryBase* targetDictionary, const std::vector<MgCoordinateSystemFilter*>* const filters);
    
class CCoordinateSystemEnum : public MgCoordinateSystemEnum
{
public:
    CCoordinateSystemEnum();
    virtual ~CCoordinateSystemEnum();

    void Initialize(MgCoordinateSystemDictionaryBase* pDict, CSystemNameDescriptionMap *pmapSystemNameDescription);
    void Uninitialize();

    virtual MgDisposableCollection* Next(UINT32 ulCount);
    virtual void Skip(UINT32 ulSkipCount);
    virtual void Reset();
    virtual MgCoordinateSystemEnum* CreateClone();
    virtual void AddFilter(MgCoordinateSystemFilter *pFilter);
    virtual MgStringCollection* NextName(UINT32 ulCount);
    virtual MgStringCollection* NextDescription(UINT32 ulCount);

    void SetReadAllDefinitionCallback(ReadAllDictionaryDefinitionsCallback callback);

protected:
    virtual void Dispose();

protected:
    bool IsFilteredOut(MgGuardDisposable *pDef);
    bool IsFilteredOut(const char *kpName);
    void ClearFilter();

protected:
    //Data members
    CSystemNameDescriptionMap *m_pmapSystemNameDescription;
    CSystemNameDescriptionMap::iterator m_iter;
    Ptr<MgCoordinateSystemDictionaryBase> m_pDict;
    std::vector<MgCoordinateSystemFilter*> m_vectFilter;
    ReadAllDictionaryDefinitionsCallback m_readAllDefCallback;

private:
    CCoordinateSystemEnum(const CCoordinateSystemEnum&);
    CCoordinateSystemEnum& operator=(const CCoordinateSystemEnum&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMENUM_H_
