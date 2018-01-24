project(imake)
include_directories(${ACE_INCLUDE_DIR}
    ${TCLAP_INCLUDE_DIR}
)

set(IMake_SRCS
    IMake.cpp
    SimpleXmlParser.cpp
    stdafx.cpp
)

add_executable(IMake ${IMake_SRCS})
target_link_libraries(IMake ${ACE_LIBRARY})