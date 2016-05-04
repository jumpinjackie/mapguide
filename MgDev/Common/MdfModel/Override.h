//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef OVERRIDE_H_
#define OVERRIDE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "ThemeLabel.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API Override : public MdfRootObject
    {
    public:
        // Construction, initialization
        Override();
        virtual ~Override();

        const MdfString& GetSymbolName() const;
        void SetSymbolName(const MdfString& symbolName);

        const MdfString& GetParameterIdentifier() const;
        void SetParameterIdentifier(const MdfString& parameterIdentifier);

        const MdfString& GetParameterValue() const;
        void SetParameterValue(const MdfString& parameterValue);

        ThemeLabel* GetThemeLabel();
        void AdoptThemeLabel(ThemeLabel* themeLabel);
        ThemeLabel* OrphanThemeLabel();

    private:
        // Hidden copy constructor and assignment operator.
        Override(const Override&);
        Override& operator=(const Override&);

        // Data members
        MdfString m_sSymbolName;
        MdfString m_sParameterIdentifier;
        MdfString m_sParameterValue;
        ThemeLabel* m_pThemeLabel;
    };

    typedef MdfOwnerCollection<Override> OverrideCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<Override>;

END_NAMESPACE_MDFMODEL
#endif // OVERRIDE_H_
