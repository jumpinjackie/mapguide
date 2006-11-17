//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef VECTOR_H_
#define VECTOR_H_

BEGIN_NAMESPACE_MDFMODEL

class MDFMODEL_API Vector
{
    public:
        Vector();
        virtual ~Vector();

        double GetX() const;
        void SetX(double x);

        double GetY() const;
        void SetY(double y);

        double GetZ() const;
        void SetZ(double z);

    private:
        // Hidden copy constructor and assignment operator.
        Vector(const Vector&);
        Vector& operator=(const Vector&);

        double m_dX;
        double m_dY;
        double m_dZ;
};

END_NAMESPACE_MDFMODEL
#endif
