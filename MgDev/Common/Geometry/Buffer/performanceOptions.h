
#ifndef	_PERFORMANCEOPTIONS_H_
#define	_PERFORMANCEOPTIONS_H_

// Dump intermediate data to .ffgf files
#define PERF_DUMPFFGF			false

// -1 for entire polygon, 0 external loop, 1 first ring etc.
#define PERF_PROCESS_ONLY_N_LOOP	-1

////////// FIXES ///////////////////////

// Do sanity check (useless since just throws an exception)
#define PERF_CHECK_BORDER_CROSSING_VERT false

// Trace the buffer on both sides (true) or only in CCW direction (false)
#define PERF_TRACE_BOTH_SIDES		false

// Process the entire geometry (polygons with rings, multi geometries) together
#define PERF_MERGE_ENTIRE_GEOMETRY	false

// Add the start vertex at the end of the loop. This will simply tracing and get rid of the last arc.
#define PERF_ADD_START_POINT_TO_LOOP true

// Avoid repeated transformation by caching the offsets 
#define PERF_USE_CACHED_OFFSET		true

// Redirect to file for now. TODO: dump to file.
#define PERF_SHOW_TRACE_BOUNDARY	false
//#define PERF_SHOW_STATISTICS		1  // some timings
#define PERF_SHOW_SPIKES			false

#define PERF_SKIP_DUPLICATE_INTERSECTIONS  false // Some problems.
#define PERF_CORRECT_SPIKES			false//true

// Horribly slow and apparently defective (out of memory)
#define PERF_RUN_LBL_ALGORITHM		false 

// Might be necessary to increase to hook length at very large offsets 
#define PERF_INCREASE_HOOKLENGTH_BY		1

// The arc step was 15 degrees. Increase the smoothess 5 times, i.e. the arc step = 3 degrees
#define PERF_INCREASE_SMOOTHNESS_BY		5
#endif

