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

/// \cond INTERNAL

//------------------------------------------------------------------------------
//
// FILE: bufrsort.h.
//
// PURPOSE: Declaration of the BufferSort<> class template. BufferSort<> imple-
//          ments functionality that allows objects allocate by a BufferAlloca-
//          tor<> object to be sorted into ascending order. The class refer-
//          enced by the template parameter must support an operator<() method.
//          These BufferSort<> template is intended for internal use by the
//          buffering library.
//
//------------------------------------------------------------------------------

#ifndef _BUFRSORT_H_
#define _BUFRSORT_H_

// an enumeration defining the two supported sorting algorithms

enum SortMethod {QUICKSORT, HEAPSORT};


template <class ObjType> class BufferSort {
public:
    /// method to sort the objects allocated by a BufferAllocator<> object

    void Sort(SortMethod method, BufferAllocator<ObjType> &allocator,
        int nStepsPerInterval, ProgressCallback &callback);

private:
    /// implementation methods

    void QuickSort(BufferAllocator<ObjType> &allocator, int left, int right,
        int nStepsPerInterval, ProgressCallback &callback) const;
    void HeapSort(BufferAllocator<ObjType> &allocator, int nStepsPerInterval,
        ProgressCallback &callback) const;
    void Sift(BufferAllocator<ObjType> &allocator, int left, int right) const;
    int GetNProgressSubIntervals(int nSteps, int nStepsPerInterval) const;
};


//------------------------------------------------------------------------------
//
// METHOD: Sort().
//
// PURPOSE: Sort the list of IntersectionRecord objects into lexicographic
//          order on the points of intersection.
//
// PARAMETERS:
//
//     Input:
//
//         sortMethod        - passes an enumerated value of type SortMethod
//                             specifying which algorithm to use.
//         allocator         - reference to the BufferAllocator<> object
//                             containing
//                             the objects to be sorted.
//         nStepsPerInterval - passes an integer specifying the number of
//                             "steps" per progress interval.
//         callback          - passes a reference to a ProgressCallback object.
//                             The referenced callback object is periodically
//                             queried to determine whether or not to cancel
//                             processing, and to report progress.
//
//     Output:
//
//         allocator         - the objects are sorted on ascending order on
//                             return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> void BufferSort<ObjType>::Sort(SortMethod method,
    BufferAllocator<ObjType> &allocator, int nStepsPerInterval,
    ProgressCallback &callback)
{
    /// call specified sort method

    if (method == QUICKSORT) {
        int nObjects = allocator.GetNObjects();
        callback.BeginProgressInterval(GetNProgressSubIntervals(nObjects,
            nStepsPerInterval));
        QuickSort(allocator, 0, nObjects - 1, nStepsPerInterval, callback);
        callback.EndProgressInterval();
    }
    else
        HeapSort(allocator, nStepsPerInterval, callback);

} // end: Sort()


//------------------------------------------------------------------------------
//
// METHOD: QuickSort<>().
//
// PURPOSE: Sort the objects allocated by a BufferAllocator<> into ascending
//          order. The particular implementation of the quicksort algorithm is
//          based on one given in "Algorithms and Data Structures" by Niklaus
//          Wirth (Prentice Hall 1986).
//
// PARAMETERS:
//
//     Input:
//
//         allocator         - reference to the BufferAllocator<> object
//                             containing the objects to be sorted.
//         left              - passes the lower bounding index of the subarray
//                             to be sorted.
//         right             - passes the upper bounding index of the subarray
//                             to be sorted.
//         nStepsPerInterval - passes an integer specifying the number of
//                             "steps" per progress interval.
//         callback          - passes a reference to a ProgressCallback object.
//                             The referenced callback object is periodically
//                             queried to determine whether or not to cancel
//                             processing, and to report progress.
//
//     Output:
//
//         allocator         - the objects are sorted on ascending order on
//                             return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> void BufferSort<ObjType>::QuickSort(
    BufferAllocator<ObjType> &allocator, int left, int right,
    int nStepsPerInterval, ProgressCallback &callback) const
{
    assert(left >= 0);
    assert(right >= 0);

    int i = left;
    int j = right;
    ObjType x = *allocator[(left + right) / 2];

    do {
        while (*allocator[i] < x)
            i++;

        while (x < *allocator[j])
            j--;

        if (i <= j) {
            if (i < j) {
                ObjType  w = *allocator[i];
                *allocator[i] = *allocator[j];
                *allocator[j] = w;
            }

            i++; j--;
        }
    } while (i <= j);

    if (callback.IsCanceled())
        return;

    if (left < j)
        QuickSort(allocator, left, j, nStepsPerInterval, callback);
    else if (left % nStepsPerInterval == 0)
        callback.AdvanceOneSubInterval();

    if (i < right)
        QuickSort(allocator, i, right, nStepsPerInterval, callback);
    else if (right % nStepsPerInterval == 0)
        callback.AdvanceOneSubInterval();

} // end: QuickSort<>()


//------------------------------------------------------------------------------
//
// METHOD: HeapSort<>().
//
// PURPOSE: Sort the objects allocated by a BufferAllocator<> into ascending
//          order. The particular implementation of the heapsort algorithm is
//          based on one given in "Algorithms and Data Structures" by Niklaus
//          Wirth (Prentice Hall 1986).
//
// PARAMETERS:
//
//     Input:
//
//         allocator         - reference to the BufferAllocator<> object
//                             containing the objects to be sorted.
//         nStepsPerInterval - passes an integer specifying the number of
//                             "steps" per progress interval.
//         callback          - passes a reference to a ProgressCallback object.
//                             The referenced callback object is periodically
//                             queried to determine whether or not to cancel
//                             processing, and to report progress.
//
//     Output:
//
//         allocator         - the objects are sorted on ascending order on
//                             return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> void BufferSort<ObjType>::HeapSort(
    BufferAllocator<ObjType> &allocator, int nStepsPerInterval,
    ProgressCallback &callback) const
{
    int nObjects = allocator.GetNObjects();
    int left = nObjects / 2 + 1;
    int right = nObjects;

    int nItems = left + right;
    callback.BeginProgressInterval(GetNProgressSubIntervals(nItems,
        nStepsPerInterval));

    while (left > 1) {
        if (left % nStepsPerInterval == 0)
            callback.AdvanceOneSubInterval();

        left--;
        Sift(allocator, left, right);

        if (callback.IsCanceled())
            return;
    }

    ObjType swapObject;

    while (right > 1) {
        if (right % nStepsPerInterval == 0)
            callback.AdvanceOneSubInterval();

        swapObject = *allocator[0];
        *allocator[0] = *allocator[right-1];
        *allocator[right-1] = swapObject;

        right--;
        Sift(allocator, 1, right);

        if (callback.IsCanceled())
            return ;
    }

    callback.EndProgressInterval();

} // end: HeapSort<>()


//------------------------------------------------------------------------------
//
// METHOD: Sift<>().
//
// PURPOSE: Auxiliary routine used by the heapsort algorithm to sift an object
//          into its proper position in the heap.
//
//------------------------------------------------------------------------------

template <class ObjType> void BufferSort<ObjType>::Sift(
    BufferAllocator<ObjType> &allocator, int left, int right) const
{
    int i = left;
    int j = 2 * left;
    ObjType swapObject = *allocator[left-1];

    if (j < right && *allocator[j-1] < *allocator[j])
        j++;

    while (j <= right && swapObject < *allocator[j-1]) {
        *allocator[i-1] = *allocator[j-1];

        i = j;
        j = 2 * j;

        if (j < right && *allocator[j-1] < *allocator[j])
            j++;
    }

    *allocator[i-1] = swapObject;

} // end: Sift<>()


//------------------------------------------------------------------------------
//
// METHOD: GetNProgressSubIntervals().
//
// PURPOSE: Determine how many subintervals of progress should be used for the
//          specified number of steps. The number subintervals is computed to
//          be ceiling(nSteps / nStepsPerInterval).
//
// PARAMETERS:
//
//     Input:
//
//         nSteps            - passes the number of "steps" to report progress
//                             for.
//         nStepsPerInterval - passes an integer specifying the number of
//                             "steps" per progress interval.
//
//     Output:
//
//         None.
//
// RETURNS: The number of subintervals of progress to use.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> int BufferSort<ObjType>::GetNProgressSubIntervals(
    int nSteps, int nStepsPerInterval) const
{
    if (nSteps % nStepsPerInterval)
        return nSteps / nStepsPerInterval + 1;
    else
        return nSteps / nStepsPerInterval;

} // end: GetNProgressSubIntervals()

/// \endcond

#endif
