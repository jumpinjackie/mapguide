//
//  Copyright (c) 1996-2005 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED,
//  AS TO THE CORRECTNESS OF THIS CODE OR ANY DERIVATIVE
//  WORKS WHICH INCORPORATE IT.
//
//  AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS
//  AND EXPLICITLY DISCLAIMS ANY LIABILITY, INCLUDING
//  CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//

#ifndef _DWFW3DTK_OPCODE_HANDLER_H
#define _DWFW3DTK_OPCODE_HANDLER_H

///
///\file        dwf/w3dtk/W3DOpcodeHandler.h
///\brief       This file contains the BaseOpcodeHandlerObserver and W3DOpcodeHandler class declarations.
///

#include "dwfcore/Exception.h"
using namespace DWFCore;


#include "dwf/Toolkit.h"


//
// fwd decl
//
class BBaseOpcodeHandler;

///
///\interface   BaseOpcodeHandlerObserver   dwf/w3dtk/W3DOpcodeHandler.h     "dwf/w3dtk/W3DOpcodeHandler.h"
///\brief       Classes implementing this interface can be notified when an op-code handler
///             has been requested to serialize itself.
///\since       1.0.1000
///
class BBINFILETK_API BaseOpcodeHandlerObserver
{

public:

    ///
    ///         Destructor
    ///
    virtual ~BaseOpcodeHandlerObserver()
        throw()
    {;}

    ///
    ///         Notification event
    ///
    ///\param   pHandler    The handler that is being serialized.
    ///\param   pTag        An optional identifier for this handler.
    ///
    ///\throw   DWFException
    ///
    virtual void notify( BBaseOpcodeHandler* pHandler,
                         const void*         pTag = NULL )
        throw( DWFException ) = 0;

protected:

    ///
    ///         Constructor
    ///
    BaseOpcodeHandlerObserver()
        throw()
    {;}
};


///
///\interface   W3DOpcodeHandler   dwf/w3dtk/W3DOpcodeHandler.h     "dwf/w3dtk/W3DOpcodeHandler.h"
///\brief       Base class shim for BOpcodeHandlers.
///\since       1.0.1000
///
class BBINFILETK_API W3DOpcodeHandler
{

public:

    ///
    ///         Destructor
    ///
    virtual ~W3DOpcodeHandler()
    {;}

    ///
    ///         This object can be notified when this handler is serialized.
    ///
    ///\param   pObserver   The object to notify.
    ///
    void setObserver( BaseOpcodeHandlerObserver* pObserver )
        throw()
    {
        _pObserver = pObserver;
    }

    ///
    ///         This method will write the handler's opcode and data to the W3D stream.
    ///
    ///\param   pTag    An optional tag to associate the data.
    ///\throw   DWFException
    ///
    virtual void serialize( const void* pTag = NULL )
        throw( DWFException ) = 0;

protected:

    ///
    ///         Constructor
    ///
    ///\param   pObserver   The object to notify when this handler is serialized.
    ///
    W3DOpcodeHandler( BaseOpcodeHandlerObserver* pObserver = NULL )
        throw()
        : _pObserver( pObserver )
    {;}

protected:

    BaseOpcodeHandlerObserver*  _pObserver;

private:

    //
    // Not Implemented
    //

    W3DOpcodeHandler( const W3DOpcodeHandler& rCamera );
    W3DOpcodeHandler& operator=( const W3DOpcodeHandler& rCamera );
};



#endif

