# Package file for PNG, part of Inviwo projects
# It defines the following variables
# 
# PNG_FOUND
# PNG_INCLUDE_DIR
# PNG_LIBRARY_DIR
# PNG_LIBRARIES
# PNG_DEFINITIONS
# PNG_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on libpng")

set(BUILD_PNG 1)
set(PNG_PROJECT libpng)
set(PNG_FOUND 1)
set(PNG_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/ext/zlib-1.2.11;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/ext/libpng")
set(PNG_LIBRARY_DIR "")
set(PNG_LIBRARIES "libpng$<$<CONFIG:DEBUG>:>")
set(PNG_DEFINITIONS )
set(PNG_LINK_FLAGS )
