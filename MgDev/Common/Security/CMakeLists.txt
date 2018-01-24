set (MgSecurity_SRCS
    SecurityBuild.cpp
)

add_library(MgSecurity${MG_VERSION_SUFFIX} STATIC ${MgSecurity_SRCS})