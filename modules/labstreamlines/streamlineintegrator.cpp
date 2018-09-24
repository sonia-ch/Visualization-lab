/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, September 19, 2017 - 15:08:33
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labstreamlines/streamlineintegrator.h>
#include <labstreamlines/integrator.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/interaction/events/mouseevent.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo StreamlineIntegrator::processorInfo_{
    "org.inviwo.StreamlineIntegrator",  // Class identifier
    "Streamline Integrator",            // Display name
    "KTH Lab",                          // Category
    CodeState::Experimental,            // Code state
    Tags::None,                         // Tags
};

const ProcessorInfo StreamlineIntegrator::getProcessorInfo() const { return processorInfo_; }

StreamlineIntegrator::StreamlineIntegrator()
    : Processor()
    , inData("volIn")
    , outMesh("meshOut")
    , propStartPoint("startPoint", "Start Point", vec2(0.5, 0.5), vec2(0), vec2(1024), vec2(0.5))
    , propSeedMode("seedMode", "Seeds")
    // TODO: Initialize additional properties
    , propDirection("direction", "Direction")
    , propStepSize("stepSize", "Step size", 1.0f, 0.0f, 10.0f, 0.1f)
    , propNormalized("normalized", "Normalize to Direction Field")
    , propMaxSteps("maxSteps", "Maximum # of Steps", 5, 0, 1000, 1)
    , propArcLength("arcLength", "Stream line arc length", 0.0f, 0.0f, 1000.0f, 1.0f)
    // propertyName("propertyIdentifier", "Display Name of the Propery",
    // default value (optional), minimum value (optional), maximum value (optional), increment
    // (optional)); propertyIdentifier cannot have spaces
    , mouseMoveStart("mouseMoveStart", "Move Start", [this](Event* e) { eventMoveStart(e); },
                     MouseButton::Left, MouseState::Press | MouseState::Move) {
    // Register Ports
    addPort(inData);
    addPort(outMesh);

    // Register Properties
    propSeedMode.addOption("one", "Single Start Point", 0);
    propSeedMode.addOption("multiple", "Multiple Seeds", 1);
    addProperty(propSeedMode);
    addProperty(propStartPoint);
    addProperty(mouseMoveStart);

    // TODO: Register additional properties
    addProperty(propDirection);
    propDirection.addOption("forward", "Forward", 1);
    propDirection.addOption("backward", "Backward", -1);
    addProperty(propStepSize);
    addProperty(propMaxSteps);
    addProperty(propNormalized);
    addProperty(propArcLength);
    // addProperty(propertyName);

    // You can hide and show properties for a single seed and hide properties for multiple seeds (TODO)
    propSeedMode.onChange([this]() {
        if (propSeedMode.get() == 0) {
            util::show(propStartPoint, mouseMoveStart);
            // util::hide(...)
        } else {
            util::hide(propStartPoint, mouseMoveStart);
            // util::show(...)
        }
    });

}

void StreamlineIntegrator::eventMoveStart(Event* event) {
    // Handle mouse interaction only if we
    // are in the mode with a single point
    if (propSeedMode.get() == 1) return;
    auto mouseEvent = static_cast<MouseEvent*>(event);
    vec2 mousePos = mouseEvent->posNormalized();
    // Denormalize to volume dimensions
    mousePos.x *= dims.x - 1;
    mousePos.y *= dims.y - 1;
    // Update starting point
    propStartPoint.set(mousePos);
    event->markAsUsed();
}

void StreamlineIntegrator::process() {
    // Get input
    if (!inData.hasData()) {
        return;
    }
    auto vol = inData.getData();

    // Retreive data in a form that we can access it
    auto vr = vol->getRepresentation<VolumeRAM>();
    dims = vol->getDimensions();
    // The start point should be inside the volume (set maximum to the upper right corner)
    propStartPoint.setMaxValue(vec2(dims.x - 1, dims.y - 1));

    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;

    if (propSeedMode.get() == 0) {
        auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
        // Draw start point
        vec2 startPoint = propStartPoint.get();
        vertices.push_back({vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                            vec3(0), vec3(0), vec4(0, 0, 0, 1)});
        indexBufferPoints->add(static_cast<std::uint32_t>(0));

        // TODO: Create one stream line from the given start point
        // Runga Kutta 4th Order
        vec2 prevPosition;
        vec2 position = startPoint;
        vec2 changeVec;
        float velocity = 1.0f;

        float arcLength = 0.0f;

        // d.) stop after max steps
        for (int i=0; i < propMaxSteps.get(); i++) {
            // a.) direction, b.) stepsize & c.) normalized direction field
            prevPosition = position;
            position = Integrator::RK4(vr, dims, position, propDirection.get() * propStepSize.get(), propNormalized.get());
            changeVec = position - prevPosition;
            velocity = float(sqrt((changeVec.x*changeVec.x)+(changeVec.y*changeVec.y)));
            arcLength += velocity;

            // e.) after certain arc length
            if (arcLength > propArcLength.get()) {
                break;
            }

            // f.) stop at domain boundary
            if (position.x < -dims.x or position.x > dims.x or
                position.y < -dims.y or position.y > dims.y) {
                break;
            }

            // g.) zero & h.) slow velocity
            if (velocity == 0.0f or velocity < 0.5f){
                break;
            }

            // Add vertex
            indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
            vertices.push_back({ vec3(position.x / (dims.x -1), position.y / (dims.y-1), 0), vec3(0), vec3(0), vec4(0, 0, 1, 1)});
        }
    } else {
        // TODO: Seed multiple stream lines either randomly or using a uniform grid
        // (TODO: Bonus, sample randomly according to magnitude of the vector field)
    }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

}  // namespace inviwo
