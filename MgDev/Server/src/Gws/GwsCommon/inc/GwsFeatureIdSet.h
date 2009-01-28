//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef GwsFeatureIdSet_h
#define GwsFeatureIdSet_h
#include <set>
#include <vector>

template< typename T , class Interface>
class CGwsFeatureIdSet : public GWSObject<Interface>
{
public:
    typedef std::map<T, FdoInt32> IdToIdxMap;

    CGwsFeatureIdSet()
    {
        Clear();
    }

    virtual ~CGwsFeatureIdSet() throw()
    {
        Clear();
    }

    virtual FdoInt32 Size() const
    {
        return count;
    }

    virtual bool Contains(const T& id) const
    {
        return (mMapIds.find(id) != mMapIds.end());
    }

    virtual bool Add(const T& id)
    {
        typename GWSObject< Interface >::ThisLock lock(this);
        typename std::pair<typename IdToIdxMap::iterator, bool> pair = mMapIds.insert(typename IdToIdxMap::value_type(id, count));
        if(!pair.second)
            return false;
        mVecIds.push_back(id);
        count++;
        assert(count == (FdoInt32)mVecIds.size());
        return true;
    }

    virtual bool Remove(const T& id)
    {
        typename GWSObject< Interface >::ThisLock lock(this);
        typename IdToIdxMap::iterator iter = mMapIds.find (id);
        if(iter == mMapIds.end())
            return false;

        FdoInt32 pos = iter->second;
        mMapIds.erase(iter);

        //mVecIds.erase(remove(mVecIds.begin(), mVecIds.end(), id), mVecIds.end());
        mVecIds.erase(mVecIds.begin() + pos);

        for(unsigned int i=pos ; i<mVecIds.size(); i++)
        {
            const T& key = mVecIds[i];
            typename IdToIdxMap::iterator iter = mMapIds.find (key);
            if(iter == mMapIds.end())
                assert(false);
            iter->second = i;
        }

        count--;
        assert(count == (FdoInt32)mVecIds.size());
        return true;
    }

    virtual const T& Id(FdoInt32 index)
    {
        typename GWSObject< Interface >::ThisLock lock(this);
        if(index < 0 || index > (FdoInt32)mVecIds.size()-1)
            GWS_THROW(eGwsIndexOutOfBounds);
        return mVecIds[index];
    }

    virtual void Clear()
    {
        mMapIds.clear();
        mVecIds.clear();
        count = 0;
    }

    virtual std::vector<T>& AsVector() { return mVecIds; }

protected:
    GWSFeatureId                ConstructId(long id) const
    {
        FdoPtr<FdoInt32Value> pVal  = FdoInt32Value::Create (id);
        FdoPtr<FdoDataValueCollection> keyvals = CGwsDataValueCollection::Create ();
        keyvals->Add (pVal);
        return GWSFeatureId(keyvals);
    }

private:
    IdToIdxMap                  mMapIds;
    std::vector<T>              mVecIds;
    FdoInt32                    count;
};

#endif /* GwsFeatureIdSet_h */
