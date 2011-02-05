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

#ifndef MgMathUtility_H
#define MgMathUtility_H

#include <limits>       // For quiet_NaN()
#include <float.h>      // For _isnan()
#ifdef _WIN32
// For M_PI in math.h on Windows (doesn't seem to work though).
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

/// \cond INTERNAL
// Mathematical Utility.
class MgMathUtility
{
public:

    /// Get the value of PI.  The constant name isn't a C++ standard, although
    /// this method will use whatever the local compiler offers.
    static double GetPi();

    /// Gets a NaN (not-a-number) value that doesn't cause exceptions in
    /// normal assignments and comparisons.
    /// This is numeric_limits<double>::quiet_NaN(), just without the
    /// sometimes bothersome header file requirements.
    static double GetQuietNan();

    /// Returns true if the value is not-a-number.
    static bool IsNan(double n);

    /// Returns true if two numbers are equal (no tolerance), even if they are NaN.
    /// Normally, IEEE rules have all NaN values failing an equality test.
    static bool AreEqualAndMaybeNan(double a, double b);

    /// Snaps a number to 0.0 if it is of extremely small magnitude.
    /// This is for avoiding mathematical problems such as underflow,
    /// division by zero, major loss of precision, etc.
    /// It is NOT for enforcing any tolerance floor in a coordinate system.
    /// It does use an internal, tiny tolerance, whose value should not be assumed.
    static double SnapToZero(double n);

    /// Decompose input matrix a into matrices L and U
    /// such that L[i][i] = 1, L[i][j] = 0 for i,j = 0, ..., n-1 and
    /// j > i; U[i][j] = 0 for i,j = 0, ..., n-1 and i < j.
    /// The resulting L and U are written back to matrix a such that
    /// a[i*n + j] = L[i][j] if i < j; otherwsie a[i*n+j] = U[i][j],
    /// for i, j = 1,2, ..., n.
    ///
    /// Parameters
    ///    n: input
    ///        Dimension of matrix a
    ///
    ///    a: updated
    ///        Matrix (represented by an array of siez n*n) to be decomposed,
    ///        where the element at the ith row and the jth column is
    ///        a[ i*n + j ] for i,j = 0, ..., n;
    ///
    ///    eindex: output
    ///        Row interchange indices, which should be interpreted orderedly
    ///        from eindex[0] to eindex[n-1] such that if eindex[i] != i, it
    ///        implies a row interchange between row i and row eindex[i].
    /// Function value
    ///       -1: Failure due to the singularity of the matrix;
    ///        0: Even row interchanges;
    ///        1: odd row interchanges;
    static int LUDecompose( int n, double a[], int eindex[] );

    /// Interpolate a number in the range [start,end].  'proportion' must
    /// be in the range [0.0,1.0].
    /// 'start > end' is supported.
    /// It is allowable to pass NaN for any parameter, in which case the return
    /// value is also a (quiet) NaN.
    static double LinearInterpolate(double start, double end, double proportion);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Reliably compares two doubles, providing that neither of the two values
    /// is a hard zero, ignoring differences in the least significant bits of
    /// the mantissa which are unreliable if either of the values being
    /// compared are the result of some serious calculations.
    /// </summary>
    /// <param name="first">
    /// The first of two doubles to be compared.  Must not be a hard zero.
    /// </param>
    /// <param name="second">
    /// The second of two doubles to be compared.  Must not be a hard zero.
    /// </param>
    /// <returns>
    /// Returns <c>true</c> for "essentially equal".
    /// </returns>
    static bool DblCmp (double first,double second);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Reliably compares two doubles ignoring differences in the least
    /// significant bits of the mantissa which are unreliable if either of
    /// the values being compared are the result of some serious calculations.
    /// </summary>
    /// <param name="first">
    /// The first of two doubles to be compared.
    /// </param>
    /// <param name="second">
    /// The second of two doubles to be compared.
    /// </param>
    /// <param name="tolerance">
    /// If the difference between the two values is euqal to or less than
    /// this value, the function returns true.
    /// </param>
    /// <returns>
    /// Returns <c>true</c> for "essentially equal".
    /// </returns>
    static bool DblCmp (double first,double second,double tolerance);

};
/// \endcond

#endif
