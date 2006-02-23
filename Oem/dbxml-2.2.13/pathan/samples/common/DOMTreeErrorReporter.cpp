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
 * $Log: DOMTreeErrorReporter.cpp,v $
 * Revision 1.1  2004/07/21 09:22:58  jpcs
 * Initial check in of the Parthenon backup of Pathan.
 *
 * Revision 1.5  2004/07/12 08:36:41  swhite
 * Restored old, uncorrupted, copy.  What is it about perl -pi -e on NFS?
 *
 * Revision 1.3  2003/09/02 12:13:13  gareth
 * we were not setting the error handler so these tests were not reporting.They also report slightly differently in the new way of doing things.
 *
 * Revision 1.2  2003/08/04 12:42:29  alby
 * Added missing std prefixes
 *
 * Revision 1.1  2003/07/31 12:54:36  crioux
 * * Moved eval out of functionalTesting and into samples/evaluator
 * * Updated all tester.xml files to point to the right location
 * * moved DOMTreeErrorReporter out of xgrep and into common
 * * changed xgrep and eval to point to the common directory and no longer
 *   use the local include trick that was confusing
 * * DOMTreeErrorReporter now implements DOMErrorHandler
 *
 * Revision 1.4  2003/07/29 11:47:59  crioux
 * Removed usage of DOMSerialize. Fixed sequence/id tests. Due to a bug in xerces getTextContent(), some sequence tests do not work yet. Fixed some cut-n-paste errors in FunctionId and FunctionIdref
 *
 * Revision 1.3  2003/06/24 10:57:05  crioux
 * added tests for Base64Binary, and remove a nested comment in xgrep/DOMTreeErrorReporter.cpp to prevent gcc from complaining
 *
 * Revision 1.2  2003/02/24 15:27:50  alby
 * Remove warning occurring with gcc 3.2 (about using deprecated header iostream.h)
 *
 * Revision 1.1  2002/12/09 10:11:22  nfg
 * Moved examples/ * to samples/ *.
 *
 * Revision 1.8  2002/11/07 17:16:19  gareth
 * ported to the new fantastic namespace scheme of xerces
 *
 * Revision 1.7  2002/08/02 14:45:04  gareth
 * changed include paths over so we only have to include the xercesc source directory.
 *
 * Revision 1.6  2002/08/01 12:53:03  gareth
 * all examples except for the basic interface tests now compile. The interface ones will have to wait for tom.
 *
 * Revision 1.5  2002/07/31 15:51:31  tom
 * moving over to xerces 2.0. There will be many problems after this. The tree builds but thats it.
 *
 * Revision 1.4  2002/07/23 16:41:07  gareth
 * moved over to use xerces 2.0.0 .everyone will haev to run DSLconfigure again. You should still be able to use older xerces, just change the path directory in Makefile.defs.in.
 *
 * Revision 1.3  2002/03/14 09:52:08  tom
 * #include <> rather than #include <dom/>.
 * Improve make clean.
 * Removed obselete files from make install and distrib scripts.
 *
 * Revision 1.2  2001/07/25 13:18:44  jpcs
 * Added VC6 project files for pathan, pathan-testharness, and xgrep.
 * Fixed pathan to build under windows.
 *
 * Revision 1.1  2001/05/25 18:11:07  jpcs
 * Sorted out the main Pathan makefiles, to generate the include files.
 * Added an example project, xgrep, that shows how to use Pathan.
 *
 * Revision 1.1  2001/02/09 16:40:16  gareth
 * added Xerces 1_4
 *
 * Revision 1.6  2000/08/24 22:36:22  andyh
 * Fix DOMPrint crash when input xml file is not found.
 *
 * Revision 1.5  2000/05/15 22:31:06  andyh
 * Replace #include<memory.h> with <string.h> everywhere.
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
 * Revision 1.6  1999/11/08 20:43:35  rahul
 * Swat for adding in Product name and CVS comment log variable.
 *
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/SAXParseException.hpp>
#include "DOMTreeErrorReporter.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <xercesc/util/XMLString.hpp>

void DOMTreeErrorReporter::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void DOMTreeErrorReporter::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
{
	std::cerr << "Error at file \"" << toCatch.getSystemId()
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage()) << std::endl;
    throw XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException(toCatch);  // Copy the 'toCatch' object before throwing - 
                                       //   otherwise we would be throwing a reference to
                                       //   a local object that gets destroyed before
                                       //   the catch.
}

void DOMTreeErrorReporter::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
{
	std::cerr << "Fatal Error at file \"" << toCatch.getSystemId()
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage()) << std::endl;
    throw XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException(toCatch);
}

void DOMTreeErrorReporter::resetErrors()
{
    // No-op in this case
}


bool DOMTreeErrorReporter::handleError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError& domError) {

	std::cerr << "Message: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(domError.getMessage()) << std::endl;

    return true;
}

