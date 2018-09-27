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
	, propStepSize("stepSize", "Step Size", 0.1f, 0.05f, 1.0f, 0.05f)
	, propNumSteps("numStep", "Number of integration steps", 30, 1, 1000)
	, propNumSeeds("numSeeds", "Number seeds", 10, 1, 100)
	, propNormalized("normalized", "Use Direction Field")
	, propMultipleType("multipleType", "Type of seeding")
	, propGridLinesX("gridLinesX", "# of Grid Points X-Axis", 3, 2, 50, 1)
	, propGridLinesY("gridLinesY", "# of Grid Points Y-Axis", 3, 2, 50, 1)
	, propArcLength("arcLength", "Stream line arc length", 10.0f, 0.0f, 10.0f, 0.1f)
	, propDoArcLen("doArcLen", "Use arc length", false)
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

	addProperty(propNumSeeds);
	addProperty(propMultipleType);
	propMultipleType.addOption("random", "Random Seeding", 0);
	propMultipleType.addOption("uniform", "Uniform Grid", 1);
	propMultipleType.addOption("distribution", "Based on Magnitude Distribution", 2);
	addProperty(propGridLinesX);
	addProperty(propGridLinesY);
	addProperty(propArcLength);
	addProperty(propDoArcLen);
	addProperty(propNormalized);

	util::hide(propArcLength, propGridLinesX, propGridLinesY, propNumSeeds, propMultipleType);


    // You can hide and show properties for a single seed and hide properties for multiple seeds (TODO)
    propSeedMode.onChange([this]() {
        if (propSeedMode.get() == 0) {
            util::show(propStartPoint, mouseMoveStart);
			util::hide(propMultipleType, propGridLinesX, propGridLinesY, propNumSeeds);
            // util::hide(...)
        } else {
            util::hide(propStartPoint, mouseMoveStart);
			util::show(propMultipleType, propGridLinesX, propGridLinesY, propNumSeeds);
            // util::show(...)
        }
    });

	propDoArcLen.onChange([this]() {
		if (propDoArcLen.get() == true) {
			util::show(propArcLength);
		}
		else {
			util::hide(propArcLength);
		}
	});

	propMultipleType.onChange([this]() {
		if (propMultipleType.get() == 1) {
			util::show(propGridLinesX, propGridLinesY);
		}
		else {
			util::hide(propGridLinesX, propGridLinesY);
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

	// Extract the minimum and maximum value from the input data
	const double minValue = vol->dataMap_.valueRange[0];
	const double maxValue = vol->dataMap_.valueRange[1];

    // Retreive data in a form that we can access it
    auto vr = vol->getRepresentation<VolumeRAM>();
    dims = vol->getDimensions();
    // The start point should be inside the volume (set maximum to the upper right corner)
    propStartPoint.setMaxValue(vec2(dims.x - 1, dims.y - 1));

    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;

    if (propSeedMode.get() == 0) {
        auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
		auto indexBufferStreamline = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);

        // Draw start point
        vec2 startPoint = propStartPoint.get();
        vertices.push_back({vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                            vec3(0), vec3(0), vec4(0, 0, 0, 1)});
        indexBufferPoints->add(static_cast<std::uint32_t>(0));
		indexBufferStreamline->add(static_cast<std::uint32_t>(0));

        // TODO: Create one stream line from the given start point
		singleStreamline(vr, dims, startPoint, propStepSize.get(), propNumSteps.get(), indexBufferStreamline, indexBufferPoints, vertices);
    } else {
        // TODO: Seed multiple stream lines either randomly or using a uniform grid
        // (TODO: Bonus, sample randomly according to magnitude of the vector field)

		if (propMultipleType.get() == 0) //Random distribution
		{
			srand(1); //For keeping the same random seeds while changing parameters in the interface

			for (int i = 0; i < propNumSeeds.get(); i++)
			{
				// Draw start point
				vec2 startPoint = vec2((float)rand() / (RAND_MAX / dims.x), (float)rand() / (RAND_MAX / dims.y));
				auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
				auto indexBufferStreamline = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);

				singleStreamline(vr, dims, startPoint, propStepSize.get(), propNumSteps.get(), indexBufferStreamline, indexBufferPoints, vertices);
			}
		}
		else if (propMultipleType.get() == 1) //Grid distribution
		{
			for (int x = 0; x < propGridLinesX.get(); x++) {
				for (int y = 0; y < propGridLinesY.get(); y++) {
					double coordX = x * ((dims.x - 1) / (1.0*propGridLinesX.get()-1));
					double coordY = y * ((dims.y - 1) / (1.0*propGridLinesY.get()-1));
					vec2 startPoint = vec2(coordX, coordY);

					auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
					auto indexBufferStreamline = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
			
					singleStreamline(vr, dims, startPoint, propStepSize.get(), propNumSteps.get(), indexBufferStreamline, indexBufferPoints, vertices);
				}
			}
		}
		else if (propMultipleType.get() == 2) //Magnitude-proporcional distribution
		{
			int gridPoints = 10;
			std::vector<float> values;
			float sum = 0.0f;
			srand(2); //For keeping the same random seeds while changing parameters in the interface

			//Store values
			for (int y = 0; y < gridPoints; y++) {
				for (int x = 0; x < gridPoints; x++) {
					double coordX = x * ((dims.x - 1) / (1.0*gridPoints-1));
					double coordY = y * ((dims.y - 1) / (1.0*gridPoints-1));
					vec2 samplePoint = vec2(coordX, coordY);
					vec2 vecValue = (Integrator::sampleFromField(vr, dims, samplePoint));
					float value = (float)sqrt(vecValue.x*vecValue.x + vecValue.y*vecValue.y);
					sum += value;
					values.push_back(value);
				}
			}

			for (int n = 0; n < propNumSeeds.get(); n++)
			{
				//Generate random number
				float randValue = ((float)rand() / RAND_MAX);

				float cum = 0.0f;
				for (int i = 0; i < values.size(); i++)
				{
					// Find position
					cum += values[i]/sum;

					if (randValue < cum)
					{
						//Draw in that position
						double coordX = i % gridPoints * ((dims.x - 1) / (1.0*gridPoints));
						double coordY = (int)i / gridPoints * ((dims.y - 1) / (1.0*gridPoints));
						vec2 startPoint = vec2(coordX, coordY);

						auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
						auto indexBufferStreamline = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);

						singleStreamline(vr, dims, startPoint, propStepSize.get(), propNumSteps.get(), indexBufferStreamline, indexBufferPoints, vertices);

						break;
					}
				}
			}
		}
    }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}
void StreamlineIntegrator::singleStreamline(const VolumeRAM* vr, size3_t dims, const vec2& startPosition, float stepSize, int numSteps, 
	IndexBufferRAM* indexBuffer, IndexBufferRAM* indexBufferPoints, std::vector<BasicMesh::Vertex>& vertices)
{
	// Draw first point
	indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
	indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
	vertices.push_back({ vec3(startPosition.x / (dims.x - 1), startPosition.y / (dims.y - 1), 0),
		vec3(0), vec3(0), vec4(0, 0, 0, 1) });

	vec2 nextPointRK = startPosition;
	vec2 prevPoint, pointDiff;
	float velocity, arcLength = 0.0f;

	for (int i = 0; i < numSteps; i++)
	{
		vec2 prevPoint = nextPointRK;
		// RK integration
		nextPointRK = Integrator::RK4(vr, dims, nextPointRK, stepSize, propDirection.get(), propNormalized.get());

		pointDiff = nextPointRK - prevPoint;
		velocity = float(sqrt((pointDiff.x*pointDiff.x) + (pointDiff.y*pointDiff.y)));
		arcLength += velocity;

		// e.) after certain arc length
		if (propDoArcLen.get() && arcLength > propArcLength.get()) {
			LogProcessorInfo("Maximal arc length " << arcLength);
			break;
		}

		// f.) stop at domain boundary
		if (nextPointRK.x >(dims.x - 1) || nextPointRK.x < 0 || nextPointRK.y >(dims.y - 1) || nextPointRK.y < 0) {
			LogProcessorInfo("Out of domain boundaries (" << nextPointRK.x << "," << nextPointRK.y << ")");
			break;
		}

		// g.) zero & h.) slow velocity
		if (velocity == 0.0f || velocity < 0.001f) {
			LogProcessorInfo("Velocity is to slow " << velocity);
			break;
		}

		// Draw next point
		indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
		indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
		vertices.push_back({ vec3(nextPointRK.x / (dims.x - 1), nextPointRK.y / (dims.y - 1), 0),
			vec3(0), vec3(0), vec4(0, 0, 1, 1) });
	}
 }


}  // namespace inviwo
