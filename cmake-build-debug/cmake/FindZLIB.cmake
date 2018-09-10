# Package file for ZLIB, part of Inviwo projects
# It defines the following variables
# 
# ZLIB_FOUND
# ZLIB_INCLUDE_DIR
# ZLIB_LIBRARY_DIR
# ZLIB_LIBRARIES
# ZLIB_DEFINITIONS
# ZLIB_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on zlib1")

set(BUILD_ZLIB 1)
set(ZLIB_PROJECT zlib1)
set(ZLIB_FOUND 1)
set(ZLIB_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo")
set(ZLIB_LIBRARY_DIR "/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/lib")
set(ZLIB_LIBRARIES "zlib1$<$<CONFIG:DEBUG>:>")
set(ZLIB_DEFINITIONS )
set(ZLIB_LINK_FLAGS )
