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

#ifndef _MG_SERVER_ENUMERATE_DATASTORES_H_
#define _MG_SERVER_ENUMERATE_DATASTORES_H_

#include "MgDesktop.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"

class MgdEnumerateDataStores
{
public:
    MgdEnumerateDataStores();
    ~MgdEnumerateDataStores();


    MgByteReader* EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString);

private:
    MgXmlUtil* m_xmlUtil;
};

#endif
