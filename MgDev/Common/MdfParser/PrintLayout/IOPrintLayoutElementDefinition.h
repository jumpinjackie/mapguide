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

#ifndef IOPRINTLAYOUTELEMENTDEFINITION_H_
#define IOPRINTLAYOUTELEMENTDEFINITION_H_

#include "../SAX2ElementHandler.h"
#include "../MdfModel/PrintLayout/PrintLayoutElementDefinition.h"

BEGIN_NAMESPACE_MDFPARSER

class IOPrintLayoutElementDefinition : public SAX2ElementHandler
{
public:
    IOPrintLayoutElementDefinition(PrintLayoutElementDefinition* layoutElemDef, Version& version);
    virtual ~IOPrintLayoutElementDefinition();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);
    virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

protected:
    static void Write(MdfStream& fd, PrintLayoutElementDefinition* layoutElemDef, Version* version, MgTab& tab);

private:
    // Hidden default/copy constructors and assignment operator.
    IOPrintLayoutElementDefinition();
    IOPrintLayoutElementDefinition(const IOPrintLayoutElementDefinition&);
    IOPrintLayoutElementDefinition& operator=(const IOPrintLayoutElementDefinition&);

protected:
    PrintLayoutElementDefinition* m_layoutElemDef;
};

END_NAMESPACE_MDFPARSER
#endif // IOPRINTLAYOUTELEMENTDEFINITION_H_
