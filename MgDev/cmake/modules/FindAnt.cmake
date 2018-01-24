# - Find Ant
# Find the Apache Ant binary
# This module defines
#  ANT_PROGRAM, the ant executable

find_program(ANT_PROGRAM NAMES ant)

if (ANT_PROGRAM)
    message(STATUS "Found ant: ${ANT_PROGRAM}")
else (ANT_PROGRAM)
    message(FATAL_ERROR "Ant not found")
endif (ANT_PROGRAM)

mark_as_advanced(ANT_PROGRAM)
