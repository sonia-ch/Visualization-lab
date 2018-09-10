# Package file for OPENEXR-ILMTHREAD, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-ILMTHREAD_FOUND
# OPENEXR-ILMTHREAD_INCLUDE_DIR
# OPENEXR-ILMTHREAD_LIBRARY_DIR
# OPENEXR-ILMTHREAD_LIBRARIES
# OPENEXR-ILMTHREAD_DEFINITIONS
# OPENEXR-ILMTHREAD_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-ilmthread")

set(BUILD_OPENEXR-ILMTHREAD 1)
set(OPENEXR-ILMTHREAD_PROJECT openexr-ilmthread)
set(OPENEXR-ILMTHREAD_FOUND 1)
set(OPENEXR-ILMTHREAD_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread")
set(OPENEXR-ILMTHREAD_LIBRARY_DIR "")
set(OPENEXR-ILMTHREAD_LIBRARIES "openexr-iex;openexr-ilmthread$<$<CONFIG:DEBUG>:>")
set(OPENEXR-ILMTHREAD_DEFINITIONS )
set(OPENEXR-ILMTHREAD_LINK_FLAGS )
