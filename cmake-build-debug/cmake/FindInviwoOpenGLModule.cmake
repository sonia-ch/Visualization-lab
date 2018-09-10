# Package file for INVIWOOPENGLMODULE, part of Inviwo projects
# It defines the following variables
# 
# INVIWOOPENGLMODULE_FOUND
# INVIWOOPENGLMODULE_INCLUDE_DIR
# INVIWOOPENGLMODULE_LIBRARY_DIR
# INVIWOOPENGLMODULE_LIBRARIES
# INVIWOOPENGLMODULE_DEFINITIONS
# INVIWOOPENGLMODULE_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on inviwo-module-opengl")

set(BUILD_INVIWOOPENGLMODULE 1)
set(INVIWOOPENGLMODULE_PROJECT inviwo-module-opengl)
set(INVIWOOPENGLMODULE_FOUND 1)
set(INVIWOOPENGLMODULE_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/include;/home/kaikun/IDEs/Tools/inviwo/ext;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/modules;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/include;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/src/os/linux;/home/kaikun/IDEs/Tools/inviwo/modules/opengl/ext/glew/include;/usr/include")
set(INVIWOOPENGLMODULE_LIBRARY_DIR "/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/lib")
set(INVIWOOPENGLMODULE_LIBRARIES "ticpp$<$<CONFIG:DEBUG>:>;sigar$<$<CONFIG:DEBUG>:>;inviwo-core$<$<CONFIG:DEBUG>:>;GLEW$<$<CONFIG:DEBUG>:>;/usr/lib/x86_64-linux-gnu/libGL.so;/usr/lib/x86_64-linux-gnu/libGLU.so;inviwo-module-opengl$<$<CONFIG:DEBUG>:>")
set(INVIWOOPENGLMODULE_DEFINITIONS -DTIXML_USE_TICPP;-DIVW_SIGAR)
set(INVIWOOPENGLMODULE_LINK_FLAGS )
