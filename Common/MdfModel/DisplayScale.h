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

#ifndef DISPLAYSCALE_H_
#define DISPLAYSCALE_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Holds a double.  We make the double into a class so that the
    // FiniteDisplayScaleCollection is consistent with other collections.
    //-------------------------------------------------------------------------
    class MDFMODEL_API DisplayScale
    {
    public:
        // Construction, initialization
        // Uses the default destructor as it passed around by value
        // and thus should not be subclassed.
        DisplayScale(double val);

        double GetValue() const;
        void SetValue(double val);

    private:
        double m_value;

    };

    typedef MdfOwnerCollection<DisplayScale> DisplayScaleCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<DisplayScale>;

END_NAMESPACE_MDFMODEL
#endif // DISPLAYSCALE_H_
