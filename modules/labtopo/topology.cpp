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
, propSeparaticesSeedDistance("separaticesSeedDistance", "Distance of separatices seed to saddle point", 0.1, 0.00001, 1, 0.0001)

{
    // Register Ports
    addPort(outMesh);
    addPort(inData);

    // TODO: Register additional properties
    // addProperty(propertyName);
	addProperty(propSeparaticesSeedDistance);

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
    auto indexBufferSeparatrices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
    auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);

    // TODO: Compute the topological skeleton of the input vector field.
    // Find the critical points and color them according to their type.
    // Integrate all separatrices.
    // You can use your previous integration code (copy it over or call it from <lablic/integrator.h>).

	// (1) Find critical points
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
			
			// Check if there is a possible zero
			if (checkZero(values))
			{
				// Find the critical points
				Topology::findZero(vol.get(), vec2(x, y), 0.5f, criticalPoints);
			}
		}
	}

	// (2) Classify the points to color them
	vector<vec2> saddlePoints;
	for (int i = 0; i < criticalPoints.size(); i++) {
		const vec2& criticalPoint = criticalPoints[i];
		mat2 jacobian = Interpolator::sampleJacobian(vol.get(), criticalPoint);
		// determinant: ad - bc is not zero  => First order 2D critical Point
		if ((jacobian[0][1] * jacobian[1][0] - jacobian[0][0] * jacobian[1][1]) != 0) {
			util::EigenResult eigenResult = util::eigenAnalysis(jacobian);

			TypeCP type = TypeCP::Saddle; // TODO: Don't initialize randomly

										  // Imaginary zero
										  //      Saddle [hyperbolic sector] = All tangent curves to by critical point, except 2 - one originates , other ends
										  //      Repelling or Attracting node [parabolic sector] = all tangent end or origin from  critical point
			if (eigenResult.eigenvaluesIm[0] == 0 && eigenResult.eigenvaluesIm[1] == 0) {

				if ((eigenResult.eigenvaluesRe[0] < 0 && eigenResult.eigenvaluesRe[1] > 0) || (eigenResult.eigenvaluesRe[0] > 0 && eigenResult.eigenvaluesRe[1] < 0)) {
					type = TypeCP::Saddle; // SADDLE
					saddlePoints.push_back(criticalPoint);
				}
				else if (eigenResult.eigenvaluesRe[0] < 0 && eigenResult.eigenvaluesRe[1] < 0)
					type = TypeCP::AttractingNode; // Attracting node
				else if (eigenResult.eigenvaluesRe[0] > 0 && eigenResult.eigenvaluesRe[1] > 0)
					type = TypeCP::RepellingNode; // Repelling node
			}
			// Imaginary part non zero
			//      repelling/attracting focus, center [eliptic sector] = all tanget curves originate AND end in the critical point
			else {

				if (eigenResult.eigenvaluesRe[0] == 0 && eigenResult.eigenvaluesRe[1] == 0)
					type = TypeCP::Center; // Center
				else if (eigenResult.eigenvaluesRe[0] < 0 && eigenResult.eigenvaluesRe[1] < 0)
					type = TypeCP::AttractingFocus; // Attracting focus
				else if (eigenResult.eigenvaluesRe[0] > 0 && eigenResult.eigenvaluesRe[1] > 0)
					type = TypeCP::RepellingFocus; // Repelling focus
			}

			vec4 color = ColorsCP[type];

			// Add first vertex
			indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
			// A vertex has a position, a normal, a texture coordinate and a color
			// we do not use normal or texture coordinate, but still have to specify them
			vertices.push_back({ vec3(criticalPoint.x / (dims.x - 1), criticalPoint.y / (dims.y - 1), 0), vec3(0, 0, 1), vec3(criticalPoint.x / (dims.x - 1), criticalPoint.y / (dims.y - 1), 0), color });
		}
	}

	// (3) Draw separatices
	for (int i = 0; i < saddlePoints.size(); i++) {
		const vec2& saddlePoint = saddlePoints[i];
		mat2 jacobian = Interpolator::sampleJacobian(vol.get(), saddlePoint);
		util::EigenResult eigenResult = util::eigenAnalysis(jacobian);

		// Integrate forward/backward dependend on repelling/attracting focus
		vec2 directions;
		directions[0] = eigenResult.eigenvaluesRe[0] > 0 ? 1 : -1; // RealVal > 0 => outgoing => go forward , else attracting => go backward
		directions[1] = eigenResult.eigenvaluesRe[1] < 0 ? -1 : 1; // RealVal < 0 => incoming => go backward , else repelling => go forward

		
		auto indexBufferSeparatrices1 = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
		auto indexBufferSeparatrices2 = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
		auto indexBufferSeparatrices3 = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
		auto indexBufferSeparatrices4 = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
		// There must be a better way of doing this...

		drawSeparatices(vol.get(), vertices, dims, indexBufferSeparatrices1, indexBufferSeparatrices2, indexBufferSeparatrices3, indexBufferSeparatrices4,
			saddlePoint, eigenResult.eigenvectors, directions);
	}
        
    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

void Topology::drawSeparatices(const Volume* vol,
	std::vector<BasicMesh::Vertex>& vertices,
	const size3_t dims,
	IndexBufferRAM* indexBufferLines1,
	IndexBufferRAM* indexBufferLines2,
	IndexBufferRAM* indexBufferLines3,
	IndexBufferRAM* indexBufferLines4,
	const vec2& saddlePoint,
	const mat2& eigenVectors,
	const vec2& directions) 
{
	// Draw saddle point itself
	vertices.push_back({ vec3(saddlePoint.x / (dims.x - 1), saddlePoint.y / (dims.y - 1), 0), vec3(0), vec3(0), vec4(1, 1, 1, 1) });
	indexBufferLines1->add(static_cast<std::uint32_t>(vertices.size() - 1));
	indexBufferLines2->add(static_cast<std::uint32_t>(vertices.size() - 1));
	indexBufferLines3->add(static_cast<std::uint32_t>(vertices.size() - 1));
	indexBufferLines4->add(static_cast<std::uint32_t>(vertices.size() - 1));

	float factor = propSeparaticesSeedDistance.get();


	// (1) Eigenvector direction [Go for the incoming(RealVal < 0) and outgoing(RealVal > 0) tangent lines, backward and forward respectively.]
	vec2 saddle1 = vec2(saddlePoint.x + factor * eigenVectors[0][0], saddlePoint.y + factor * eigenVectors[0][1]);
	vec2 saddle2 = vec2(saddlePoint.x - factor * eigenVectors[0][0], saddlePoint.y - factor * eigenVectors[0][1]);
	
	Integrator::singleStreamline(vol, dims, saddle1, directions[0], indexBufferLines1, vertices);
	Integrator::singleStreamline(vol, dims, saddle2, directions[0], indexBufferLines2, vertices);

	// (2) Integrate in direction of second Eigenvector
	vec2 saddle3 = vec2(saddlePoint.x + factor * eigenVectors[1][0], saddlePoint.y + factor * eigenVectors[1][1]);
	vec2 saddle4 = vec2(saddlePoint.x - factor * eigenVectors[1][0], saddlePoint.y - factor * eigenVectors[1][1]);

	Integrator::singleStreamline(vol, dims, saddle3, directions[1], indexBufferLines3, vertices);
	Integrator::singleStreamline(vol, dims, saddle4, directions[1], indexBufferLines4, vertices);
}


}// namespace
