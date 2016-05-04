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

#ifndef _IONAMESTRINGPAIR_H
#define _IONAMESTRINGPAIR_H

#include "SAX2ElementHandler.h"
#include "NameStringPair.h"
#include "VectorLayerDefinition.h"
#include "FeatureSource.h"
#include "TileStoreParameters.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IONameStringPair : public SAX2ElementHandler
{
    public:
        IONameStringPair(Version& version);
        IONameStringPair(VectorLayerDefinition* layer, Version& version);
        IONameStringPair(FeatureSource* featureSource, Version& version);
        IONameStringPair(TileStoreParameters* params, Version& version);
        virtual ~IONameStringPair();

        virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
        virtual void ElementChars(const wchar_t* ch);
        virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

        static void Write(MdfStream& fd, std::string name, NameStringPair* nameStringPair, Version* version, MgTab& tab);

    private:
        NameStringPair* m_nameStringPair;
        VectorLayerDefinition* m_layer;
        FeatureSource* m_featureSource;
        TileStoreParameters* m_tileStoreParams;
        NameStringPairCollection* m_overrides;
};

END_NAMESPACE_MDFPARSER
#endif // _IONAMESTRINGPAIR_H
