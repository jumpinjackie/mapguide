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

#ifndef _PERFORMANCEOPTIONS_H_
#define _PERFORMANCEOPTIONS_H_

// FOR DEBUGGING A CERTAIN POLYGON BUFFERING FAILURE, SET THE FOLLOWING:
//----------------------------------------------------------------------
// Dump intermediate data to .ffgf files (the winged edges). Set these flags in the compiler's command line:
// PERF_DUMPFFGF

// Dump intermediate data to .ffgf files (in addition dumps the chained edges and the intersection points)
// PERF_DUMPFFGF_MAX

// -1 for entire polygon, 0 external loop, 1 first ring etc.
#define PERF_PROCESS_ONLY_N_LOOP -1

// Redirect to file for now. TODO: dump to file.
#define PERF_SHOW_TRACE_BOUNDARY false

//---------------------------------------------------------------------

////////// FIXES ///////////////////////

// Do sanity check (useless since just throws an exception)
#define PERF_CHECK_BORDER_CROSSING_VERT false

// Trace the buffer on both sides (true) or only in CCW direction (false)
#define PERF_TRACE_BOTH_SIDES       false

// Process the entire geometry (polygons with rings, multi geometries) together
#define PERF_MERGE_ENTIRE_GEOMETRY  true //false - TODO: fix the algorithm for interior rings

// Add the start vertex at the end of the loop. This will simplify tracing and get rid of the last arc.
#define PERF_ADD_START_POINT_TO_LOOP true

// Avoid repeated transformation by caching the offsets
#define PERF_USE_CACHED_OFFSET      true

// Redirect to file for now. TODO: dump to file.
#define PERF_SHOW_SPIKES            false



#define PERF_SKIP_DUPLICATE_INTERSECTIONS  false // Some problems.
#define PERF_CORRECT_SPIKES         false

#define PERF_CORRECT_SPIKES2        true


// Slow and apparently defective (out of memory).
// But necessary at poles when the hook points get very far from 90 degrees
// and fail to intersect the segments.
#define PERF_RUN_LBL_ALGORITHM_ON_FAILURE true//false

// Might be necessary to increase the hook length at very large offsets
#define PERF_INCREASE_HOOKLENGTH_BY     1

// The arc step was 15 degrees. Increase the smoothess 5 times, i.e. the arc step = 3 degrees
#define PERF_INCREASE_SMOOTHNESS_BY     5

#endif


