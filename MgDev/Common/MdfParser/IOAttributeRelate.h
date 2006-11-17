/*
 * Copyright (C) 2004 Autodesk, Inc. All Rights Reserved.
 *
 * By using this code, you are agreeing to the terms and conditions of
 * the License Agreement included in the documentation for this code.
 *
 * AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
 * DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
 *
 * Use, duplication, or disclosure by the U.S. Government is subject
 * to restrictions set forth in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
 * (Rights in Technical Data and Computer Software), as applicable.
 */

#ifndef _IOATTRIBUTERELATE_H
#define _IOATTRIBUTERELATE_H

#include "../MdfModel/AttributeRelate.h"
#include "../MdfModel/Extension.h"
#include "SAX2ElementHandler.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class MDFPARSER_API IOAttributeRelate : public SAX2ElementHandler
{
    private:
        AttributeRelate *m_pAttributeRelate;
        Extension *m_pExtension;

		void WriteType(MdfStream &fd, AttributeRelate *pAttributeRelate);
		AttributeRelate::RelateType IOAttributeRelate::ReadType(const wchar_t *name);

    public:
        IOAttributeRelate();
        IOAttributeRelate(Extension *pExtension);
        ~IOAttributeRelate();
        void Write(MdfStream &fd, AttributeRelate *pAttributeRelate);

        virtual void StartElement(const wchar_t *name, HandlerStack *handlerStack);
        virtual void ElementChars(const wchar_t *ch);
        virtual void EndElement(const wchar_t *name, HandlerStack *handlerStack);
};
END_NAMESPACE_MDFPARSER
#endif
