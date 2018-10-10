/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labstreamlines/integrator.h>

namespace inviwo
{

Integrator::Integrator()
{
}

vec2 Integrator::sampleFromField(const VolumeRAM* vr, size3_t dims, const vec2& position)
{
    // Sampled outside the domain!
    if (position[0] < 0 || position[0] > dims[0] - 1 || position[1] < 0 || position[1] > dims[1] - 1)
    {
        return vec2(0, 0);
    }

    int x0 = int(position[0]);
    int y0 = int(position[1]);

    // Leads to accessing only inside the volume
    // Coefficients computation takes care of using the correct values
    if (x0 == dims[0] - 1)
    {
        x0--;
    }
    if (y0 == dims[1] - 1)
    {
        y0--;
    }

    auto f00 = vr->getAsDVec2(size3_t(x0, y0, 0));
    auto f10 = vr->getAsDVec2(size3_t(x0 + 1, y0, 0));
    auto f01 = vr->getAsDVec2(size3_t(x0, y0 + 1, 0));
    auto f11 = vr->getAsDVec2(size3_t(x0 + 1, y0 + 1, 0));

    float x = position[0] - x0;
    float y = position[1] - y0;

    vec2 f;

    for (int i = 0; i < 2; i++)
    {
        f[i] = f00[i] * (1 - x) * (1 - y) + f01[i] * (1 - x) * y + f10[i] * x * (1 - y) + f11[i] * x * y;
    }

    return f;
}


// TODO: Implement a single integration step here

vec2 Integrator::Euler(const VolumeRAM* vr, size3_t dims, const vec2& position, float stepSize)
{
 //Access the vector field with sampleFromField(vr, dims, ...)
	
	vec2 dir = sampleFromField(vr, dims, position);
	vec2 point = position + stepSize * dir;
	return point;

}

vec2 Integrator::RK4(const VolumeRAM* vr, size3_t dims, const vec2& position, float stepSize, int direction, bool normalized)
{
	vec2 v1 = (float)direction * Integrator::sampleFromField(vr, dims, position);
	vec2 v2 = (float)direction * Integrator::sampleFromField(vr, dims, position + stepSize / 2 * v1);
	vec2 v3 = (float)direction * Integrator::sampleFromField(vr, dims, position + stepSize / 2 * v2);
	vec2 v4 = (float)direction * Integrator::sampleFromField(vr, dims, position + stepSize * v3);

	//vec2 point = position + stepSize * (v1 / 6.0f + v2 / 3.0f + v3 / 3.0f + v4 / 6.0f);
	vec2 step = (v1 / 6.0f + v2 / 3.0f + v3 / 3.0f + v4 / 6.0f);
	// c.) normalize vector to integrate over direction field
	if (normalized) {
		step = step / float(sqrt((step.x*step.x) + (step.y*step.y)));
	}
	return position + stepSize * step;

}

} // namespace

