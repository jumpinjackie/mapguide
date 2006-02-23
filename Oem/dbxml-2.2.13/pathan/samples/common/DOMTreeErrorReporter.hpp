
/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 1999-2000 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

/*
 * $Log: DOMTreeErrorReporter.hpp,v $
 * Revision 1.1  2004/07/21 09:22:58  jpcs
 * Initial check in of the Parthenon backup of Pathan.
 *
 * Revision 1.1  2003/07/31 12:54:37  crioux
 * * Moved eval out of functionalTesting and into samples/evaluator
 * * Updated all tester.xml files to point to the right location
 * * moved DOMTreeErrorReporter out of xgrep and into common
 * * changed xgrep and eval to point to the common directory and no longer
 *   use the local include trick that was confusing
 * * DOMTreeErrorReporter now implements DOMErrorHandler
 *
 * Revision 1.3  2003/06/25 14:08:16  crioux
 * * changed the docs on the function isTypeOrDerivedFromType to make it clear
 * * changed use of isTypeOrDerivedFromType in AnyAtomicType
 * * Added tests for ATDecimalOrDerived (not finished)
 * * Added catching of XPathException and DSLException is test-datatypes
 * * Added xs binding to test-datatypes
 * * fixed a nested 'comment' in DOMTreeErrorReporter.hpp
 * * changed ATHexBinaryOrDerived to store the hexBinary in upper case
 * * changed ATDecimalOrDerived to output correct canonical representation
 *
 * Revision 1.2  2002/12/29 18:00:57  tom
 * Many source changes. Pathan and eval now compile and link.
 *
 * Revision 1.1  2002/12/09 10:11:22  nfg
 * Moved examples/ * to samples/ *.
 *
 * Revision 1.5  2002/11/07 17:16:19  gareth
 * ported to the new fantastic namespace scheme of xerces
 *
 * Revision 1.4  2002/07/23 16:41:07  gareth
 * moved over to use xerces 2.0.0 .everyone will haev to run DSLconfigure again. You should still be able to use older xerces, just change the path directory in Makefile.defs.in.
 *
 * Revision 1.3  2002/03/14 09:52:08  tom
 * #include <> rather than #include <dom/>.
 * Improve make clean.
 * Removed obselete files from make install and distrib scripts.
 *
 * Revision 1.2  2001/12/14 12:35:41  tom
 * Merge between dom3-xpath and head pre moving CVS to external box.
 *
 * Revision 1.1.2.1  2001/12/04 12:14:24  tom
 * Many #include etc. changes for Windows build.
 *
 * Revision 1.1  2001/05/25 18:11:07  jpcs
 * Sorted out the main Pathan makefiles, to generate the include files.
 * Added an example project, xgrep, that shows how to use Pathan.
 *
 * Revision 1.1  2001/02/09 16:40:16  gareth
 * added Xerces 1_4
 *
 * Revision 1.4  2000/03/02 19:53:40  roddey
 * This checkin includes many changes done while waiting for the
 * 1.1.0 code to be finished. I can't list them all here, but a list is
 * available elsewhere.
 *
 * Revision 1.3  2000/02/06 07:47:18  rahulj
 * Year 2K copyright swat.
 *
 * Revision 1.2  1999/12/03 00:14:53  andyh
 * Removed transcoding stuff, replaced with DOMString::transcode.
 *
 * Tweaked xml encoding= declaration to say ISO-8859-1.  Still wrong,
 * but not as wrong as utf-8
 *
 * Revision 1.1.1.1  1999/11/09 01:09:51  twl
 * Initial checkin
 *
 * Revision 1.4  1999/11/08 20:43:36  rahul
 * Swat for adding in Product name and CVS comment log variable.
 *
 */

#include <xercesc/sax/ErrorHandler.hpp>
#include <iostream>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>

class DOMTreeErrorReporter : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler, public XERCES_CPP_NAMESPACE_QUALIFIER DOMErrorHandler 
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMTreeErrorReporter()
    {
    }

    ~DOMTreeErrorReporter()
    {
    } 


    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void resetErrors();


    virtual bool handleError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError& domError);
};


