//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "FdoUnitTesting.h"
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int Execute(CREFSTRING fileName)
{
    int nResult = 0;

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    if (fileName.size() > 0)
    {
        ofstream outfile(MG_WCHAR_TO_CHAR(fileName.c_str()));

        if (outfile.is_open())
        {
            runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), outfile, "ISO-8859-1"));
            runner.run();
            outfile.close();
        }
    }
    else
    {
        runner.setOutputter(new CppUnit::TextOutputter(&runner.result(), std::cout));
        runner.run();
    }

    int nFailures = runner.result().testFailuresTotal();
    if(nFailures > 0)
    {
        nResult = -1;
    }

    return nResult;
}
