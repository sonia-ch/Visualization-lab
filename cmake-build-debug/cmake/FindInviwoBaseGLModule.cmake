# Package file for INVIWOBASEGLMODULE, part of Inviwo projects
# It defines the following variables
# 
# INVIWOBASEGLMODULE_FOUND
# INVIWOBASEGLMODULE_INCLUDE_DIR
# INVIWOBASEGLMODULE_LIBRARY_DIR
# INVIWOBASEGLMODULE_LIBRARIES
# INVIWOBASEGLMODULE_DEFINITIONS
# INVIWOBASEGLMODULE_LINK_FLAGS
# 

get_filename_component(PARENT_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
ivw_debug_message(STATUS "${PARENT_NAME} added dependency on inviwo-module-basegl")

set(BUILD_INVIWOBASEGLMODULE 1)
set(INVIWOBASEGLMODULE_PROJECT inviwo-module-basegl)
set(INVIWOBASEGLMODULE_FOUND 1)
set(INVIWOBASEGLMODULE_INCLUDE_DIR "/home/kaikun/IDEs/Tools/inviwo/include;/home/kaikun/IDEs/Tools/inviwo/ext;/home/kaikun/IDEs/Tools/inviwo;/home/kaikun/IDEs/Tools/inviwo/modules;/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/modules/_generated;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/include;/home/kaikun/IDEs/Tools/inviwo/ext/sigar/src/os/linux;/home/kaikun/IDEs/Tools/inviwo/modules/opengl/ext/glew/include;/usr/include")
set(INVIWOBASEGLMODULE_LIBRARY_DIR "/home/kaikun/IDEs/Tools/inviwo/cmake-build-debug/lib")
set(INVIWOBASEGLMODULE_LIBRARIES "ticpp$<$<CONFIG:DEBUG>:>;sigar$<$<CONFIG:DEBUG>:>;inviwo-core$<$<CONFIG:DEBUG>:>;GLEW$<$<CONFIG:DEBUG>:>;/usr/lib/x86_64-linux-gnu/libGL.so;/usr/lib/x86_64-linux-gnu/libGLU.so;inviwo-module-opengl$<$<CONFIG:DEBUG>:>;inviwo-module-base$<$<CONFIG:DEBUG>:>;inviwo-module-basegl$<$<CONFIG:DEBUG>:>")
set(INVIWOBASEGLMODULE_DEFINITIONS -DTIXML_USE_TICPP;-DIVW_SIGAR)
set(INVIWOBASEGLMODULE_LINK_FLAGS )
