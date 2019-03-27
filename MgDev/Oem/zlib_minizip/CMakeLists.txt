project(minizip)

include_directories(
    ../gd/zlib
    ../gd/zlib/contrib/minizip
)

set(minizip_SRCS
    ../gd/zlib/contrib/minizip/zip.c
    ../gd/zlib/contrib/minizip/unzip.c
    ../gd/zlib/contrib/minizip/ioapi.c
)

add_library(minizip STATIC ${minizip_SRCS})