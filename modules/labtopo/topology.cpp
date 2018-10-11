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
    , propThresholdZeroPoints("thresholdZeroPoints", "Threshold Zero points", 0.02, 0.00001, 1, 0.001)
    , propSeparaticesSeedDistance("separaticesSeedDistance", "Distance of separatices seed to saddle point", 0.1, 0.00001, 1, 0.0001)
// propertyName("propertyIdentifier", "Display Name of the Propery",
// default value (optional), minimum value (optional), maximum value (optional), increment (optional));
// propertyIdentifier cannot have spaces
{
    // Register Ports
    addPort(outMesh);
    addPort(inData);
    addProperty(propThresholdZeroPoints);
    addProperty(propSeparaticesSeedDistance);

    // TODO: Register additional properties
    // addProperty(propertyName);
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
    // (1) Find the critical points and
    // (2) color them according to their type.
    // (3) Integrate all separatrices.
    // You can use your previous integration code (copy it over or call it from <lablic/integrator.h>).

    vector<vec2> criticalPoints;
    float initRange = 1;

    // (1) Find critical points
    // Looping through all values in the vector field.
    LogProcessorInfo("Search for points")
    for (int y = 0; y < dims[1]-1; y++) {
        for (int x = 0; x < dims[0]-1; x++) {
//            getCriticalPoints(vol.get(), vec2(x,y), initRange, criticalPoints);
            if (getCP(vol.get(), vec2(x,y), initRange, criticalPoints)) {
                vec2& p = criticalPoints[criticalPoints.size()-1];
                LogProcessorInfo("Add point: (" << p.x/ (dims.x - 1) << ", " << p.y/ (dims.y - 1) << ")");
            }
        }
    }
    LogProcessorInfo("Size criticalPoints: " << criticalPoints.size());

    // (2) Classify the points to color them
    vector<vec2> saddlePoints;
    for (int i = 0; i < criticalPoints.size(); i++) {
        const vec2& criticalPoint = criticalPoints[i];
        mat2 jacobian = Interpolator::sampleJacobian(vol.get(), criticalPoint);
        // determinant: ad - bc is not zero  => First order 2D critical Point
        if ((jacobian[0][1]*jacobian[1][0] - jacobian[0][0]*jacobian[1][1]) != 0) {
            util::EigenResult eigenResult = util::eigenAnalysis(jacobian);

            TypeCP type = TypeCP::Saddle; // TODO: Don't initialize randomly

            // Imaginary zero
            //      Saddle [hyperbolic sector] = All tangent curves to by critical point, except 2 - one originates , other ends
            //      Repelling or Attracting node [parabolic sector] = all tangent end or origin from  critical point
            if (eigenResult.eigenvaluesIm[0] == 0 && eigenResult.eigenvaluesIm[1] == 0){

                if ((eigenResult.eigenvaluesRe[0] < 0 && eigenResult.eigenvaluesRe[1] > 0) || (eigenResult.eigenvaluesRe[0] > 0 && eigenResult.eigenvaluesRe[1] < 0)) {
                    type = TypeCP::Saddle; // SADDLE
                    saddlePoints.push_back(criticalPoint);
                } else if (eigenResult.eigenvaluesRe[0] < 0 && eigenResult.eigenvaluesRe[1] < 0)
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
            vertices.push_back({ vec3(criticalPoint.x / (dims.x-1), criticalPoint.y / (dims.y-1), 0), vec3(0, 0, 1), vec3(criticalPoint.x / (dims.x-1), criticalPoint.y / (dims.y-1), 0), color });
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

        drawSeparatices(vol.get(), vertices, dims, indexBufferSeparatrices, saddlePoint, eigenResult.eigenvectors, directions);

    }

    // (4 extra) Find boundary switch points v(x) is parallel to tangent of boundary curve
    vector<vec2> boundaryPoints;
    vec4 boundaryPointsColor = vec4(100,100,100,255);

    // Go y-axis
    int x1 = 0;
    vec2 previousPointX1;
    vec2 pX1 = Interpolator::sampleFromField(vol.get(), vec2(x1,0)); // initialize previous point

    int x2 = dims[0]-1;
    vec2 previousPointX2;
    vec2 pX2 = Interpolator::sampleFromField(vol.get(), vec2(x2,0)); // initialize previous point

    vec2 possibleBoundaryPoint;
    float threshold = 0.0001;

    for (int y = 1; y < dims[1]; y++) {
        // x1 = 0
        LogProcessorInfo("P_Y(" << x1 << ", " << y-1 << ") =  [" << pX1.x << ", " << pX1.y << "]")

        previousPointX1 = pX1;
        pX1 = Interpolator::sampleFromField(vol.get(), vec2(x1,y));
        if (pX1.x < 0 && previousPointX1.x > 0 || pX1.x > 0 && previousPointX1.x < 0) { // change of sign => interpolate to find zero (possible boundary point)
            float totalDist = std::abs(pX1.x - previousPointX1.x);
            float relDist = pX1.x / totalDist;
            possibleBoundaryPoint = Interpolator::sampleFromField(vol.get(), vec2(x1,y-relDist)); // interpolate to get point
            LogProcessorInfo("PossibleBoundaryPoint(" << x1 << ", " << y-relDist << ") =  [" << possibleBoundaryPoint.x << ", " << possibleBoundaryPoint.y << "]")
            // check parallel to vertical boundary (e.g. not an extreme point)
            LogProcessorInfo("Total Dist " << totalDist << " rel. Dist " << relDist);
            if (possibleBoundaryPoint.y != 0 && std::abs(possibleBoundaryPoint.x) < threshold) {
                boundaryPoints.push_back(possibleBoundaryPoint);
            }
        }


        // x2 = dims-1
        LogProcessorInfo("P_Y(" << x2 << ", " << y-1 << ") =  [" << pX2.x << ", " << pX2.y << "]")

        previousPointX2 = pX2;
        pX2 = Interpolator::sampleFromField(vol.get(), vec2(x2,y));
        if (pX2.x < 0 && previousPointX2.x > 0 || pX2.x > 0 && previousPointX2.x < 0) { // change of sign => interpolate to find zero (possible boundary point)
            float totalDist = std::abs(pX2.x - previousPointX2.x);
            float relDist = pX2.x / totalDist;
            possibleBoundaryPoint = Interpolator::sampleFromField(vol.get(), vec2(x2,y-relDist)); // interpolate to get point
            LogProcessorInfo("PossibleBoundaryPoint(" << x2 << ", " << y-relDist << ") =  [" << possibleBoundaryPoint.x << ", " << possibleBoundaryPoint.y << "]")
            // check parallel to vertical boundary (e.g. not an extreme point)
            LogProcessorInfo("Total Dist " << totalDist << " rel. Dist " << relDist);
            if (possibleBoundaryPoint.y != 0 && std::abs(possibleBoundaryPoint.x) < threshold) {
                boundaryPoints.push_back(possibleBoundaryPoint);
            }
        }
    }

    // Go x-axis
    int y1 = 0;
    vec2 previousPointY1;
    vec2 pY1 = Interpolator::sampleFromField(vol.get(), vec2(y1,0)); // initialize previous point

    int y2 = dims[1]-1;
    vec2 previousPointY2;
    vec2 pY2 = Interpolator::sampleFromField(vol.get(), vec2(y2,0)); // initialize previous point

    for (int x = 1; x < dims[1]; x++) {
        // y1 = 0
        LogProcessorInfo("P_X(" << x-1 << ", " << y1 << ") =  [" << pY1.x << ", " << pY1.y << "]")

        previousPointY1 = pY1;
        pY1 = Interpolator::sampleFromField(vol.get(), vec2(x,y1));
        if (pY1.y < 0 && previousPointY1.y > 0 || pY1.y > 0 && previousPointY1.y < 0) { // change of sign => interpolate to find zero (possible boundary point)
            float totalDist = std::abs(pY1.y - previousPointY1.y);
            float relDist = pY1.y / totalDist;
            possibleBoundaryPoint = Interpolator::sampleFromField(vol.get(), vec2(x-relDist,y1)); // interpolate to get point
            LogProcessorInfo("PossibleBoundaryPoint(" << x-relDist << ", " << y1 << ") =  [" << possibleBoundaryPoint.x << ", " << possibleBoundaryPoint.y << "]")
            // check parallel to vertical boundary (e.g. not an extreme point)
            LogProcessorInfo("Total Dist " << totalDist << " rel. Dist " << relDist);
            if (possibleBoundaryPoint.x != 0 && std::abs(possibleBoundaryPoint.y) < threshold) {
                boundaryPoints.push_back(possibleBoundaryPoint);
            }
        }


        // y2 = dims-1
        LogProcessorInfo("P_X(" << x-1 << ", " << y2 << ") =  [" << pY2.x << ", " << pY2.y << "]")

        previousPointY2 = pY2;
        pY2 = Interpolator::sampleFromField(vol.get(), vec2(x,y2));
        if (pY2.y < 0 && previousPointY2.y > 0 || pY2.y > 0 && previousPointY2.y < 0) { // change of sign => interpolate to find zero (possible boundary point)
            float totalDist = std::abs(pY2.y - previousPointY2.y);
            float relDist = pY2.y / totalDist;
            possibleBoundaryPoint = Interpolator::sampleFromField(vol.get(), vec2(x-relDist,y2)); // interpolate to get point
            LogProcessorInfo("PossibleBoundaryPoint(" << x-relDist << ", " << y2 << ") =  [" << possibleBoundaryPoint.x << ", " << possibleBoundaryPoint.y << "]")
            // check parallel to vertical boundary (e.g. not an extreme point)
            LogProcessorInfo("Total Dist " << totalDist << " rel. Dist " << relDist);
            if (possibleBoundaryPoint.x != 0 && std::abs(possibleBoundaryPoint.y) < threshold) {
                boundaryPoints.push_back(possibleBoundaryPoint);
            }
        }
    }


    // (5 extra) draw separatices and points at boundary points
    LogProcessorInfo("BoundaryPoints printout size:" << boundaryPoints.size());
    for (int i = 0; i < boundaryPoints.size(); i++) {
        const vec2& boundaryPoint = boundaryPoints[i];
        mat2 jacobian = Interpolator::sampleJacobian(vol.get(), boundaryPoint);
        util::EigenResult eigenResult = util::eigenAnalysis(jacobian);

        LogProcessorInfo("Boundary point (" << boundaryPoint.x << ", " << boundaryPoint.y << ")");

        // Add to points
        indexBufferPoints->add(static_cast<std::uint32_t>(vertices.size()));
        vertices.push_back({ vec3(boundaryPoint.x / (dims.x-1), boundaryPoint.y / (dims.y-1), 0), vec3(0, 0, 1), vec3(boundaryPoint.x / (dims.x-1), boundaryPoint.y / (dims.y-1), 0), boundaryPointsColor });


        // Integrate forward/backward dependend on repelling/attracting focus
        vec2 directions;
        directions[0] = eigenResult.eigenvaluesRe[0] > 0 ? 1 : -1; // RealVal > 0 => outgoing => go forward , else attracting => go backward
        directions[1] = eigenResult.eigenvaluesRe[1] < 0 ? -1 : 1; // RealVal < 0 => incoming => go backward , else repelling => go forward

        drawSeparatices(vol.get(), vertices, dims, indexBufferSeparatrices, boundaryPoint, eigenResult.eigenvectors, directions);

    }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

void Topology::drawSeparatices(const Volume* vol,
                               std::vector<BasicMesh::Vertex>& vertices,
                               const size3_t dims,
                               IndexBufferRAM* indexBufferLines,
                               const vec2& saddlePoint,
                               const mat2& eigenVectors,
                               const vec2& directions) {
    // Draw saddle point itself
    vertices.push_back({vec3(saddlePoint.x / (dims.x - 1), saddlePoint.y / (dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 0, 0, 1)});
    indexBufferLines->add(static_cast<std::uint32_t>(vertices.size()-1));

    float factor = propSeparaticesSeedDistance.get();


    // (1) Eigenvector direction [Go for the incoming(RealVal < 0) and outgoing(RealVal > 0) tangent lines, backward and forward respectively.]
    vec2 saddle1 = vec2(saddlePoint.x + factor * eigenVectors[0][0], saddlePoint.y + factor * eigenVectors[0][1]);
    vec2 saddle2 = vec2(saddlePoint.x - factor * eigenVectors[0][0], saddlePoint.y - factor * eigenVectors[0][1]);
    Integrator::drawStreamLine(vol, vertices, dims, indexBufferLines, saddle1, directions[0]);
    Integrator::drawStreamLine(vol, vertices, dims, indexBufferLines, saddle2, directions[0]);

    // (2) Integrate in direction of second Eigenvector
    vec2 saddle3 = vec2(saddlePoint.x + factor * eigenVectors[1][0], saddlePoint.y + factor * eigenVectors[1][1]);
    vec2 saddle4 = vec2(saddlePoint.x - factor * eigenVectors[1][0], saddlePoint.y - factor * eigenVectors[1][1]);
    Integrator::drawStreamLine(vol, vertices, dims, indexBufferLines, saddle3, directions[1]);
    Integrator::drawStreamLine(vol, vertices, dims, indexBufferLines, saddle4, directions[1]);
}

bool Topology::changeOfSign(const vec2& f00, const vec2& f10, const vec2& f01, const vec2& f11) {
    // two vectors have a change of sign if
    // check for change of sign for x-attribute
    if ((f00.x < 0 && f10.x > 0) || (f00.x > 0 && f10.x < 0) || // [f00] -------------------- [f10]
        (f00.x < 0 && f01.x > 0) || (f00.x > 0 && f01.x < 0) || // [f00] - [f01]
        (f01.x < 0 && f11.x > 0) || (f01.x > 0 && f11.x < 0) || // [f01] -------------------- [f11]
        (f10.x < 0 && f11.x > 0) || (f10.x > 0 && f11.x < 0)) { // [f10] - [f11]

        // check for change of sign for y-attribute
        if ((f00.y < 0 && f10.y > 0) || (f00.y > 0 && f10.y < 0) || // [f00] -------------------- [f10]
            (f00.y < 0 && f01.y > 0) || (f00.y > 0 && f01.y < 0) || // [f00] - [f01]
            (f01.y < 0 && f11.y > 0) || (f01.y > 0 && f11.y < 0) || // [f01] -------------------- [f11]
            (f10.y < 0 && f11.y > 0) || (f10.y > 0 && f11.y < 0)) { // [f10] - [f11]
            return true;
        }
    }
    return false;
}

void Topology::getCriticalPoints(const Volume* vol, const vec2& leftCorner, float range, std::vector<vec2>& criticalPoints) {
    // [f01] -------------------- [f11]
    //       |                  |
    // [f00] -------------------- [f10]
    vec2 f00 = Interpolator::sampleFromField(vol, vec2(leftCorner.x,leftCorner.y));
    vec2 f10 = Interpolator::sampleFromField(vol, vec2(leftCorner.x+range,leftCorner.y));
    vec2 f11 = Interpolator::sampleFromField(vol, vec2(leftCorner.x+range,leftCorner.y+range));
    vec2 f01 = Interpolator::sampleFromField(vol, vec2(leftCorner.x,leftCorner.y+range));

    if (changeOfSign(f00, f10, f11, f01)) {
        // If we cross threshold add critical point
        if (range < propThresholdZeroPoints.get()) {
            criticalPoints.push_back(leftCorner);
            LogProcessorInfo("Add point: (" << leftCorner.x << ", " << leftCorner.y << ")");
//                LogProcessorInfo("\tf00 (" << f00.x << ", " << f00.y << ")");
//                LogProcessorInfo("\tf01 (" << f01.x << ", " << f01.y << ")");
//                LogProcessorInfo("\tf10 (" << f10.x << ", " << f10.y << ")");
//                LogProcessorInfo("\tf11 (" << f11.x << ", " << f11.y << ")");
            LogProcessorInfo("\tf00 (" << ((f00.x > 0) ? "+" : "-") << ", " << ((f00.x > 0) ? "+" : "-") << ")");
            LogProcessorInfo("\tf01 (" << ((f01.x > 0) ? "+" : "-") << ", " << ((f01.x > 0) ? "+" : "-") << ")");
            LogProcessorInfo("\tf10 (" << ((f10.x > 0) ? "+" : "-") << ", " << ((f10.x > 0) ? "+" : "-") << ")");
            LogProcessorInfo("\tf11 (" << ((f11.x > 0) ? "+" : "-") << ", " << ((f11.x > 0) ? "+" : "-") << ")");

            // Else Search in the 4 sub-domains to get point coordinates
        } else {
            // There is a possible zero (because change-of-sign)
            getCriticalPoints(vol, leftCorner, range/2.0f, criticalPoints);
            getCriticalPoints(vol, vec2(leftCorner.x+(range/2.0f), leftCorner.y), range/2.0f, criticalPoints);
            getCriticalPoints(vol, vec2(leftCorner.x+(range/2.0f), leftCorner.y+(range/2.0f)), range/2.0f, criticalPoints);
            getCriticalPoints(vol, vec2(leftCorner.x, leftCorner.y+(range/2.0f)), range/2.0f, criticalPoints);
        }
    }
}

// Using assumption: Maximal one point per cell
bool Topology::getCP(const Volume* vol, const vec2& leftCorner, float range, std::vector<vec2>& criticalPoints) {
    // If we cross threshold add critical point
    if (range < propThresholdZeroPoints.get()) {
        criticalPoints.push_back(leftCorner);
        return true;
    // Else Search in the 4 sub-domains to get point coordinates
    } else {
        // [f01] -------------------- [f11]
        //       |                  |
        // [f00] -------------------- [f10]
        vec2 f00 = Interpolator::sampleFromField(vol, vec2(leftCorner.x,leftCorner.y));
        vec2 f10 = Interpolator::sampleFromField(vol, vec2(leftCorner.x+range,leftCorner.y));
        vec2 f11 = Interpolator::sampleFromField(vol, vec2(leftCorner.x+range,leftCorner.y+range));
        vec2 f01 = Interpolator::sampleFromField(vol, vec2(leftCorner.x,leftCorner.y+range));

        // check for change of sign on x-axis
        if (changeOfSign(f00,f10,f11,f01)) {
            // There is a possible zero (because change-of-sign)
            return (getCP(vol, leftCorner, range/2.0f, criticalPoints) ||
                    getCP(vol, vec2(leftCorner.x+(range/2.0f), leftCorner.y), range/2.0f, criticalPoints) ||
                    getCP(vol, vec2(leftCorner.x+(range/2.0f), leftCorner.y+(range/2.0f)), range/2.0f, criticalPoints) ||
                    getCP(vol, vec2(leftCorner.x, leftCorner.y+(range/2.0f)), range/2.0f, criticalPoints)
            );
        }
        return false;
    }
}



}// namespace
