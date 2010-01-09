//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MG_WARNING_H_
#define _MG_WARNING_H_

/// \defgroup MgWarnings MgWarnings
/// \ingroup Common_Module
/// \{

// To undefine GetMessage macro defined in windows
#ifdef _WIN32
    #undef GetMessage
#endif

class MgWarnings;
template class MG_PLATFORMBASE_API Ptr<MgWarnings>;

/// \brief
/// Exposes non-critical warning messages from services.
///
/// \remarks
/// Warnings are applicable only to the last operation performed on a service.
/// Warnings are cleared before each new call to a service API.
/// They are provided for information only.
class MG_PLATFORMBASE_API MgWarnings : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgWarnings)

    ///////////////////////////////////////////////////////
    /// Methods
PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieves collection of warning messages
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgStringCollection GetMessages();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgStringCollection GetMessages();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgStringCollection GetMessages();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Collection of messages
    ///
    MgStringCollection* GetMessages();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the count of warning messages
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Number of warning messages
    ///
    INT32 GetCount();   /// __get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor
    MgWarnings();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgWarnings object
    ///
    virtual ~MgWarnings();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Appends collection of messages to existing messages (it copies)
    ///
    /// \param arguments
    /// Collection of messages for warning
    ///
    void AddMessages(MgStringCollection* pArguments);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Appends message to existing messages (it copies)
    ///
    /// \param message
    /// message for warning
    ///
    void AddMessage(CREFSTRING message);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Self Destructor for MgWarnings object
    ///
    virtual void Dispose();

private:
    /// Collection of arguments associated with the exception
    Ptr<MgStringCollection> m_arguments;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Service_Warning;
};
/// \}

#endif // _MG_WARNING_H_
