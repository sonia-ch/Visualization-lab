# Package file for OPENEXR-IEX, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-IEX_FOUND
# OPENEXR-IEX_INCLUDE_DIR
# OPENEXR-IEX_LIBRARY_DIR
# OPENEXR-IEX_LIBRARIES
# OPENEXR-IEX_DEFINITIONS
# OPENEXR-IEX_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-iex")

set(BUILD_OPENEXR-IEX 1)
set(OPENEXR-IEX_PROJECT openexr-iex)
set(OPENEXR-IEX_FOUND 1)
set(OPENEXR-IEX_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread")
set(OPENEXR-IEX_LIBRARY_DIR "")
set(OPENEXR-IEX_LIBRARIES "openexr-iex$<$<CONFIG:DEBUG>:>")
set(OPENEXR-IEX_DEFINITIONS )
set(OPENEXR-IEX_LINK_FLAGS )
