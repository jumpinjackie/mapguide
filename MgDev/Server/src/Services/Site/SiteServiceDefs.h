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

#ifndef MG_SITE_SERVICE_DEFS_H
#define MG_SITE_SERVICE_DEFS_H

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"

#include <assert.h>

// TODO These should be static const strings:
// e.g.
//      MgXmlElementName::Name          = "Name";
//      MgXmlElementName::Description   = "Description";
//      MgXmlAttributeName::Version     = "version";
//

#define ELEMENT_USER                        "User"          //  NOXLATE
#define ELEMENT_USER_NAME                   "Name"          //  NOXLATE
#define ELEMENT_USER_FULLNAME               "FullName"      //  NOXLATE
#define ELEMENT_USER_PASSWORD               "Password"      //  NOXLATE
#define ELEMENT_USER_DESCRIPTION            "Description"   //  NOXLATE
#define ELEMENT_USERLIST                    "UserList"      //  NOXLATE

#define ELEMENT_GROUP                       "Group"         //  NOXLATE
#define ELEMENT_GROUP_NAME                  "Name"          //  NOXLATE
#define ELEMENT_GROUP_DESCRIPTION           "Description"   //  NOXLATE
#define ELEMENT_GROUPLIST                   "GroupList"     //  NOXLATE

#endif  //  MG_SITE_SERVICE_DEFS_H
