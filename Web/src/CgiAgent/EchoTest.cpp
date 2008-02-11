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

#include "MapGuideCommon.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif


void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++)
        printf("%s\n", *envp);
    printf("</pre><p>\n");
}

int EchoTest()
{
    static int count = 0;

    char *contentLength = getenv("CONTENT_LENGTH");
    int len;

    printf("Content-type: text/html\r\n"
        "\r\n"
        "<title>FastCGI echo</title>"
        "<h1>FastCGI echo</h1>\n"
            "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

    if (contentLength != NULL)
    {
        len = strtol(contentLength, NULL, 10);
    }
    else
    {
        len = 0;
    }

    if (len <= 0)
    {
    printf("No data from standard input.<p>\n");
    }
    else
    {
        int i, ch;
        printf("Standard input:<br>\n<pre>\n");
        for (i = 0; i < len; i++)
        {
            if ((ch = getchar()) < 0)
            {
                printf("Error: Not enough bytes received on standard input<p>\n");
                break;
            }
            putchar(ch);
        }
        printf("\n</pre><p>\n");
    }

    PrintEnv("Request environment", environ);

    return 0;
}
