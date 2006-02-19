#ifndef TESTER_LINUX_H
#define TESTER_LINUX_H

#define TESTRESULTSPATH L"../../TestResults/"
#define GETUSER char* userName = getenv("USER");\
    if (userName == NULL)\
    {\
        fprintf(stderr, "Failed to lookup username.\n");\
        exit(1);\
    }\

#endif
