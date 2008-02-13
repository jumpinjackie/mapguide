//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "OgcFramework.h"
#include "HttpRequestParameters.h"

#include "HttpHandlerApiDllExport.h"
#include "System/Ptr.h"
#include "HttpRequestParam.h"

// Constructor adapted for use as a wrapper around that other Request Parameter
// thing (the one outside of the OgcFramework) since that bad boy is case
// sensitive, and we don't want it to be.
// We adapt this from the NameValueCollection class that came from the OgcFramework.
MgHttpRequestParameters::MgHttpRequestParameters(MgHttpRequestParam* pParams)
: m_pParams(pParams)
{
    MgStringPropertyCollection* p = m_pParams->GetParameters();
    SetCount(p->GetCount());

    for(int i=0; i< Count(); i++) {
      STRING sName = p->GetName(i);
      STRING sValue = p->GetValue(i);

      VPSZ pszName = szdup(sName.c_str());
      VPSZ pszValue = szdup(sValue.c_str());
      SetName(i,pszName);
      SetValue(i,pszValue);
    }
}

/////////////////////////////////////////////////////////////////////////
//  Our dtor needs to call Empty explicity, because by the time the
//  base class is called the object isn't a CHttpRequestParameters object
//  anymore, so the virtual table doesn't point to our Empty.
//
MgHttpRequestParameters::~MgHttpRequestParameters()
{
    Empty();
}


/////////////////////////////////////////////////////////////////////////
// If the collection is emptied, we need to free up our working buffer,
// too.
void MgHttpRequestParameters::Empty()
{
    for(int i=0; i<Count(); i++) {
        CPSZ pszName = Name(i);
        if(pszName != NULL) {
            free((void*)pszName);
            SetName(i,NULL);
        }
        void* pvValue = Value(i);
        if(pvValue != NULL) {
            free(pvValue);
            SetValue(i,NULL);
        }
    }
    MgUtilNameStringValueCollection::Empty();
}



