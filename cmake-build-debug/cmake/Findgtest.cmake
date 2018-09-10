# Package file for GTEST, part of Inviwo projects
# It defines the following variables
# 
# GTEST_FOUND
# GTEST_INCLUDE_DIR
# GTEST_LIBRARY_DIR
# GTEST_LIBRARIES
# GTEST_DEFINITIONS
# GTEST_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on gtest")

set(BUILD_GTEST 1)
set(GTEST_PROJECT gtest)
set(GTEST_FOUND 1)
set(GTEST_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/ext/gtest/include;/home/kaikun/IDEs/Tools/inviwo/ext/gtest")
set(GTEST_LIBRARY_DIR "")
set(GTEST_LIBRARIES "gtest$<$<CONFIG:DEBUG>:>")
set(GTEST_DEFINITIONS -DGTEST_HAS_TR1_TUPLE=0;-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
set(GTEST_LINK_FLAGS )
