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

#ifndef IOVECTOR3D_H_
#define IOVECTOR3D_H_

#include "../SAX2ElementHandler.h"
#include "../MdfModel/Common/Vector3D.h"

BEGIN_NAMESPACE_MDFPARSER

class IOVector3D : public SAX2ElementHandler
{
public:
    IOVector3D(Vector3D* vector, Version& version);
    virtual ~IOVector3D();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);
    virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

    static void Write(MdfStream& fd, Vector3D* vector, Version* version, MgTab& tab);

private:
    // Hidden default/copy constructors and assignment operator.
    IOVector3D();
    IOVector3D(const IOVector3D&);
    IOVector3D& operator=(const IOVector3D&);

    Vector3D* m_vector;
};

END_NAMESPACE_MDFPARSER
#endif // IOVECTOR3D_H_
