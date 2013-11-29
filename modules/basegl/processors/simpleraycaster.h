#ifndef IVW_SIMPLERAYCASTER_H
#define IVW_SIMPLERAYCASTER_H

#include <modules/basegl/baseglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/properties/properties.h>
#include <modules/opengl/inviwoopengl.h>
#include <modules/opengl/volume/volumeraycastergl.h>

namespace inviwo {

class IVW_MODULE_BASEGL_API SimpleRaycaster : public VolumeRaycasterGL {
public:
    SimpleRaycaster();
    
    InviwoProcessorInfo();

protected:
    virtual void process();

private:
    VolumeInport volumePort_;
    ImageInport entryPort_;
    ImageInport exitPort_;
    ImageOutport outport_;

    TransferFunctionProperty transferFunction_;
};

} // namespace

#endif // IVW_SIMPLERAYCASTER_H