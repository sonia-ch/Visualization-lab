#ifndef OPENGL_SHADER_RESOURCE
#define OPENGL_SHADER_RESOURCE

#include <modules/opengl/shader/shadermanager.h>



namespace inviwo {

namespace opengl {

inline void addGeneratedShaderResources(ShaderManager* manager) {

}

inline void addShaderResources(ShaderManager* manager, std::vector<std::string> includePaths) {
#ifdef OPENGL_INCLUDE_SHADER_RESOURCES
    addGeneratedShaderResources(manager);
#else
    for (auto& includePath : includePaths) {
        manager->addShaderSearchPath(includePath);
    }
#endif
}

}  // namespace

}  // namespace

#endif
