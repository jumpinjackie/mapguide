//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MG_LONG_TRANSACTION_READER_H
#define _MG_LONG_TRANSACTION_READER_H

class MgLongTransactionData;

/// \defgroup MgLongTransactionReader MgLongTransactionReader
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Provides forward-only, read-only functionality for describing
/// a set of long transactions that exist in the feature source.
/// \remarks
/// You must call ReadNext before you can access any data. Calling
/// ReadNext() gives you access to the information about one long
/// transaction. See the example code.
/// MgFeatureService::GetLongTransactions returns an
/// MgLongTransactionReader object.
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The code in this example has not been tested. It's a start.
/// \code
/// <?php
/// function printLongTransactionReader($longTransactionReader)
/// {
///    global $logFileHandle;
///    $i = 0;
///    fwrite($logFileHandle, "Printing long transaction readern");
///    while ($longTransactionReader->ReadNext())
///    {
///       $i++;
///       $name = $longTransactionReader->GetName();
///       fwrite($logFileHandle, "Name: $namen");
///       $desc = $longTransactionReader->GetDescription();
///       fwrite($logFileHandle, "Description: $descn");
///       $owner = $longTransactionReader->GetOwner();
///       fwrite($logFileHandle, "Owner: $ownern");
///       $dateTime = $longTransactionReader->GetCreationDate();
///       $printableDateTime = printDateTime($dateTime);
///       fwrite($logFileHandle, "Creation Date: $printableDateTimen");
///       $isActive = $longTransactionReader->IsActive();
///       $printableIsActive = prtBool($isActive);
///       fwrite($logFileHandle, "IsActive: $printableIsActiven");
///       $isFrozen = $longTransactionReader->IsFrozen();
///       $printableIsFrozen = prtBool($isFrozen);
///       fwrite($logFileHandle, "IsFrozen: $printableIsFrozenn");
///       $parentLongTransactionReader = $longTransactionReader->GetParents();
///       if ($parentLongTransactionReader == NULL)
///       {
///          fwrite($logFileHandle, "There is no parent long transactionn");
///       }
///       else
///       {
///          fwrite($logFileHandle, "Printing parent long transaction readern");
///          printLongTransactionReader($parentLongTransactionReader);
///          $parentLongTransactionReader->Close();
///       }
///       $childrenLongTransactionReader = $longTransactionReader->GetChildren();
///       if ($childrenLongTransactionReader == NULL)
///       {
///          fwrite($logFileHandle, "There are no child long transactionsn");
///       }
///       else
///       {
///          fwrite($logFileHandle, "Printing children long transaction readern");
///          printLongTransactionReader($childrenLongTransactionReader);
///          $childrenLongTransactionReader->Close();
///       }
///    }
///    fwrite($logFileHandle, "Printed $i long transaction(s)n");
/// }
/// ?>
/// \endcode
/// \htmlinclude ExampleBottom.html
class MG_PLATFORMBASE_API  MgLongTransactionReader : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgLongTransactionReader)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the long transaction currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the long transaction.
    ///
    STRING GetName();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of the long transaction currently being
    /// read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the description of the long transaction.
    ///
    STRING GetDescription();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Provides access to the direct descendant long transactions
    /// for the long transaction being currently read.
    ///
    /// \remarks
    /// The caller can use the returned reader to get to the next
    /// level of descendants if required. The returned reader is
    /// empty if there are no descendants for the long transaction.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLongTransactionReader GetChildren();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLongTransactionReader GetChildren();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLongTransactionReader GetChildren();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgLongTransactionReader instance containing the
    /// direct descendants for the long transaction currently being
    /// read.
    ///
    MgLongTransactionReader* GetChildren();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Provides access to the direct ascendant long transactions for
    /// the long transaction being currently read.
    ///
    /// \remarks
    /// The caller can use the returned reader to get to the next
    /// level of ascendants if required. The returned reader is empty
    /// if there are no ascendants for the long transaction.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLongTransactionReader GetParents();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLongTransactionReader GetParents();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLongTransactionReader GetParents();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgLongTransactionReader instance containing the
    /// direct ascendants for the long transaction currently being
    /// read.
    ///
    MgLongTransactionReader* GetParents();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the owner of the long transaction currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetOwner();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetOwner();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetOwner();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the owner name.
    ///
    STRING GetOwner();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the creation date of the long transaction currently
    /// being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDateTime GetCreationDate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDateTime GetCreationDate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDateTime GetCreationDate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgDateTime object containing the date when the
    /// long transaction was created.
    ///
    MgDateTime* GetCreationDate();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether the long transaction currently being read
    /// is the active long transaction.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsActive();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsActive();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsActive();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the long transaction is active; otherwise
    /// returns false.
    ///
    bool IsActive();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether the long transaction currently being read
    /// is frozen.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsFrozen();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsFrozen();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsFrozen();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the long transaction is frozen; otherwise
    /// returns false.
    ///
    bool IsFrozen();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the reader to the next long transaction.
    ///
    /// \remarks
    /// The default position of the reader is prior to the first
    /// item. You must call ReadNext before you can access any data.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if there is a next item.
    ///
    bool ReadNext();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Closes the MgLongTransactionReader object, freeing any
    /// resources it may be holding.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    void Close();

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes the long transactions in the reader to XML whose structure
    /// is governed by the FdoLongTransactionList schema.
    ///
    /// \return
    /// Returns an MgByteReader object containing the contents of
    /// the long transaction reader in XML format.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <FdoLongTransactionList xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="FdoLongTransactionList-1.0.0.xsd">
    ///    <ProviderName>OSGeo.ArcSDE</ProviderName>
    ///    <LongTransaction IsActive="false" IsFrozen="true">
    ///        <Name>DEFAULT</Name>
    ///        <Description>Instance default version.</Description>
    ///        <Owner>Instance default version.</Owner>
    ///        <CreationDate>2/18/2004-7:49:0</CreationDate>
    ///    </LongTransaction>
    ///    <LongTransaction IsActive="false" IsFrozen="true">
    ///        <Name>MyLT</Name>
    ///        <Description/>
    ///        <Owner/>
    ///        <CreationDate>11/18/2004-10:4:0</CreationDate>
    ///    </LongTransaction>
    /// </FdoLongTransactionList>
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgByteReader* ToXml();

INTERNAL_API:

    MgLongTransactionReader();
    ~MgLongTransactionReader();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    INT32 AddLongTransactionData(MgLongTransactionData* data);
    void SetProviderName(CREFSTRING providerName);

protected:

    void ToXml(string& str);

    virtual void Dispose()
    {
        delete this;
    }

private:

    MgDisposableCollection m_longTransactionCol;
    INT32 m_currPos;
    STRING m_providerName;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_LongTransactionReader;
};
/// \}

#endif
