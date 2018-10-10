/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labtopo/integrator.h>
#include <labtopo/interpolator.h>

namespace inviwo {

Integrator::Integrator() {}

// TODO: Implementation of the functions defined in the header file integrator.h

void Integrator::singleStreamline(const Volume* vr, size3_t dims, const vec2& startPosition, int direction,
	IndexBufferRAM* indexBuffer, std::vector<BasicMesh::Vertex>& vertices)
{
	
	indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
	vertices.push_back({ vec3(startPosition.x / (dims.x - 1), startPosition.y / (dims.y - 1), 0),
		vec3(0), vec3(0), vec4(1, 1, 1, 1) });

	vec2 nextPointRK = startPosition;
	vec2 prevPoint, pointDiff;
	float velocity, arcLength = 0.0f;

	// Initialize properties
	int numSteps = 10000; // propMaxSteps.get()
	float propStepSize = 0.01; // propStepSize.get()
	bool propNormalized = false; // propNormalized.get()
	int propDirection = direction; // propDirection.get()
	float propArcLength = 10000;  // propArcLength.get()

	for (int i = 0; i < numSteps; i++)
	{
		vec2 prevPoint = nextPointRK;
		// RK integration
		nextPointRK = Integrator::RK4(vr, nextPointRK, propStepSize * direction, propNormalized);

		pointDiff = nextPointRK - prevPoint;
		velocity = float(sqrt((pointDiff.x*pointDiff.x) + (pointDiff.y*pointDiff.y)));
		arcLength += velocity;

		// e.) after certain arc length
		if (arcLength > propArcLength) {
			//LogProcessorInfo("Maximal arc length " << arcLength);
			break;
		}

		// f.) stop at domain boundary
		if (nextPointRK.x >(dims.x - 1) || nextPointRK.x < 0 || nextPointRK.y >(dims.y - 1) || nextPointRK.y < 0) {
			//LogProcessorInfo("Out of domain boundaries (" << nextPointRK.x << "," << nextPointRK.y << ")");
			break;
		}

		// g.) zero & h.) slow velocity
		if (velocity == 0.0f || velocity < 0.001f) {
			//LogProcessorInfo("Velocity is to slow " << velocity);
			break;
		}

		// Draw next point
		indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
		vertices.push_back({ vec3(nextPointRK.x / (dims.x - 1), nextPointRK.y / (dims.y - 1), 0),
			vec3(0), vec3(0), vec4(1, 1, 1, 1) });

	}
}

vec2 Integrator::RK4(const Volume* vol, const vec2& position, float stepSize, bool normalized)
{
	// 4th order Runga Kutta:
	vec2 v1 = Interpolator::sampleFromField(vol, position);
	vec2 v2 = Interpolator::sampleFromField(vol, position + (stepSize / 2.0f)*v1);
	vec2 v3 = Interpolator::sampleFromField(vol, position + (stepSize / 2.0f)*v2);
	vec2 v4 = Interpolator::sampleFromField(vol, position + stepSize * v3);


	// c.) normalize vector to integrate over direction field
	if (normalized) {
		if (v1.x != 0 || v1.y != 0) {
			v1 = v1 / float(sqrt((v1.x*v1.x) + (v1.y*v1.y)));
		}
		if (v2.x != 0 || v2.y != 0) {
			v2 = v2 / float(sqrt((v2.x*v2.x) + (v2.y*v2.y)));
		}
		if (v3.x != 0 || v3.y != 0) {
			v3 = v3 / float(sqrt((v3.x*v3.x) + (v3.y*v3.y)));
		}
		if (v4.x != 0 || v4.y != 0) {
			v4 = v4 / float(sqrt((v4.x*v4.x) + (v4.y*v4.y)));
		}
	}

	vec2 direction = (v1 / 6.0f + v2 / 3.0f + v3 / 3.0f + v4 / 6.0f);

	// x_{i+1} = x_i + s* ( v1/6 + v2/3 + v3/3 + v4/6 )
	vec2 updatedPosition = position + stepSize * direction;

	return updatedPosition;
}


}  // namespace inviwo
