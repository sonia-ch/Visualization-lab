#ifndef IVW_FINDEDGES_H
#define IVW_FINDEDGES_H

#include <modules/basegl/baseglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <modules/opengl/inviwoopengl.h>
#include <modules/opengl/processorgl.h>
#include <modules/opengl/glwrap/shader.h>

namespace inviwo {

class IVW_MODULE_BASEGL_API FindEdges : public ProcessorGL {
public:
    FindEdges();
    ~FindEdges();
    
    InviwoProcessorInfo();

    void initialize();
    void deinitialize();

protected:
    virtual void process();

private:
    ImageInport inport_;
    ImageOutport outport_;
    FloatProperty alpha_;

    Shader* shader_;
};

} // namespace

#endif // IVW_FINDEDGES_H