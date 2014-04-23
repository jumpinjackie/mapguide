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

#include "GeometryCommon.h"
#include "GeometryConsoleTest.h"

static wchar_t moduleName [] = L"GctTestA.cpp";

#define MAX_THREADS 16
static struct MgCrsThreadStatusEntry
{
    INT32 nThreadNbr;
    HANDLE nThreadHandle;
    DWORD nThreadId;
    clock_t startTime;
    clock_t endTime;
    INT32 exeCount;
    DWORD threadStatus;
    DWORD status;
} MgCrsThreadStatusTable [MAX_THREADS] =
{
    { 1,   0,   0L,   0L,  0,  0, 0 },
    { 2,   0,   0L,   0L,  0,  0, 0 },
    { 3,   0,   0L,   0L,  0,  0, 0 },
    { 4,   0,   0L,   0L,  0,  0, 0 },
    { 5,   0,   0L,   0L,  0,  0, 0 },
    { 6,   0,   0L,   0L,  0,  0, 0 },
    { 7,   0,   0L,   0L,  0,  0, 0 },
    { 8,   0,   0L,   0L,  0,  0, 0 },
    { 9,   0,   0L,   0L,  0,  0, 0 },
    {10,   0,   0L,   0L,  0,  0, 0 },
    {11,   0,   0L,   0L,  0,  0, 0 },
    {12,   0,   0L,   0L,  0,  0, 0 },
    {13,   0,   0L,   0L,  0,  0, 0 },
    {14,   0,   0L,   0L,  0,  0, 0 },
    {15,   0,   0L,   0L,  0,  0, 0 },
    {16,   0,   0L,   0L,  0,  0, 0 }
};

DWORD WINAPI GctAThreadTest (void* pThreadParms);
bool GctADblCmp (double first,double second,double tolerance);

INT32 GctTestA (MgGctTestParameters& cmdLineParms)
{
    INT32 index;
    INT32 errorCount (0);

    wostream& rptStrm (std::wcerr);

    MgCoordinateSystemTestThreadParams sThreadParams;
    Ptr<MgCoordinateSystemCatalog> ptrCatalog;
    MgCrsThreadTestDataVector threadTestVector;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[A] Coordinate conversion reentrancy test "
                << std::endl;
    }

    // The API throws execptions for all errors.  We need to catch them all
    // and return a "failed" status.  In this test, we are not testing
    // behavior in the erroneous state; that's done elsewhere.
    Ptr<MgException> mgException;
    STRING mgStackParams;
    try
    {
        errorCount = 0;
        
        MgCrsThreadTestDataEntry* nextTestPtr;

#if !defined(__SKIP__)        
// Getting the reentrancy test to fail takes some effort.
//
// The following set of tests has demonstrated the reliable ability to produce
// an error in the reentrancy test (Test A) of the GeomatryCOnsoleTest module.
// Thus, we use these tests to verify the reentrancy of the changes made to
// the CS_nadcn.c module.
        nextTestPtr = new MgCrsThreadTestDataEntry (L"CO83-N",L"UTM27-13");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"CO83-C",L"UTM27-13");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"CO83-S",L"UTM27-13");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"WY-E",L"UTM27-13");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"MT83",L"US48");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"UT83-N",L"UTM27-12");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"UT83-C",L"UTM27-12");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"UT83-S",L"UTM27-12");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"IA83-N",L"UTM27-15");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"IA83-S",L"UTM27-15");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"LA83-N",L"UTM27-15");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"LA83-S",L"UTM27-15");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"LA83-O",L"UTM27-15");
        threadTestVector.push_back (nextTestPtr);
#endif
        nextTestPtr = new MgCrsThreadTestDataEntry (L"HARN/CO.CO-C",L"US48");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"NAD27.QuebecLambert",L"CANADA-LCC");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"LL84",L"LL83");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"CO-N",L"LL27");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"LL27",L"LL");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"TN83",L"UTM27-16");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"DHDN/2.Gauss3d-1",L"UTM84-31N");
        threadTestVector.push_back (nextTestPtr);
        nextTestPtr = new MgCrsThreadTestDataEntry (L"TOKYO.UTM-51N",L"JGD2000.UTM-51N");
        threadTestVector.push_back (nextTestPtr);

        sThreadParams.m_pTestData = &threadTestVector;
        sThreadParams.m_cmdLineParms = &cmdLineParms;

        // Create 16 threads
        for (index = 0;index < MAX_THREADS;index += 1)
        {
            MgCrsThreadStatusTable [index].nThreadHandle = CreateThread (NULL,0,GctAThreadTest,&sThreadParams,0,0);
        }

        // Start them up one millisecond apart.
        for (index = 0;index < MAX_THREADS;index += 1)
        {
            DWORD resumeStatus;
            DWORD lastError;
          
            MgCrsThreadStatusTable [index].startTime = clock();
            resumeStatus = ResumeThread (MgCrsThreadStatusTable [index].nThreadHandle);
            if (resumeStatus < 0)
            {
                lastError = GetLastError ();
            }
            else
            {
                MgCrsThreadStatusTable [index].exeCount += 1;
            }
            Sleep (1);
        }

        // We keep cycling the threads until eacah of the 16 threads has executed
        // 20 times.
        clock_t testStart = clock ();
        clock_t testEnd = testStart + 20000 * (cmdLineParms.GetDuration () / 100);
        for (;;)
        {   
            clock_t currentTime = clock ();
            if (currentTime >= testEnd)
            {
                break;
            }
            
            for (index = 0;index < MAX_THREADS;index += 1)
            {
                // Check the current task for completion.
                if (MgCrsThreadStatusTable [index].status != 0)
                {
                    continue;
                }
                DWORD waitStatus = WaitForSingleObject (MgCrsThreadStatusTable [index].nThreadHandle,0);
                if (waitStatus == WAIT_OBJECT_0)
                {
                    MgCrsThreadStatusTable [index].endTime = clock ();
                    MgCrsThreadStatusTable [index].threadStatus = GetExitCodeThread (MgCrsThreadStatusTable [index].nThreadHandle,&MgCrsThreadStatusTable [index].status);
                    CloseHandle (MgCrsThreadStatusTable [index].nThreadHandle);

                    if (MgCrsThreadStatusTable [index].status == 0)
                    {
                        // Create a new thread, and have it start right away.
                        MgCrsThreadStatusTable [index].exeCount += 1;
                        MgCrsThreadStatusTable [index].startTime = clock();
                        MgCrsThreadStatusTable [index].nThreadHandle = CreateThread (NULL,0,GctAThreadTest,&sThreadParams,0,&MgCrsThreadStatusTable [index].nThreadId);
                    }
                }
            }
            Sleep (1);
        }
        for (index = 0;index < MAX_THREADS;index += 1)
        {
            // Check the current task for completion.
            DWORD waitStatus = WaitForSingleObject (MgCrsThreadStatusTable [index].nThreadHandle,2000);
            if (waitStatus == WAIT_OBJECT_0)
            {
                MgCrsThreadStatusTable [index].endTime = clock ();
                MgCrsThreadStatusTable [index].threadStatus = GetExitCodeThread (MgCrsThreadStatusTable [index].nThreadHandle,&MgCrsThreadStatusTable [index].status);
                CloseHandle (MgCrsThreadStatusTable [index].nThreadHandle);
            }
        }

        // Determine sdtatus of the entire test.
        for (index = 0;index < MAX_THREADS;index += 1)
        {
            if (MgCrsThreadStatusTable [index].status != 0)
            {
                errorCount += 1;
            }
        }
    }
    catch (MgException* e)
    {
        mgException = e;
        errorCount = -1;
        rptStrm << L"\tUnexpected MapGuide exception encountered in "
                << moduleName
                << L"."
                << std::endl;
        rptStrm << L"\tMESSAGE: "
                << e->GetExceptionMessage ()
                << std::endl;
        rptStrm << L"\tSTACK TRACE: "
                << e->GetStackTrace ()
                << std::endl;
    }
    catch (...)
    {
        errorCount = -2;
        errorCount = -1;
        rptStrm << L"\tUnexpected exception of unknown type encountered in "
                << moduleName
                << L"."
                << std::endl;
    }

    // All done.
    return errorCount;
}

// The thread function which performs the test.  Our objective is to have
// 16 of these running at a given time.  We assume that on multi-core
// systems, Windows and/or Linux will use separate cores and thus we'll
// have multiple threads doing the same conversions with the same
// Transform objects simultaneoulsy.
DWORD WINAPI GctAThreadTest (void* pThreadParmsVoid)
{
    wostream& rptStrm (std::wcerr);

    DWORD threadStatus (0);
    MgCoordinateSystemTestThreadParams* pThreadParms;
    MgCrsThreadTestDataVector* testDataVector;
	MgGctTestParameters* pCmdLineParms;
    
    MgCoordinateSystemFactory csFactory;

    pThreadParms = static_cast<MgCoordinateSystemTestThreadParams*>(pThreadParmsVoid);
    pCmdLineParms = pThreadParms->m_cmdLineParms;
    testDataVector = pThreadParms->m_pTestData;
    MgCrsThreadTestDataItr testDataItr;

    for (testDataItr = (*testDataVector).begin();testDataItr != (*testDataVector).end();testDataItr++)
    {
        bool ok (true);

        MgCrsThreadTestDataEntry* thisTestPtr = *testDataItr;
        
        Ptr<MgCoordinateSystemTransform> fwdTransform = thisTestPtr->GetForwardTransform ();
        Ptr<MgCoordinateSystemTransform> invTransform = thisTestPtr->GetInverseTransform ();
        Ptr<MgCoordinateCollection> lclPoints = thisTestPtr->GetSrcPointsCopy ();

        Ptr<MgCoordinateIterator> pointItr = lclPoints->GetIterator ();
        while (pointItr->MoveNext ())
        {
            Ptr<MgCoordinate> sourcePtr = pointItr->GetCurrent ();
            Ptr<MgCoordinate> resultPtr = fwdTransform->Transform (sourcePtr->GetX (),sourcePtr->GetY());
            sourcePtr->SetX (resultPtr->GetX ());
            sourcePtr->SetY (resultPtr->GetY ());
        }
        pointItr->Reset ();
        while (pointItr->MoveNext ())
        {
            Ptr<MgCoordinate> sourcePtr = pointItr->GetCurrent ();
            Ptr<MgCoordinate> resultPtr = invTransform->Transform (sourcePtr->GetX (),sourcePtr->GetY());
            sourcePtr->SetX (resultPtr->GetX ());
            sourcePtr->SetY (resultPtr->GetY ());
        }
        ok = thisTestPtr->VerifyResults (lclPoints);
        if (!ok)
        {
            if (pCmdLineParms->Verbose ())
            {
                rptStrm << L"\tReentrancy failure detected: "
                        << (thisTestPtr->GetTransformName ()).c_str ()
                        << std::endl;
            }
            INT32 failIdx = static_cast<INT32>(testDataItr - (*testDataVector).begin()) + 1;
            threadStatus |= 1 << failIdx;
        }
    }
    return threadStatus;
}

