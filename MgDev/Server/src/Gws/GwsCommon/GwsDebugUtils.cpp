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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


#ifdef _DEBUG
void GwsDebugUtils::TraceToFile (const WSTR & msg)
{
    char * fname = getenv("GWS_TRACE_FILE");
    if (fname == NULL || * fname == 0)
        return;

    FILE * fh = fopen (fname, "a+");
    if (fh != NULL) {
        fwprintf (fh, L"%ls\n", msg.c_str ());
        fclose (fh);
    }
}

void GwsDebugUtils::TraceToFile (
    const WSTR & /*evar*/,
    const WSTR & msg
)
{
//  char * fname = getenv(evar.c_str ());
    char * fname = NULL;
    if (fname == NULL || * fname == 0)
        return;

    FILE * fh = fopen (fname, "a+");
    if (fh != NULL) {
        fwprintf (fh, L"%ls\n", msg.c_str ());
        fclose (fh);
    }
}
#endif
