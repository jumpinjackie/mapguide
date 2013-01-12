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

#ifndef _IOMAPLAYERCOMMON_H
#define _IOMAPLAYERCOMMON_H

#include "SAX2ElementHandler.h"
#include "BaseMapLayer.h"
#include "MapDefinition.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IOMapLayerCommon : public SAX2ElementHandler
{
    protected:
        IOMapLayerCommon(Version& version);

    public:
        IOMapLayerCommon(BaseMapLayer* mapLayerCommon, Version& version);
        virtual ~IOMapLayerCommon();

        virtual void ElementChars(const wchar_t* ch);

        static void Write(MdfStream& fd, BaseMapLayer* baseMapLayer, Version* version, MgTab& tab);

    protected:
        BaseMapLayer* m_mapLayerCommon;
};

END_NAMESPACE_MDFPARSER
#endif // _IOMAPLAYERCOMMON_H
