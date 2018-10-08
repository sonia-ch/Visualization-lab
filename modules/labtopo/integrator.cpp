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
    vec2 Integrator::Euler(const Volume* vr, const vec2& position, float stepSize)
    {
        // x_{i+1} = x_i + s * v(x_i)
        vec2 direction = Interpolator::sampleFromField(vr, position);
        vec2 updatedPosition = position + stepSize*direction;

        return updatedPosition;
    }

    vec2 Integrator::RK4(const Volume* vol, const vec2& position, float stepSize, bool normalized)
    {
        // 4th order Runga Kutta:
        vec2 v1 = Interpolator::sampleFromField(vol, position);
        vec2 v2 = Interpolator::sampleFromField(vol, position + (stepSize/2.0f)*v1);
        vec2 v3 = Interpolator::sampleFromField(vol, position + (stepSize/2.0f)*v2);
        vec2 v4 = Interpolator::sampleFromField(vol, position + stepSize * v3);


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

    void Integrator::drawStreamLine(const Volume* vol,
                                      std::vector<BasicMesh::Vertex>& vertices,
                                      const size3_t dims,
                                      IndexBufferRAM* indexBufferLines,
                                      vec2 startPoint,
                                      int direction) {
        // Draw start point
        vertices.push_back({vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                            vec3(0), vec3(0), vec4(0, 0, 0, 1)});
        indexBufferLines->add(static_cast<std::uint32_t>(vertices.size()-1));


        // TODO: Create one stream line from the given start point
        // Initialize variables
        vec2 prevPosition;
        vec2 position = startPoint;
        vec2 changeVec;
        float velocity = 1.0f;
        float arcLength = 0.0f;

        // Initialize properties
        int propMaxSteps = 10000; // propMaxSteps.get()
        float propStepSize = 0.1; // propStepSize.get()
        bool propNormalized = false; // propNormalized.get()
        int propDirection = direction; // propDirection.get()
        float propArcLength = 10000;  // propArcLength.get()


        // Runga Kutta 4th Order
        // d.) stop after max steps
        for (int i=0; i < propMaxSteps; i++) {
            // a.) direction, b.) stepsize & c.) normalized direction field
            prevPosition = position;
            position = RK4(vol, position, propDirection * propStepSize, propNormalized);
            changeVec = position - prevPosition;
            velocity = float(sqrt((changeVec.x*changeVec.x)+(changeVec.y*changeVec.y)));
            arcLength += velocity;

            // e.) after certain arc length
            if (arcLength > propArcLength) {
                break;
            }

            // f.) stop at domain boundary
            if (position.x > (dims.x-1) || position.x < 0 || position.y > (dims.y-1) || position.y < 0) {
                break;
            }

            // g.) zero & h.) slow velocity
            if (velocity == 0.0f || velocity < 0.001f){
                break;
            }

            // Add vertex
            indexBufferLines->add(static_cast<std::uint32_t>(vertices.size()));
            vertices.push_back({ vec3(position.x / (dims.x -1), position.y / (dims.y-1), 0), vec3(0), vec3(0), vec4(0, 0, 1, 1)});
        }

    }


} // namespace inviwo
