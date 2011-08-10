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
#ifndef _MGGCTTERSTPARAMETERS_H_
#define _MGGCTTERSTPARAMETERS_H_

/// \defgroup MgGeometryConsoleTest MgGeometryConsoleTest
/// \ingroup Geometry_Module_classes
/// \{
class MgGctTestParameters
{
    static const INT32 m_nMaxPositionalArgumentCount = 8;
    static const INT32 m_nMaxPositionalArgumentSize = 1024;
    static wchar_t m_cOptionChar;
    static wchar_t m_cDirectorySeparator;
    static bool m_bDefaultVerbose;
    static bool m_bDefaultSilent;
    static bool m_bDefaultBackground;
    static bool m_bDefaultRadiiCheck;
    static INT32 m_nDefaultDuration;
    static wchar_t m_cDefaultLocale [128];
    static wchar_t m_cDefaultTestSpecification [128];
    static wchar_t m_cDefaultConfigurationURL [1024];
    static wchar_t m_cDefaultTestDataFolder [1024];
    static wchar_t m_cDefaultCrsTestDataFile [128];
    static wchar_t m_cDefaultRegressionTestDataFile [128];

public:
    MgGctTestParameters (void);
    MgGctTestParameters (const MgGctTestParameters& source);
    virtual ~MgGctTestParameters (void);
    MgGctTestParameters& operator= (const MgGctTestParameters& lhs);

    // For the following, the "argument" parameter is expected to include the command
    // line argumnet character.
    bool Initialize (int argc,const char* argv[]);
    bool AddArgument (const char* argument);
    INT32 GetTestCount (void);
    wchar_t GetTestChar (INT32 index);
    INT32 GetPositionalCount (void); 
    const wchar_t* GetPositionalArgument (INT32 index);

    bool Verbose (void) const {return m_bVerbose; };
    bool Silent (void) const {return m_bSilent; };
    bool Background (void) const {return m_bBackground; };
    bool RadiiCheck (void) const {return m_bRadiiCheck; };
    INT32 GetDuration (void) const {return m_nDuration; };
    UINT32 GetRandomSeed (void) const {return m_nSeed; };
    const wchar_t* GetLocale (void) const {return m_cLocale; };
    const wchar_t* GetTestSpecification (void) const {return m_cTestSpecification; };
    const wchar_t* GetConfigurationURL (void) const {return m_cConfigurationURL; };
    const wchar_t* GetTestDataFolder (void) const {return m_cTestDataFolder; };
    const wchar_t* GetCrsTestDataFile (void) const {return m_cCrsTestDataFile; };
    const wchar_t* GetRegressionTestDataFile (void) const {return m_cRegressionTestDataFile; };
    const wchar_t* GetReason (void) const {return m_cReason; };
    void Usage (bool batch);

private:
    void InitializeToDefaultValues (void);
    bool ProcessOption (const char* option);
    bool ProcessPositional (const char* argument);

    INT32 m_nPositionalArgumentCount;       // Number of positional arguments.
    bool m_bVerbose;                        // Produces detailed error/warning/progress reports.
    bool m_bSilent;                         // Produces nothing but an exit status value.
    bool m_bBackground;                     // Does not require achnowledgement before exit.
    bool m_bRadiiCheck;                     // true disables any and all checks on ellipsoid radii, useful as
                                            // many test cases use the unit sphere.
    INT32 m_nDuration;                      // A percentage of the normal test duration, values greater than 100 are supported.
    UINT32 m_nSeed;                         // User specified seed value for the random number generator.
    wchar_t m_cLocale [128];                // The locale currently in effect for l;ocale sensitive functions.
    wchar_t m_cTestSpecification [128];     // Each character indactes a specific test to be run, in the order they are to be run.
    wchar_t m_cConfigurationURL [1024];     // Path to the configuration file to use.  In the case of
                                            // CS-MAP, this will be the path to the Dictionaries folder.
                                            // In either case, it is the argument provided to the initialization
                                            // code of whatever library is in use.
    wchar_t m_cTestDataFolder [1024];       // Path to the location of the test data files used by this
                                            // application by default.
    wchar_t m_cCrsTestDataFile [128];       // Name of the test data file implied by test 4 and A.
    wchar_t m_cRegressionTestDataFile [128];// Name of the test data file implied by test 4 and A.
    wchar_t m_cPositionals [m_nMaxPositionalArgumentCount][m_nMaxPositionalArgumentSize];
                                            // Positional arguments
    wchar_t m_cReason [1024];               // In the event of a failure of some sort, a description of
                                            // the reason for the failure is delivered here.
};

/// \}

#endif //_MGGCTTERSTPARAMETERS_H_
