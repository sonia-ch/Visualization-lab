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
    // propertyName("propertyIdentifier", "Display Name of the Propery",
    // default value (optional), minimum value (optional), maximum value (optional), increment
    // (optional)); propertyIdentifier cannot have spaces
	, propDirection("direction", "Direction")
	, propStepSize("stepSize", "Step Size", 0.1f, 0.1f, 4.0f, 0.1f)
	, propNumSteps("numStep", "Number of integration steps", 30, 1, 1000)
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
    // addProperty(propertyName);
	addProperty(propDirection);
	propDirection.addOption("forward", "Forward", 1);
	propDirection.addOption("backward", "Backward", -1);
	addProperty(propStepSize);
	addProperty(propNumSteps);



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
		auto indexBufferStreamline = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
		singleStreamline(vr, dims, startPoint, propStepSize.get(), propNumSteps.get(), indexBufferStreamline, indexBufferPoints, vertices);
    } else {
        // TODO: Seed multiple stream lines either randomly or using a uniform grid
        // (TODO: Bonus, sample randomly according to magnitude of the vector field)
    }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}
void StreamlineIntegrator::singleStreamline(const VolumeRAM* vr, size3_t dims, const vec2& startPosition, float stepSize, int numSteps, 
	IndexBufferRAM* indexBuffer, IndexBufferRAM* indexBufferPoints, std::vector<BasicMesh::Vertex>& vertices)
{
	vec2 nextPointRK = startPosition;

	for (int i = 0; i < numSteps; i++)
	{
		// RK integration
		nextPointRK = singleStepIntegrator(vr, dims, nextPointRK, stepSize, propDirection.get());

		//indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
		indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
		indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
		vertices.push_back({ vec3(nextPointRK.x / (dims.x - 1), nextPointRK.y / (dims.y - 1), 0),
			vec3(0), vec3(0), vec4(0, 0, 1, 1) });
	}
 }

vec2 StreamlineIntegrator::singleStepIntegrator(const VolumeRAM* vr, size3_t dims, const vec2& position, float stepSize, int direction)
{
	vec2 v1 = (float)direction * Integrator::sampleFromField(vr, dims, position);
	vec2 v2 = (float) direction * Integrator::sampleFromField(vr, dims, position + stepSize / 2 * v1);
	vec2 v3 = (float) direction * Integrator::sampleFromField(vr, dims, position + stepSize / 2 * v2);
	vec2 v4 = (float) direction * Integrator::sampleFromField(vr, dims, position + stepSize * v3);

	vec2 point = position + stepSize * (v1 / 6.0f + v2 / 3.0f + v3 / 3.0f + v4 / 6.0f);
	return point;

}


}  // namespace inviwo
