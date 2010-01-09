//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//
#ifndef _DWFEMAPTK_ADDLAYERGROUP_H
#define _DWFEMAPTK_ADDLAYERGROUP_H

#include "../../EMapToolkit.h"
#include "./Command.h"

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFEMapLayerGroup;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapAddLayerGroupCommand... TODO.
    //--------------------------------------------------------------------
    class DWFEMapAddLayerGroupCommand : public DWFEMapCommand
    {
    public:
        // deserialization constructor
        _DWFTK_API DWFEMapAddLayerGroupCommand(DWFPackageReader* pPackageReader) throw();

        // basic constructor
        _DWFTK_API DWFEMapAddLayerGroupCommand(DWFEMapLayerGroup* poLayerGroup,
                                               const DWFString& zInsertAfterObjectId = L"") throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList(const char** ppAttributeList) throw(DWFException);

        // destructor
        _DWFTK_API virtual ~DWFEMapAddLayerGroupCommand() throw();

        // property accessors
        _DWFTK_API const DWFString& insertAfter() const throw();
        _DWFTK_API DWFEMapLayerGroup* layerGroup() throw();

        // XMLCallback support
        _DWFTK_API void notifyStartElement(const char* zName,
                                           const char** ppAttributeList) throw();
        _DWFTK_API void notifyEndElement(const char* zName) throw();
        _DWFTK_API void notifyStartNamespace(const char* zPrefix,
                                             const char* zURI) throw();
        _DWFTK_API void notifyEndNamespace(const char* zPrefix) throw();
        _DWFTK_API void notifyCharacterData(const char* zCData, int nLength) throw();

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML(DWFXMLSerializer& rSerializer,
                                     unsigned int nFlags) throw(DWFException);

    private:
        // hidden copy constructor and assignment operator
        DWFEMapAddLayerGroupCommand(const DWFEMapAddLayerGroupCommand&);
        void operator=(const DWFEMapAddLayerGroupCommand&);

        // data members
        DWFString _zInsertAfter;
        DWFEMapLayerGroup* _poLayerGroup;

        // state variables for XML parsing
        bool _bIsLayerGroup;

        // cached and non-persisted class for constructing the layer group
        DWFPackageReader* _poPackageReader;
    };
}}

#endif // _DWFEMAPTK_ADDLAYERGROUP_H
// End of file.
