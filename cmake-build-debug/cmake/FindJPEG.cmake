# Package file for JPEG, part of Inviwo projects
# It defines the following variables
# 
# JPEG_FOUND
# JPEG_INCLUDE_DIR
# JPEG_LIBRARY_DIR
# JPEG_LIBRARIES
# JPEG_DEFINITIONS
# JPEG_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on libjpeg")

set(BUILD_JPEG 1)
set(JPEG_PROJECT libjpeg)
set(JPEG_FOUND 1)
set(JPEG_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/libjpeg")
set(JPEG_LIBRARY_DIR "")
set(JPEG_LIBRARIES "libjpeg$<$<CONFIG:DEBUG>:>")
set(JPEG_DEFINITIONS -DJPEG_DLL)
set(JPEG_LINK_FLAGS )
