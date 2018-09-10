# Package file for OPENEXR-IEXMATH, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-IEXMATH_FOUND
# OPENEXR-IEXMATH_INCLUDE_DIR
# OPENEXR-IEXMATH_LIBRARY_DIR
# OPENEXR-IEXMATH_LIBRARIES
# OPENEXR-IEXMATH_DEFINITIONS
# OPENEXR-IEXMATH_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-iexmath")

set(BUILD_OPENEXR-IEXMATH 1)
set(OPENEXR-IEXMATH_PROJECT openexr-iexmath)
set(OPENEXR-IEXMATH_FOUND 1)
set(OPENEXR-IEXMATH_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread")
set(OPENEXR-IEXMATH_LIBRARY_DIR "")
set(OPENEXR-IEXMATH_LIBRARIES "openexr-iex;openexr-iexmath$<$<CONFIG:DEBUG>:>")
set(OPENEXR-IEXMATH_DEFINITIONS )
set(OPENEXR-IEXMATH_LINK_FLAGS )
