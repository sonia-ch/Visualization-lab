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
    , propMultipleType("multipleType", "Type of seeding")
    , propStepSize("stepSize", "Step size", 1.0f, 0.0f, 10.0f, 0.1f)
    , propNormalized("normalized", "Normalize to Direction Field")
    , propMaxSteps("maxSteps", "Maximum # of Steps", 5, 0, 1000, 1)
    , propArcLength("arcLength", "Stream line arc length", 100.0f, 0.0f, 1000.0f, 1.0f)
    , propDoArcLen("doArcLen", "Use arc length", false)
    , propNumberLines("numberLines", "# of stream lines", 1, 1, 100, 1)
    , propGridLinesX("gridLinesX", "# of Grid Points X-Axis", 3, 1, 50, 1)
    , propGridLinesY("gridLinesY", "# of Grid Points Y-Axis", 3, 1, 50, 1)
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
    addProperty(propMultipleType);
    propMultipleType.addOption("random", "Random Seeding", 0);
    propMultipleType.addOption("uniform", "Uniform Grid", 1);
    propMultipleType.addOption("distribution", "Based on Magnitude Distribution", 2);
    addProperty(propGridLinesX);
    addProperty(propGridLinesY);

    addProperty(propStepSize);
    addProperty(propMaxSteps);
    addProperty(propNormalized);
    addProperty(propArcLength);
    addProperty(propNumberLines);


    // addProperty(propertyName);


    propDoArcLen.onChange([this]() {
        if (propDoArcLen.get() == true){
            util::show(propArcLength);
        } else {
            util::hide(propArcLength);
        }
    });

    propMultipleType.onChange([this]() {
        if (propMultipleType.get() == 1){
            util::show(propGridLinesX, propGridLinesY);
        } else {
            util::hide(propGridLinesX, propGridLinesY);
        }
    });



    // You can hide and show properties for a single seed and hide properties for multiple seeds (TODO)
    propSeedMode.onChange([this]() {
        if (propSeedMode.get() == 0) {
            util::show(propStartPoint, mouseMoveStart);
            util::hide(propMultipleType, propGridLinesX, propGridLinesY);
            // util::hide(...)
        } else {
            util::show(propMultipleType, propGridLinesX, propGridLinesY);
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

void StreamlineIntegrator::drawStreamLine(const VolumeRAM* vr,
                                          std::vector<BasicMesh::Vertex>& vertices,
                                          const size3_t dims,
                                          IndexBufferRAM* indexBufferPoints,
                                          IndexBufferRAM* indexBufferRK,
                                          vec2 startPoint) {
    // Draw start point
    vertices.push_back({vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                        vec3(0), vec3(0), vec4(0, 0, 0, 1)});

    indexBufferPoints->add(static_cast<std::uint32_t>(0));
    indexBufferRK->add(static_cast<std::uint32_t>(0));


    // TODO: Create one stream line from the given start point
    // Initialize variables
    vec2 prevPosition;
    vec2 position = startPoint;
    if (propNormalized.get()){
        position = position / float(sqrt((position.x*position.x)+(position.y*position.y)));
    }
    vec2 changeVec;
    float velocity = 1.0f;
    float arcLength = 0.0f;

    // Runga Kutta 4th Order
    // d.) stop after max steps
    for (int i=0; i < propMaxSteps.get(); i++) {
        // a.) direction, b.) stepsize & c.) normalized direction field
        prevPosition = position;
        position = Integrator::RK4(vr, dims, position, propDirection.get() * propStepSize.get(), propNormalized.get());
        changeVec = position - prevPosition;
        velocity = float(sqrt((changeVec.x*changeVec.x)+(changeVec.y*changeVec.y)));
        arcLength += velocity;

        // e.) after certain arc length
        if (propDoArcLen.get() && arcLength > propArcLength.get()) {
            LogProcessorInfo("Maximal arc length " << arcLength);
            break;
        }

        // f.) stop at domain boundary
        if (position.x > (dims.x-1) || position.x < 0 || position.y > (dims.y-1) || position.y < 0) {
            LogProcessorInfo("Out of domain boundaries (" << position.x << "," << position.y << ")");
            break;
        }

        // g.) zero & h.) slow velocity
        if (velocity == 0.0f || velocity < 0.001f){
            LogProcessorInfo("Velocity is to slow " << velocity);
            break;
        }

        // Add vertex
        indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
        indexBufferRK->add(static_cast<std::uint32_t>(vertices.size()));
        vertices.push_back({ vec3(position.x / (dims.x -1), position.y / (dims.y-1), 0), vec3(0), vec3(0), vec4(0, 0, 1, 1)});
    }

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
        auto indexBufferRK = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
        vec2 startPoint = propStartPoint.get();

        drawStreamLine(vr, vertices, dims, indexBufferPoints, indexBufferRK, startPoint);

    } else {
        // TODO: Seed multiple stream lines either randomly or using a uniform grid
        // (TODO: Bonus, sample randomly according to magnitude of the vector field)
        int n = propNumberLines.get();

        // random seeding
        if (propMultipleType.get() == 0){
            for (int i=0; i<n; i++){
                // dimensions give range -> [0:(dims-1)]
                size_t randX = std::rand() % dims.x;
                size_t randY = std::rand() % dims.y;
                vec2 startPoint = vec2(randX, randY);
                LogProcessorInfo("Seed point for line " << (i+1) << " is " << startPoint);

                auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
                auto indexBufferRK = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
                drawStreamLine(vr, vertices, dims, indexBufferPoints, indexBufferRK, startPoint);
            }
        // uniform grid seeding
        } else if (propMultipleType.get() == 1){
            for (int x=0; x < propGridLinesX.get(); x++){
                for (int y=0; y < propGridLinesY.get(); y++){
                    double coordX = x * (dims.x/(1.0*propGridLinesX.get()));
                    double coordY = y * (dims.y/(1.0*propGridLinesY.get()));
                    vec2 startPoint = vec2(coordX, coordY);
                    LogProcessorInfo("Seed point is " << startPoint);

                    auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
                    auto indexBufferRK = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
                    drawStreamLine(vr, vertices, dims, indexBufferPoints, indexBufferRK, startPoint);
                }
            }
        // distribution seeding
        } else if (propMultipleType.get() == 2) {

        }

    }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

}  // namespace inviwo
