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

#ifndef IOSTYLIZATIONCONFIGURATION_H_
#define IOSTYLIZATIONCONFIGURATION_H_

#include "../SAX2ElementHandler.h"
#include "../MdfModel/PrintLayout/StylizationConfiguration.h"

BEGIN_NAMESPACE_MDFPARSER

class IOStylizationConfiguration : public SAX2ElementHandler
{
public:
    IOStylizationConfiguration(StylizationConfiguration* stylizationConf, Version& version);
    virtual ~IOStylizationConfiguration();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);
    virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

    static void Write(MdfStream& fd, StylizationConfiguration* stylizationConf, Version* version, MgTab& tab);

private:
    // Hidden default/copy constructors and assignment operator.
    IOStylizationConfiguration();
    IOStylizationConfiguration(const IOStylizationConfiguration&);
    IOStylizationConfiguration& operator=(const IOStylizationConfiguration&);

    StylizationConfiguration* m_stylizationConf;
};

END_NAMESPACE_MDFPARSER
#endif // IOSTYLIZATIONCONFIGURATION_H_
