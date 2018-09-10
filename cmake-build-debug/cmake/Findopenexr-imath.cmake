# Package file for OPENEXR-IMATH, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-IMATH_FOUND
# OPENEXR-IMATH_INCLUDE_DIR
# OPENEXR-IMATH_LIBRARY_DIR
# OPENEXR-IMATH_LIBRARIES
# OPENEXR-IMATH_DEFINITIONS
# OPENEXR-IMATH_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-imath")

set(BUILD_OPENEXR-IMATH 1)
set(OPENEXR-IMATH_PROJECT openexr-imath)
set(OPENEXR-IMATH_FOUND 1)
set(OPENEXR-IMATH_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread")
set(OPENEXR-IMATH_LIBRARY_DIR "")
set(OPENEXR-IMATH_LIBRARIES "openexr-iex;openexr-imath$<$<CONFIG:DEBUG>:>")
set(OPENEXR-IMATH_DEFINITIONS )
set(OPENEXR-IMATH_LINK_FLAGS )
