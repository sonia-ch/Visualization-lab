/*********************************************************************
*  Author  : Anke Friederici
*
*  Project : KTH Inviwo Modules
*
*  License : Follows the Inviwo BSD license model
**********************************************************************/

#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <labtopo/integrator.h>
#include <labtopo/interpolator.h>
#include <labtopo/topology.h>
#include <labtopo/utils/gradients.h>

namespace inviwo
{

const vec4 Topology::ColorsCP[6] =
    {
        vec4(1, 1, 0, 1),  // Saddle
        vec4(0, 0, 1, 1),  // AttractingNode
        vec4(1, 0, 0, 1),  // RepellingNode
        vec4(0.5, 0, 1, 1),// AttractingFocus
        vec4(1, 0.5, 0, 1),// RepellingFocus
        vec4(0, 1, 0, 1)   // Center
};

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo Topology::processorInfo_{
    "org.inviwo.Topology",  // Class identifier
    "Vector Field Topology",// Display name
    "KTH Lab",              // Category
    CodeState::Experimental,// Code state
    Tags::None,             // Tags
};

const ProcessorInfo Topology::getProcessorInfo() const
{
    return processorInfo_;
}

Topology::Topology()
    : Processor(), outMesh("meshOut"), inData("inData")
// TODO: Initialize additional properties
// propertyName("propertyIdentifier", "Display Name of the Propery",
// default value (optional), minimum value (optional), maximum value (optional), increment (optional));
// propertyIdentifier cannot have spaces
{
    // Register Ports
    addPort(outMesh);
    addPort(inData);

    // TODO: Register additional properties
    // addProperty(propertyName);
}

bool Topology::checkZero(std::vector<dvec2>& p)
{
	bool is0 = false;
	if (!((p[0].x > 0 && p[1].x > 0 && p[2].x > 0 && p[3].x > 0) || 
		(p[0].x < 0 && p[1].x < 0 && p[2].x < 0 && p[3].x < 0)))
	{
		if (!((p[0].y > 0 && p[1].y > 0 && p[2].y > 0 && p[3].y > 0) || 
			(p[0].y < 0 && p[1].y < 0 && p[2].y < 0 && p[3].y < 0)))
		{
			is0 = true;
		}
	}
	return is0;
}

void Topology::findZero(const Volume* vol, vec2& p00, float splitWidth, std::vector<vec2>& criticalP)
{
	if (splitWidth < 0.005)
	{
		criticalP.push_back(p00);
	}
	else
	{
		// Divide into 4 cells
		vec2 p[4] = { p00,
		{ p00.x + splitWidth, p00.y },
		{ p00.x, p00.y + splitWidth },
		{ p00.x + splitWidth, p00.y + splitWidth } }; //bottom-left corner of each cell
		// Check zeros
		for (int i = 0; i < 4; i++)
		{
			std::vector<vec2> cellPoints = { p[i],{ p[i].x + splitWidth, p[i].y },{ p[i].x, p[i].y + splitWidth },{ p[i].x + splitWidth, p[i].y + splitWidth } };
			// get values
			std::vector<dvec2> cellValues = {
				Interpolator::sampleFromField(vol, cellPoints[0]),
				Interpolator::sampleFromField(vol, cellPoints[1]),
				Interpolator::sampleFromField(vol, cellPoints[2]),
				Interpolator::sampleFromField(vol, cellPoints[3]), };

			if(checkZero(cellValues))
			{
				findZero(vol, cellPoints[0], splitWidth / 2.0f, criticalP);
			}
		}
	}
}

void Topology::process()
{
    // Get input
    if (!inData.hasData())
    {
        return;
    }
    auto vol = inData.getData();

    // Retreive data in a form that we can access it
    const VolumeRAM* vr = vol->getRepresentation<VolumeRAM>();
    uvec3 dims = vr->getDimensions();

    // Initialize mesh, vertices and index buffers for the two streamlines and the
    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;
    // Either add all line segments to this index buffer (one large buffer),
    // or use several index buffers with connectivity type adjacency.
    auto indexBufferSeparatrices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);

    // TODO: Compute the topological skeleton of the input vector field.
    // Find the critical points and color them according to their type.
    // Integrate all separatrices.
    // You can use your previous integration code (copy it over or call it from <lablic/integrator.h>).

	vector<vec2> criticalPoints;

    // Looping through all values in the vector field.
	for (int y = 0; y < dims[1] - 1; ++y)
	{
		for (int x = 0; x < dims[0] - 1; ++x)
		{
			dvec2 f00 = vr->getAsDVec2(uvec3(x, y, 0));
			dvec2 f10 = vr->getAsDVec2(uvec3(x+1, y, 0));
			dvec2 f01 = vr->getAsDVec2(uvec3(x, y+1, 0));
			dvec2 f11 = vr->getAsDVec2(uvec3(x+1, y+1, 0));

			vector<dvec2> values = { f00, f10, f01, f11 };
			
			if (checkZero(values))
			{
				Topology::findZero(vol.get(), vec2(x, y), 0.5f, criticalPoints);
			}
		}
	}

	// Draw points
	for (int i = 0; i < criticalPoints.size(); i++)
	{
		indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
		// A vertex has a position, a normal, a texture coordinate and a color
		// we do not use normal or texture coordinate, but still have to specify them
		vec4 color = vec4(1, 0, 0, 1);
		vertices.push_back({ vec3(criticalPoints[i].x / (dims.x - 1), criticalPoints[i].y / (dims.y - 1), 0), vec3(0), vec3(0), color });

	}
        
    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

}// namespace
