# Package file for OPENEXR-ILMIMF, part of Inviwo projects
# It defines the following variables
# 
# OPENEXR-ILMIMF_FOUND
# OPENEXR-ILMIMF_INCLUDE_DIR
# OPENEXR-ILMIMF_LIBRARY_DIR
# OPENEXR-ILMIMF_LIBRARIES
# OPENEXR-ILMIMF_DEFINITIONS
# OPENEXR-ILMIMF_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on openexr-ilmimf")

set(BUILD_OPENEXR-ILMIMF 1)
set(OPENEXR-ILMIMF_PROJECT openexr-ilmimf)
set(OPENEXR-ILMIMF_FOUND 1)
set(OPENEXR-ILMIMF_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated/ext/openexr;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/openexr-2.2.0/IlmImf;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/openexr-2.2.0/IlmImfUtil;/home/kaikun/IDEs/Tools/inviwo/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Iex;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IexMath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Imath;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/Half;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/ilmbase-2.2.0/IlmThread;/home/kaikun/IDEs/Tools/inviwo/ext/openexr/lut")
set(OPENEXR-ILMIMF_LIBRARY_DIR "")
set(OPENEXR-ILMIMF_LIBRARIES "openexr-half;openexr-iex;openexr-imath;openexr-ilmthread;pthread;zlib1$<$<CONFIG:DEBUG>:>;openexr-ilmimf$<$<CONFIG:DEBUG>:>")
set(OPENEXR-ILMIMF_DEFINITIONS )
set(OPENEXR-ILMIMF_LINK_FLAGS )