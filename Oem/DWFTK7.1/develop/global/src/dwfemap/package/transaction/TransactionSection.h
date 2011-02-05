//
//  Copyright (C) 2003-2011 by Autodesk, Inc.
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
#ifndef _DWFEMAPTK_TRANSACTIONSECTION_H
#define _DWFEMAPTK_TRANSACTIONSECTION_H

#include "../../EMapToolkit.h"
#include "./TransactionSectionDescriptorReader.h"
#include "dwf/package/Section.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFEMapCommand;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapTransactionSection represents the root document 
    // level element of an EMapTransaction Section. 
    // As such, it offers unique functionality amongst the EMapTransaction
    // classes. It is responsible for the correct serialization and 
    // de-serialization of the EMapTransaction schema.
    // In addition to being the root object in an EMap document it also 
    // is a container for all the Resources and properties required by any 
    // EMapTransaction schema element.
    //--------------------------------------------------------------------
    class DWFEMapTransactionSection : public DWFSection,
                                      public DWFEMapTransactionSectionDescriptorReader
    {
    public:
        class Factory : public DWFSection::Factory
        {
        public:
            // constructor / destructor
            _DWFTK_API Factory() throw();
            _DWFTK_API virtual ~Factory() throw();

            // inherited from DWFSection::Factory
            _DWFTK_API virtual DWFSection* build( const DWFString& zName,
                                                  const DWFString& zTitle,
                                                  DWFPackageReader* pPackageReader )
                                                  throw( DWFException );
        };

    public:
        // basic constructors
        _DWFTK_API DWFEMapTransactionSection( const DWFString& zName,
                                              const DWFString& zTitle,
                                              DWFPackageReader* pPackageReader )
                                              throw();

        _DWFTK_API DWFEMapTransactionSection( const DWFString& zTitle,
                                              const DWFString& zObjectID,
                                              const DWFSource& rSource,
                                              unsigned int nSequenceID,
                                              const DWFString& zMapObjectID )
                                              throw();

        // destructor
        _DWFTK_API virtual ~DWFEMapTransactionSection() throw();

        // property accessors
        _DWFTK_API unsigned int sequenceID() const throw();
        _DWFTK_API const DWFString& mapObjectID() const throw();

        // returns the command at the specified index, or NULL if the
        // index value is out of range
        _DWFTK_API const DWFEMapCommand* getCommand( size_t index ) const throw();

        // returns the number of commands in this section
        _DWFTK_API size_t commandCount() const throw();

        // adds a command to the EMapTransaction section
        _DWFTK_API void addCommand( DWFEMapCommand* pCommand ) throw( DWFException );

        // XMLCallback support
        _DWFTK_API void notifyStartElement( const char* zName,
                                            const char** ppAttributeList ) throw();
        _DWFTK_API void notifyEndElement( const char* zName ) throw();
        _DWFTK_API void notifyStartNamespace( const char* zPrefix,
                                              const char* zURI ) throw();
        _DWFTK_API void notifyEndNamespace( const char* zPrefix ) throw();
        _DWFTK_API void notifyCharacterData( const char* zCData, int nLength ) throw();

        // This method will locate the one descriptor resource in the section
        // and parse it accordingly with the reader provided or the default reader
        // for the section (this object).  In order to use another reader as a filter
        // first call setFilter() on this EMapTransactionSection object.
        _DWFTK_API const DWFResource& readDescriptor(
            DWFSectionDescriptorReader* pSectionDescriptorReader = NULL ) const
            throw( DWFException );

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML( DWFXMLSerializer& rSerializer,
                                      unsigned int nFlags ) throw( DWFException );
   
    private:
        // hidden copy constructor and assignment operator
        DWFEMapTransactionSection( const DWFEMapTransactionSection& );
        void operator=( const DWFEMapTransactionSection& );

        // methods
        void _processAttributes( const char** ppAttributeList ) throw();

        // data members
        DWFString _zMapObjectID;
        unsigned int _nSequenceID;

        typedef std::vector< DWFEMapCommand* > CommandCollection;
        CommandCollection _oCommands;

        // non-persisted variables to help parse the XML data
        unsigned char _nFlag;
        DWFEMapCommand* _poCommand;
        DWFPackageReader* _poPackageReader;
    };
}}

#endif // _DWFEMAPTK_TRANSACTIONSECTION_H
// End of file.
