/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <lablic/integrator.h>
#include <lablic/interpolator.h>

namespace inviwo
{

    Integrator::Integrator()
    {
    }


// TODO: Implement a single integration step here

    vec2 Integrator::Euler(const Volume* vr, size3_t dims, const vec2& position, float stepSize)
    {
        // x_{i+1} = x_i + s * v(x_i)
        vec2 direction = Interpolator::sampleFromField(vr, position);
        vec2 updatedPosition = position + stepSize*direction;

        return updatedPosition;
    }

    vec2 Integrator::RK4(const Volume* vr, size3_t dims, const vec2& position, float stepSize, bool normalized)
    {
        // 4th order Runga Kutta:
        vec2 v1 = Interpolator::sampleFromField(vr, position);
        vec2 v2 = Interpolator::sampleFromField(vr, position + (stepSize/2.0f)*v1);
        vec2 v3 = Interpolator::sampleFromField(vr, position + (stepSize/2.0f)*v2);
        vec2 v4 = Interpolator::sampleFromField(vr, position + stepSize * v3);


        // c.) normalize vector to integrate over direction field
        if (normalized ) {
            if (v1.x != 0 || v1.y != 0) {
                v1 = v1 / float(sqrt((v1.x*v1.x)+(v1.y*v1.y)));
            }
            if (v2.x != 0 || v2.y != 0) {
                v2 = v2 / float(sqrt((v2.x*v2.x)+(v2.y*v2.y)));
            }
            if (v3.x != 0 || v3.y != 0) {
                v3 = v3 / float(sqrt((v3.x*v3.x)+(v3.y*v3.y)));
            }
            if (v4.x != 0 || v4.y != 0) {
                v4 = v4 / float(sqrt((v4.x*v4.x)+(v4.y*v4.y)));
            }
        }

        vec2 direction = (v1/6.0f + v2/3.0f + v3/3.0f + v4/6.0f);

        // x_{i+1} = x_i + s* ( v1/6 + v2/3 + v3/3 + v4/6 )
        vec2 updatedPosition = position + stepSize * direction;

        return updatedPosition;
    }

} // namespace

