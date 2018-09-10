# Package file for INVIWOCORE, part of Inviwo projects
# It defines the following variables
# 
# INVIWOCORE_FOUND
# INVIWOCORE_INCLUDE_DIR
# INVIWOCORE_LIBRARY_DIR
# INVIWOCORE_LIBRARIES
# INVIWOCORE_DEFINITIONS
# INVIWOCORE_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on inviwo-core")

set(BUILD_INVIWOCORE 1)
set(INVIWOCORE_PROJECT inviwo-core)
set(INVIWOCORE_FOUND 1)
set(INVIWOCORE_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/include;/home/kaikun/IDEs/Tools/inviwo/ext;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/modules;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/include;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/src/os/linux")
set(INVIWOCORE_LIBRARY_DIR "")
set(INVIWOCORE_LIBRARIES "ticpp$<$<CONFIG:DEBUG>:>;sigar$<$<CONFIG:DEBUG>:>;inviwo-core$<$<CONFIG:DEBUG>:>")
set(INVIWOCORE_DEFINITIONS -DTIXML_USE_TICPP;-DIVW_SIGAR)
set(INVIWOCORE_LINK_FLAGS )
