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

#ifndef MG_MATRIX_H
#define MG_MATRIX_H

#include <assert.h>
#include <iostream>

//=============================================================================
//  This matrix template is designed to be used with built-in arithmetic types.
//  It is of fixed size; it is constructed with one memory allocation.
template <typename T>
class MgdMatrix
{
    public:
        MgdMatrix(int dim1, int dim2)
        {
            assert(dim1 > 0);
            assert(dim2 > 0);
            m_dim1 = dim1;
            m_dim2 = dim2;
            m_pData = new T [ dim1 * dim2 ];
            assert(m_pData);
            Clear();
        }

        ~MgdMatrix()
        {
            delete [] m_pData;
        }

        void Set(int i, int j, T val)
        {
            *Ptr(i, j) = val;
        }

        T Get(int i, int j)
        {
            return *Ptr(i, j);
        }

        void Clear()
        {
            int bytes = sizeof(T) * m_dim1 * m_dim2;
            memset(m_pData, 0, bytes);
        }

        void Fill(T val)
        {
            for (T* p = &m_pData[0]; p < (m_pData + (m_dim1 * m_dim2)); ++p)
            {
                *p = val;
            }
        }

        void Dump(std::ostream& o)
        {
            for (int i = 0; i < m_dim1; ++i)
            {
                for (int j = 0; j < m_dim2; ++j)
                {
                    T d = Get(i, j);
                    o << /*NOXLATE*/"matrix(" << i <<
                      /*NOXLATE*/", " << j <<
                      /*NOXLATE*/") = " << d <<
                      /*NOXLATE*/"\n";
                }
            }
        }

    private:
        T* Ptr(int i , int j)
        {
            // I've had some strange experiences with complex asserts inside
            // template classes, so I'm doing each test individually.
            assert(i >= 0);
            assert(i < m_dim1);
            assert(j >= 0);
            assert(j < m_dim2);
            return &m_pData[(i * m_dim2) + j];
        }

    private:
        T*  m_pData;
        int m_dim1;
        int m_dim2;
};

#endif  // DM_MATRIX_H
