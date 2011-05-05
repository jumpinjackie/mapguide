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

#ifndef _MDFOWNERCOLLECTION_H_
#define _MDFOWNERCOLLECTION_H_

// ignore warning about needing to have dll-interface to be used by clients export
#pragma warning(disable: 4251)

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION: MdfOwnerCollection is an indexed collection template
    //              that follows the Adopt and Orphan terminology to take ownership
    //              of the objects that are added to it. The interface requires that
    //              objects adopted are created on the heap and those orphaned are
    //              deleted by the caller. The collection destructor
    //              deletes all the adopted objects in the collection in
    //              addition to the array.
    //---------------------------------------------------------------------
    template <class OBJ> class MdfOwnerCollection
    {

    public:
        //---------------------------------------------------------------------
        // PURPOSE: Constructor
        //---------------------------------------------------------------------
        MdfOwnerCollection()
        {
            m_nCapacity = INIT_CAPACITY;
            m_nSize = 0;
            m_objArray = new OBJ*[m_nCapacity];
        }

        //---------------------------------------------------------------------
        // PURPOSE: Destructor. Deletes all the objects held in the internal
        //          array as well as the array itself.
        //---------------------------------------------------------------------
        ~MdfOwnerCollection()
        {
            // delete all members
            for(int i = 0; i < m_nSize; i++)
                delete m_objArray[i];

            // delete allocated memory
            delete[] m_objArray;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Returns the number of objects in the collection
        //---------------------------------------------------------------------
        int GetCount() const
        {
            return m_nSize;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Accessor for the object at the index location.
        // PARAMETERS:
        //      Input:
        //            nIndex - index of the object to be accessed.
        // RETURNS: The object at the index. If the index is invalid, NULL is
        //          returned.
        //---------------------------------------------------------------------
        OBJ* GetAt(int nIndex) const
        {
            OBJ* pRetValue = NULL;
            if (nIndex < m_nSize &&  nIndex >= 0)
                pRetValue = m_objArray[nIndex];

            return pRetValue;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Method takes ownership of the object and adds it to
        //          the collection. The collection is now responsible for its
        //          deletion.
        // PARAMETERS:
        //      Input:
        //          pAdoptedObject - the pointer to the object created
        //                           on the heap.
        // RETURNS: The index at which the object was added to the collection.
        //---------------------------------------------------------------------
        int Adopt(OBJ* pAdoptedObject)
        {
            if (m_nSize == m_nCapacity)
            {
                this->Resize();
            }
            m_objArray[m_nSize] = pAdoptedObject;

            return m_nSize++;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Method takes ownership of the object and inserts it at the
        //          given Index. If the index is invalid, the object is not
        //          adopted.
        // PARAMETERS:
        //      nIndex - the index at which the object is to be inserted
        //      pAdoptedObject - the pointer to the object created on the heap.
        //RETURNS: true if the index is valid and the object is adopted.
        //         Otherwise, it returns false.
        //---------------------------------------------------------------------
        bool AdoptAt(int nIndex, OBJ* pAdoptedObject)
        {
            bool bRet = false;
            if (m_nSize == m_nCapacity)
            {
                this->Resize();
            }

            if (nIndex <= m_nSize && nIndex >= 0)
            {
                for (int i = m_nSize; i > nIndex; i--)
                {
                    m_objArray[i] = m_objArray[i-1];
                }
                m_objArray[nIndex] = pAdoptedObject;
                bRet = true;
                m_nSize++;
            }
            return bRet;
        }

        //---------------------------------------------------------------------
        // PURPOSE: method destroys all the objects being held by the
        //          collection. It does not however de-allocate the memory
        //          held by its internal array.
        //---------------------------------------------------------------------
        void DestroyAll()
        {
            int    i;
            for (i = 0; i < m_nSize; i++)
            {
                delete m_objArray[i];
                m_objArray[i] = NULL;
            }

            m_nSize = 0;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Removes the object if it exists in the collection. The
        //          caller is now responsible for the objects deletion.
        // PARAMETERS:
        //      Input:
        //          pValue - the pointer to the object that is to be removed.
        // RETURNS: A pointer to the object if it is found in the collection.
        //          If the object is not found then NULL is returned.
        //---------------------------------------------------------------------
        OBJ* Orphan(const OBJ* pValue)
        {
            OBJ* retValue = NULL;
            int nPosition = -1;
            //check if the object exist in the collection.
            for (int ctr = 0; ctr < m_nSize; ctr++)
            {
                if (m_objArray[ctr] == pValue)
                {
                    retValue = m_objArray[ctr];
                    nPosition = ctr;
                    break;
                }
            }

            // if the Object does exist in the Collection, move all the
            // subsequent elements in the array back one element.
            if (retValue != NULL)
            {
                while (nPosition < (m_nSize - 1))
                {
                    m_objArray[nPosition] = m_objArray[nPosition + 1];
                    nPosition++;
                }

                m_objArray[--m_nSize] = NULL;
            }

            return retValue;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Removes the object from the collection at the given index.
        //          The caller is now responsible for the object's deletion.
        // PARAMETERS:
        //      Input:
        //          nIndex - the index at which the object is to be removed.
        // RETURNS: A pointer to the object removed at the specified index.
        //          NULL is returned if the index is invalid.
        //---------------------------------------------------------------------
        OBJ* OrphanAt(int nIndex)
        {
            OBJ* retValue = NULL;
            // if index is valid.
            if (nIndex < m_nSize && nIndex >= 0)
            {
                retValue =  m_objArray[nIndex];
                m_objArray[nIndex] = NULL;
                for (int nCtr = nIndex; nCtr < m_nSize-1; nCtr++)
                {
                    m_objArray[nCtr] = m_objArray[nCtr + 1];
                }
                m_objArray[--m_nSize] = NULL;
            }
            return  retValue;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Method checks to see if a pointer to an object is  a part of
        //          this collection.
        // RETURNS: True if it exist, false otherwise.
        //---------------------------------------------------------------------
        bool Contains(const OBJ* pValue) const
        {
            bool bRet = false;
            for (int nCtr = 0; nCtr < m_nSize; nCtr++)
            {
                if (m_objArray[nCtr] == pValue)
                {
                    bRet = true;
                    break;
                }
            }
            return bRet;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Method for requesting the integer Index for a OBJ* in the
        //          internal array storage.
        // RETURNS: The index value of the object if it exists in the internal
        //          array. Otherwise, it returns -1.
        //---------------------------------------------------------------------
        int IndexOf(const OBJ* value) const
        {
            int nIdx = -1; //default if value is not found.
            for (int nCtr = 0; nCtr < m_nSize; nCtr++)
            {
                if (m_objArray[nCtr] == value)
                {
                    nIdx = nCtr;
                    break;
                }
            }
            return nIdx;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Returns any unknown XML stored with this object.
        // RETURNS: An MdfString containing the unknown XML.
        //---------------------------------------------------------------------
        const MdfString& GetUnknownXml() const
        {
            return this->m_strUnknownXml;
        }

        //---------------------------------------------------------------------
        // PURPOSE: Sets the unknown XML stored with this object.
        // PARAMETERS:
        //      Input:
        //          strUnknownXml - an MdfString containingg the unknown XML
        //---------------------------------------------------------------------
        void SetUnknownXml(const MdfString& strUnknownXml)
        {
            this->m_strUnknownXml = strUnknownXml;
        }

    protected:
        // Default initial capacity of the Collection.
        static const int INIT_CAPACITY = 10;

    private:
        //---------------------------------------------------------------------
        // PURPOSE: Resize the storage capacity of this collection class.
        //---------------------------------------------------------------------
        void Resize()
        {
            int nOldCapacity = this->m_nCapacity;
            this->m_nCapacity = (int)(this->m_nCapacity*(1.5));
            OBJ** newArray = new OBJ*[m_nCapacity];

            for (int nCtr = 0; nCtr < nOldCapacity; nCtr ++)
            {
                newArray[nCtr] = m_objArray[nCtr];
            }

            delete[] m_objArray;
            m_objArray = newArray;
        }

    private:
        // Data members
        // Array of object pointers
        OBJ**  m_objArray;

        // Current capacity of the pointer Array
        int    m_nCapacity;

        // The current number of objects added to the array.
        int    m_nSize;

        // Cache for parser, keeping unknown XML for round-tripping.
        MdfString m_strUnknownXml;
    };

END_NAMESPACE_MDFMODEL
#endif // _MDFOWNERCOLLECTION_H_
