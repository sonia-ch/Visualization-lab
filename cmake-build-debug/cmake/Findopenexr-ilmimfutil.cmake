# Package file for OPENEXR-ILMIMFUTIL, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-ILMIMFUTIL_FOUND
# OPENEXR-ILMIMFUTIL_INCLUDE_DIR
# OPENEXR-ILMIMFUTIL_LIBRARY_DIR
# OPENEXR-ILMIMFUTIL_LIBRARIES
# OPENEXR-ILMIMFUTIL_DEFINITIONS
# OPENEXR-ILMIMFUTIL_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-ilmimfutil")

set(BUILD_OPENEXR-ILMIMFUTIL 1)
set(OPENEXR-ILMIMFUTIL_PROJECT openexr-ilmimfutil)
set(OPENEXR-ILMIMFUTIL_FOUND 1)
set(OPENEXR-ILMIMFUTIL_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/openexr-2.2.0/IlmImf;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/openexr-2.2.0/IlmImfUtil;/home/kaikun/IDEs/Tools/inviwo/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/lut")
set(OPENEXR-ILMIMFUTIL_LIBRARY_DIR "")
set(OPENEXR-ILMIMFUTIL_LIBRARIES "openexr-half;openexr-iex;openexr-imath;openexr-ilmthread;openexr-ilmimf;pthread;zlib1$<$<CONFIG:DEBUG>:>;openexr-ilmimfutil$<$<CONFIG:DEBUG>:>")
set(OPENEXR-ILMIMFUTIL_DEFINITIONS )
set(OPENEXR-ILMIMFUTIL_LINK_FLAGS )
