
/* Traian 1/2/2007
   Polygon fill routines taken from the Graphics Programming Black Book by M. Abrash.

   I (heavily) modified the code as follows:
   * add support for polygons with multiple contours,
   * do polygon clipping if parts of the polygon are outside of
   the screen,
   * Reduce number of memory allocations by reusing the same
   buffer for egde lists
   * function no longer returns an int

   Original code description follows below.
*/

/* Color-fills an arbitrarily-shaped polygon described by VertexList.
If the first and last points in VertexList are not the same, the path
around the polygon is automatically closed. All vertices are offset
by (XOffset, YOffset). Returns 1 for success, 0 if memory allocation
failed.
If the polygon shape is known in advance, speedier processing may be
enabled by specifying the shape as follows: "convex" - a rubber band
stretched around the polygon would touch every vertex in order;
"nonconvex" - the polygon is not self-intersecting, but need not be
convex; "complex" - the polygon may be self-intersecting, or, indeed,
any sort of polygon at all. Complex will work for all polygons; convex
is fastest. Undefined results will occur if convex is specified for a
nonconvex or complex polygon.
Define CONVEX_CODE_LINKED if the fast convex polygon filling code from
Chapter 21 is linked in. Otherwise, convex polygons are
handled by the complex polygon filling code.
Nonconvex is handled as complex in this implementation. See text for a
discussion of faster nonconvex handling

Link with L23-4.C and L23-2.C in Small model.
Tested with Borland C++ 4.02 by Jim Mischel 12/16/94.
*/

#include "stdafx.h"
#include "complex_polygon_gd.h"
#include <stdlib.h>

/* Describes a single point (used for a single vertex) */
struct Point {
   int X;   /* X coordinate */
   int Y;   /* Y coordinate */
};

/* Describes the beginning and ending X coordinates of a single
   horizontal line (used only by fast polygon fill code) */
struct HLine {
   int XStart; /* X coordinate of leftmost pixel in line */
   int XEnd;   /* X coordinate of rightmost pixel in line */
};
/* Describes a Length-long series of horizontal lines, all assumed to
   be on contiguous scan lines starting at YStart and proceeding
   downward (used to describe a scan-converted polygon to the
   low-level hardware-dependent drawing code) (used only by fast
   polygon fill code) */
struct HLineList {
   int Length;                /* # of horizontal lines */
   int YStart;                /* Y coordinate of topmost line */
   struct HLine * HLinePtr;   /* pointer to list of horz lines */
};

#define SWAP(a,b) {temp = a; a = b; b = temp;}

struct EdgeState {
    struct EdgeState *NextEdge;
    int X;
    int StartY;
    int WholePixelXMove;
    int XDirection;
    int ErrorTerm;
    int ErrorTermAdjUp;
    int ErrorTermAdjDown;
    int Count;
};

complex_polygon_gd::complex_polygon_gd()
{
    m_bufEdgeState = NULL;
    m_nBufEdgeState = 0;
}
complex_polygon_gd::~complex_polygon_gd()
{
    delete [] m_bufEdgeState;
}


void complex_polygon_gd::FillPolygon(Point * VertexList, int nVerts, int* Contours, int nContours, int Color, gdImagePtr target)
{
    struct EdgeState *EdgeTableBuffer;
    int CurrentY;

    /* It takes a minimum of 3 vertices to cause any pixels to be
    drawn; reject polygons that are guaranteed to be invisible */
    if (nVerts < 3)
        return;

    /* Get enough memory to store the entire edge table */
    /* TIS -- reuse a buffer owned by the class */
    if (m_nBufEdgeState < (size_t)nVerts)
    {
        delete [] m_bufEdgeState;
        m_nBufEdgeState = nVerts;
        m_bufEdgeState = new EdgeState[m_nBufEdgeState];
    }

    EdgeTableBuffer = m_bufEdgeState;

    /* Build the global edge table */
    BuildGET(VertexList, Contours, nContours, EdgeTableBuffer, target->sy - 1);

    if (m_GETPtr) /* true if polygon was not completely clipped out */
    {
        /* Scan down through the polygon edges, one scan line at a time,
        so long as at least one edge remains in either the GET or AET */
        m_AETPtr = NULL;    /* initialize the active edge table to empty */
        CurrentY = m_GETPtr->StartY; /* start at the top polygon vertex */
        while (((m_GETPtr != NULL) || (m_AETPtr != NULL))
            && CurrentY != target->sy)
        {
            MoveXSortedToAET(CurrentY);  /* update AET for this scan line */
            ScanOutAET(CurrentY, Color, target); /* draw this scan line from AET */
            AdvanceAET();                /* advance AET edges 1 scan line */
            XSortAET();                  /* resort on X */
            CurrentY++;                  /* advance to the next scan line */
        }
    }
}

/* Creates a GET in the buffer pointed to by NextFreeEdgeStruc from
the vertex list. Edge endpoints are flipped, if necessary, to
guarantee all edges go top to bottom. The GET is sorted primarily
by ascending Y start coordinate, and secondarily by ascending X
start coordinate within edges with common Y coordinates */
void complex_polygon_gd::BuildGET(struct Point * VertexList,
int* Contours, int nContours, struct EdgeState * NextFreeEdgeStruc, int MaxY)
{
    int i, StartX, StartY, EndX, EndY, DeltaY, DeltaX, Width, temp, SkipY;
    struct EdgeState *NewEdgePtr;
    struct EdgeState *FollowingEdge, **FollowingEdgeLink;
    struct Point *VertexPtr;

    /* Scan through the vertex list and put all non-0-height edges into
    the GET, sorted by increasing Y start coordinate */
    VertexPtr = VertexList;   /* point to the vertex list */
    m_GETPtr = NULL;    /* initialize the global edge table to empty */

    int offset = 0;
    for (int j=0; j<nContours; j++)
    {
        for (i = 0; i < Contours[j]; i++) {
            /* Calculate the edge height and width */
            StartX = VertexPtr[offset + i].X;
            StartY = VertexPtr[offset + i].Y;
            /* The edge runs from the current point to the previous one */
            if (i == 0) {
                /* Wrap back around to the end of the list */
                EndX = VertexPtr[offset + Contours[j]-1].X;
                EndY = VertexPtr[offset + Contours[j]-1].Y;
            } else {
                EndX = VertexPtr[offset + i-1].X;
                EndY = VertexPtr[offset + i-1].Y;
            }
            /* Make sure the edge runs top to bottom */
            if (StartY > EndY) {
                SWAP(StartX, EndX);
                SWAP(StartY, EndY);
            }
            /* Skip if this can't ever be an active edge (has 0 height)
               or if the edge doesn't cross the vertical screen
               bounds at all */

            if ((DeltaY = EndY - StartY) != 0
                && (EndY > 0) && (StartY <= MaxY)) {
                /* Allocate space for this edge's info, and fill in the
                structure */
                NewEdgePtr = NextFreeEdgeStruc++;
                NewEdgePtr->XDirection =   /* direction in which X moves */
                    ((DeltaX = EndX - StartX) > 0) ? 1 : -1;
                Width = abs(DeltaX);
                NewEdgePtr->X = StartX;

                /* clip stuff that's before beginning of screen
                   We can't just change StartY -- we also need to
                   update the initial conditions for the edge
                   rasterizer
                   */

                /* find how many rows we have to skip */
                SkipY = (StartY >=0) ? 0 : -StartY;
                StartY += SkipY;
                NewEdgePtr->StartY = StartY;
                NewEdgePtr->Count = DeltaY - SkipY;
                NewEdgePtr->ErrorTermAdjDown = DeltaY;

                /* set initial rasterization conditions */
                if (DeltaX >= 0)  /* initial error term going L->R */
                    NewEdgePtr->ErrorTerm = 0;
                else              /* initial error term going R->L */
                    NewEdgePtr->ErrorTerm = -DeltaY + 1;
                if (DeltaY >= Width) {     /* Y-major edge */
                    NewEdgePtr->WholePixelXMove = 0;
                    NewEdgePtr->ErrorTermAdjUp = Width;
                } else {                   /* X-major edge */
                    NewEdgePtr->WholePixelXMove =
                        (Width / DeltaY) * NewEdgePtr->XDirection;
                    NewEdgePtr->ErrorTermAdjUp = Width % DeltaY;
                }

                /* update initial conditions only if we have to*/
                if (SkipY != 0) {
                    int XMove = (SkipY + 1) * NewEdgePtr->WholePixelXMove;
                    int ErrorMove = NewEdgePtr->ErrorTerm + (SkipY + 1) * NewEdgePtr->ErrorTermAdjUp;
                    int ErrorDiv = ErrorMove / NewEdgePtr->ErrorTermAdjDown;
                    int ErrorMod = ErrorMove % NewEdgePtr->ErrorTermAdjDown;

                    NewEdgePtr->X += XMove + ErrorDiv * NewEdgePtr->XDirection;
                    StartX = NewEdgePtr->X;
                    NewEdgePtr->ErrorTerm = ErrorMod - NewEdgePtr->ErrorTermAdjDown;
                }

                /* Link the new edge into the GET so that the edge list is
                still sorted by Y coordinate, and by X coordinate for all
                edges with the same Y coordinate */
                FollowingEdgeLink = &m_GETPtr;
                for (;;) {
                    FollowingEdge = *FollowingEdgeLink;
                    if ((FollowingEdge == NULL) ||
                        (FollowingEdge->StartY > StartY) ||
                        ((FollowingEdge->StartY == StartY) &&
                        (FollowingEdge->X >= StartX))) {
                            NewEdgePtr->NextEdge = FollowingEdge;
                            *FollowingEdgeLink = NewEdgePtr;
                            break;
                    }
                    FollowingEdgeLink = &FollowingEdge->NextEdge;
                }
            }
        }

        offset += Contours[j];
    }
}

/* Sorts all edges currently in the active edge table into ascending
order of current X coordinates */
void complex_polygon_gd::XSortAET() {
    struct EdgeState *CurrentEdge, **CurrentEdgePtr, *TempEdge;
    int SwapOccurred;

    /* Scan through the AET and swap any adjacent edges for which the
    second edge is at a lower current X coord than the first edge.
    Repeat until no further swapping is needed */
    if (m_AETPtr != NULL) {
        do {
            SwapOccurred = 0;
            CurrentEdgePtr = &m_AETPtr;
            while ((CurrentEdge = *CurrentEdgePtr)->NextEdge != NULL) {
                if (CurrentEdge->X > CurrentEdge->NextEdge->X) {
                    /* The second edge has a lower X than the first;
                    swap them in the AET */
                    TempEdge = CurrentEdge->NextEdge->NextEdge;
                    *CurrentEdgePtr = CurrentEdge->NextEdge;
                    CurrentEdge->NextEdge->NextEdge = CurrentEdge;
                    CurrentEdge->NextEdge = TempEdge;
                    SwapOccurred = 1;
                }
                CurrentEdgePtr = &(*CurrentEdgePtr)->NextEdge;
            }
        } while (SwapOccurred != 0);
    }
}

/* Advances each edge in the AET by one scan line.
Removes edges that have been fully scanned. */
void complex_polygon_gd::AdvanceAET() {
    struct EdgeState *CurrentEdge, **CurrentEdgePtr;

    /* Count down and remove or advance each edge in the AET */
    CurrentEdgePtr = &m_AETPtr;
    while ((CurrentEdge = *CurrentEdgePtr) != NULL) {
        /* Count off one scan line for this edge */
        if ((--(CurrentEdge->Count)) == 0) {
            /* This edge is finished, so remove it from the AET */
            *CurrentEdgePtr = CurrentEdge->NextEdge;
        } else {
            /* Advance the edge's X coordinate by minimum move */
            CurrentEdge->X += CurrentEdge->WholePixelXMove;
            /* Determine whether it's time for X to advance one extra */
            if ((CurrentEdge->ErrorTerm +=
                CurrentEdge->ErrorTermAdjUp) > 0) {
                    CurrentEdge->X += CurrentEdge->XDirection;
                    CurrentEdge->ErrorTerm -= CurrentEdge->ErrorTermAdjDown;
            }
            CurrentEdgePtr = &CurrentEdge->NextEdge;
        }
    }
}

/* Moves all edges that start at the specified Y coordinate from the
GET to the AET, maintaining the X sorting of the AET. */
void complex_polygon_gd::MoveXSortedToAET(int YToMove) {
    struct EdgeState *AETEdge, **AETEdgePtr, *TempEdge;
    int CurrentX;

    /* The GET is Y sorted. Any edges that start at the desired Y
    coordinate will be first in the GET, so we'll move edges from
    the GET to AET until the first edge left in the GET is no longer
    at the desired Y coordinate. Also, the GET is X sorted within
    each Y coordinate, so each successive edge we add to the AET is
    guaranteed to belong later in the AET than the one just added */
    AETEdgePtr = &m_AETPtr;
    while ((m_GETPtr != NULL) && (m_GETPtr->StartY == YToMove)) {
        CurrentX = m_GETPtr->X;
        /* Link the new edge into the AET so that the AET is still
        sorted by X coordinate */
        for (;;) {
            AETEdge = *AETEdgePtr;
            if ((AETEdge == NULL) || (AETEdge->X >= CurrentX)) {
                TempEdge = m_GETPtr->NextEdge;
                *AETEdgePtr = m_GETPtr;  /* link the edge into the AET */
                m_GETPtr->NextEdge = AETEdge;
                AETEdgePtr = &m_GETPtr->NextEdge;
                m_GETPtr = TempEdge;   /* unlink the edge from the GET */
                break;
            } else {
                AETEdgePtr = &AETEdge->NextEdge;
            }
        }
    }
}

/* Fills the scan line described by the current AET at the specified Y
coordinate in the specified color, using the odd/even fill rule */
void complex_polygon_gd::ScanOutAET(int YToScan, int Color, gdImagePtr target) {
    int LeftX;
    struct EdgeState *CurrentEdge;

    /* Scan through the AET, drawing line segments as each pair of edge
    crossings is encountered. The nearest pixel on or to the right
    of left edges is drawn, and the nearest pixel to the left of but
    not on right edges is drawn */
    CurrentEdge = m_AETPtr;
    while (CurrentEdge != NULL) {
        LeftX = CurrentEdge->X;
        CurrentEdge = CurrentEdge->NextEdge;
        DrawHorizontalLineSeg(YToScan, LeftX, CurrentEdge->X-1, Color, target);
        CurrentEdge = CurrentEdge->NextEdge;
    }
}
