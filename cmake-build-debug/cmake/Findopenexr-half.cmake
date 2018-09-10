# Package file for OPENEXR-HALF, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-HALF_FOUND
# OPENEXR-HALF_INCLUDE_DIR
# OPENEXR-HALF_LIBRARY_DIR
# OPENEXR-HALF_LIBRARIES
# OPENEXR-HALF_DEFINITIONS
# OPENEXR-HALF_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-half")

set(BUILD_OPENEXR-HALF 1)
set(OPENEXR-HALF_PROJECT openexr-half)
set(OPENEXR-HALF_FOUND 1)
set(OPENEXR-HALF_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/lut")
set(OPENEXR-HALF_LIBRARY_DIR "")
set(OPENEXR-HALF_LIBRARIES "openexr-half$<$<CONFIG:DEBUG>:>")
set(OPENEXR-HALF_DEFINITIONS )
set(OPENEXR-HALF_LINK_FLAGS )
