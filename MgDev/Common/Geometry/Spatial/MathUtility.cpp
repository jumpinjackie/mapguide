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

#include "Foundation.h"
#include "MathUtility.h"

using namespace std;


double MgMathUtility::GetPi()
{
    return M_PI;
}


double MgMathUtility::GetQuietNan()
{
    return numeric_limits<double>::quiet_NaN();
}


bool MgMathUtility::IsNan(double n)
{
#ifdef _WIN32
    return _isnan(n);
#else
    return isnan(n);
#endif
}


bool MgMathUtility::AreEqualAndMaybeNan(double a, double b)
{
    return (a == b) || (IsNan(a) && IsNan(b));
}


double MgMathUtility::SnapToZero(double n)
{
#define VERY_SMALL (1.0e-17)

    double snapped = n;

    if (fabs(n) <= VERY_SMALL)
        snapped = 0.0;

    return snapped;
}


// Implementation of LUDecompose() is taken from the AGDS codebase: ut_ludecomp(), by Derek Wang
int MgMathUtility::LUDecompose(int n, double a[], int eindex[])
{
    // Decompose a into LU, where L is lower triangular matrix and U is upper
    // triangular matrix. We also have L[i][i] = 1 for i=1,2,...

    int dim = 0;
#define MATRIX(i, j)        (*(ptr_matrix + ((i)*dim + (j))))
    double* ptr_matrix = NULL;
    int i, j, k, col_max;
    int sign = 1;   // det(matrix) sign change as a result of row exchange
    double largest; // the pivot element
    double tmp, *scale = NULL;

    // initialize static vars

    CHECKARGUMENTNULL(a, L"MgMathUtility.LUDecompose");
    CHECKARGUMENTNULL(eindex, L"MgMathUtility.LUDecompose");

    if (n <= 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(n, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgMathUtility.LUDecompose",
            __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryNoDimension", NULL);
    }

    dim = n;
    ptr_matrix = a;

    // fill scale for each row
    scale = new double[n];
    CHECKNULL(scale, L"MgMathUtility.LUDecompose")

    for (i=0; i<n; ++i)
    {
        largest = 0;
        for (j=0; j<n; ++j)
        {
            if ((tmp = fabs(MATRIX(i,j))) > largest)
                largest = tmp;
        }
        if (SnapToZero(largest) == 0.0)
        {
            delete [] scale;
            return 0;
        }
        scale[i] = 1.0 / largest;
    }

    for (j=0; j<n; ++j)
    {
        for (i=0; i<=j; ++i)
        {
            for (k=0; k<i; ++k)
                MATRIX(i,j) -= MATRIX(i,k) * MATRIX(k,j);
        }
        col_max = j;
        largest = scale[j]*fabs(MATRIX(j,j));
        for (i=j+1; i<n; ++i)
        {
            for (k=0; k<j; ++k)
                MATRIX(i,j) -= MATRIX(i,k) * MATRIX(k,j);
            if ((tmp = scale[i]*fabs(MATRIX(i,j))) > largest)
            {
                col_max = i;
                largest = tmp;
            }
        }
        if (j != col_max)
        {
            // interchange rows
            for (k=0; k<n; ++k)
            {
                tmp = MATRIX(j,k);
                MATRIX(j,k) = MATRIX(col_max, k);
                MATRIX(col_max, k) = tmp;
            }
            // interchange the scale factor
            scale[col_max] = scale[j];
            sign = -sign;
        }
        eindex[j] = col_max;
        if (SnapToZero(largest) == 0.0)
        {
            // if pivot element is near zero, matrix is singular
            delete [] scale;
            return 0;
        }
        // divide by pivot element
        if (j < n-1)
        {
            tmp = 1.0 / MATRIX(j,j);
            for (i=j+1; i<n; ++i)
                MATRIX(i,j) *= tmp;
        }
    }

    delete [] scale;
    return sign;
}


double MgMathUtility::LinearInterpolate(double start, double end, double proportion)
{
    double interpolated = GetQuietNan();

    if (!IsNan(start) && !IsNan(end) && !IsNan(proportion))
    {
        if (proportion < 0.0 || proportion > 1.0)
        {
            STRING buffer;
            MgUtil::DoubleToString(proportion, buffer);

            MgStringCollection arguments;
            arguments.Add(L"3");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgMathUtility.LUDecompose",
                __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryBadProportion", NULL);
        }

        if (SnapToZero(fabs(end-start)) == 0.0)
        {
            // The range is essentially zero.  Snap to the first value.
            interpolated = start;
        }
        else
        {
            if (start > end)
            {
                // The range is in reverse.  Turn it around.
                double tmp = start;
                start = end;
                end = tmp;
                proportion = 1.0 - proportion;
            }
            double range = end - start;
            interpolated = start + ( range * proportion );
        }
    }

    return interpolated;
}

///////////////////////////////////////////////////////////////////////////////
// This overload of DblCmp __MUST NOT__ be used if either argument can
// be a hard zero.
bool MgMathUtility::DblCmp (double first,double second)
{
    int exp1, exp2;
    int deltaExp;
    double mant1,mant2;

    mant1 = frexp (first ,&exp1);
    mant2 = frexp (second,&exp2);
    deltaExp = exp1 - exp2;
    if (abs (deltaExp) >= 2) return false;

    // Need to deal with the case where the difference between the exponents
    // is +/-1.  This is necessary as the two values may be infinitesimally
    // greater and less than an even true binary value like 0.5.  In this
    // case, they would be equal, but the exponents would not be the same.
    if (deltaExp == 1)
    {
        // exp of first is bigger than exp of second.   Adjust exp2 and mant2
        // so that they still represent the same value, but exp2 is now the
        // same as exp1.  Note, this de-normalizes the numbers,  but that
        // does not affect the comparison result in any way.
        exp2 += 1;
        mant2 *= 0.5;
    }
    else if (deltaExp == -1)
    {
        exp1 += 1;
        mant1 *= 0.5;
    }

    // Now, if the exponents are not equal, obviously the original values
    // are not equal; not even close.
    if (exp1 != exp2) return false;

    // Since the exponents are equal, then we can simply compare the mantissas.
    // We ignore any difference in the last few bits.
    return (fabs (mant1 - mant2) < 5.0E-13);
}

///////////////////////////////////////////////////////////////////////////////
// Use this overload of DblCmp if either argument can be expected to be a
// hard zero.
bool MgMathUtility::DblCmp (double first,double second,double tolerance)
{
    bool equal;

    equal = (fabs (first - second) < tolerance);
    return equal;
}
