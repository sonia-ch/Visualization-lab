# Package file for INVIWOBASEMODULE, part of Inviwo projects
# It defines the following variables
# 
# INVIWOBASEMODULE_FOUND
# INVIWOBASEMODULE_INCLUDE_DIR
# INVIWOBASEMODULE_LIBRARY_DIR
# INVIWOBASEMODULE_LIBRARIES
# INVIWOBASEMODULE_DEFINITIONS
# INVIWOBASEMODULE_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on inviwo-module-base")

set(BUILD_INVIWOBASEMODULE 1)
set(INVIWOBASEMODULE_PROJECT inviwo-module-base)
set(INVIWOBASEMODULE_FOUND 1)
set(INVIWOBASEMODULE_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/include;/home/kaikun/IDEs/Tools/inviwo/ext;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/modules;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/include;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/src/os/linux")
set(INVIWOBASEMODULE_LIBRARY_DIR "")
set(INVIWOBASEMODULE_LIBRARIES "ticpp$<$<CONFIG:DEBUG>:>;sigar$<$<CONFIG:DEBUG>:>;inviwo-core$<$<CONFIG:DEBUG>:>;inviwo-module-base$<$<CONFIG:DEBUG>:>")
set(INVIWOBASEMODULE_DEFINITIONS -DTIXML_USE_TICPP;-DIVW_SIGAR)
set(INVIWOBASEMODULE_LINK_FLAGS )
