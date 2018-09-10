# Package file for TICPP, part of Inviwo projects
# It defines the following variables
# 
# TICPP_FOUND
# TICPP_INCLUDE_DIR
# TICPP_LIBRARY_DIR
# TICPP_LIBRARIES
# TICPP_DEFINITIONS
# TICPP_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on ticpp")

set(BUILD_TICPP 1)
set(TICPP_PROJECT ticpp)
set(TICPP_FOUND 1)
set(TICPP_INCLUDE_DIR "")
set(TICPP_LIBRARY_DIR "")
set(TICPP_LIBRARIES "ticpp$<$<CONFIG:DEBUG>:>")
set(TICPP_DEFINITIONS -DTIXML_USE_TICPP)
set(TICPP_LINK_FLAGS )
