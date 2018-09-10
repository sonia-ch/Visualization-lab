# Package file for SIGAR, part of Inviwo projects
# It defines the following variables
# 
# SIGAR_FOUND
# SIGAR_INCLUDE_DIR
# SIGAR_LIBRARY_DIR
# SIGAR_LIBRARIES
# SIGAR_DEFINITIONS
# SIGAR_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on sigar")

set(BUILD_SIGAR 1)
set(SIGAR_PROJECT sigar)
set(SIGAR_FOUND 1)
set(SIGAR_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/sigar/include;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/src/os/linux")
set(SIGAR_LIBRARY_DIR "")
set(SIGAR_LIBRARIES "sigar$<$<CONFIG:DEBUG>:>")
set(SIGAR_DEFINITIONS -DIVW_SIGAR)
set(SIGAR_LINK_FLAGS )
