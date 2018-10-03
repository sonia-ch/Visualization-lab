/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Monday, October 02, 2017 - 13:31:17
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <lablic/licprocessor.h>
#include <lablic/integrator.h>
#include <lablic/interpolator.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo LICProcessor::processorInfo_{
    "org.inviwo.LICProcessor",  // Class identifier
    "LICProcessor",             // Display name
    "KTH Labs",                 // Category
    CodeState::Experimental,    // Code state
    Tags::None,                 // Tags
};

const ProcessorInfo LICProcessor::getProcessorInfo() const { return processorInfo_; }

LICProcessor::LICProcessor()
    : Processor()
    , volumeIn_("volIn")
    , noiseTexIn_("noiseTexIn")
    , licOut_("licOut")

// TODO: Register additional properties
    , propKernelSize("kernelSize", "Kernel Size", 10, 2, 1000, 1)
    , propBasicLIC("basicLIC", "Basic LIC", false)
    , propFastLIC("fastLIC", "Fast LIC", false)
{
    // Register ports
    addPort(volumeIn_);
    addPort(noiseTexIn_);
    addPort(licOut_);

    // Register properties

    // TODO: Register additional properties
    addProperty(propKernelSize);
    addProperty(propBasicLIC);
    addProperty(propFastLIC);
}

void LICProcessor::process() {
    // Get input
    if (!volumeIn_.hasData()) {
        return;
    }

    if (!noiseTexIn_.hasData()) {
        return;
    }

    auto vol = volumeIn_.getData();
    vectorFieldDims_ = vol->getDimensions();
    auto vr = vol->getRepresentation<VolumeRAM>();

    // An accessible form of on image is retrieved analogous to a volume
    auto tex = noiseTexIn_.getData();
    texDims_ = tex->getDimensions();
    auto tr = tex->getRepresentation<ImageRAM>();

    // Prepare the output, it has the same dimensions and datatype as the output
    // and an editable version is retrieved analogous to a volume
    auto outImage = tex->clone();
    auto outLayer = outImage->getColorLayer();
    auto lr = outLayer->getEditableRepresentation<LayerRAM>();

    // To access the image at a floating point position, you can call
    //      Interpolator::sampleFromGrayscaleImage(tr, somePos)

    // TODO: Implement LIC and FastLIC
    // This code instead sets all pixels to the same gray value
    std::vector<std::vector<double>> licTexture(texDims_.x, std::vector<double>(texDims_.y, 127.0));

    // FastLIC: boolean visited array
    std::vector<std::vector<bool>> visited(texDims_.x, std::vector<bool>(texDims_.y, false));

    // param
    int kernelLength = propKernelSize.get(); // in each direction (backward and forward excluding the starting point)
    double mean = 0;
    double std = 1;

    LogProcessorInfo("VectorField dims " << vectorFieldDims_.x << " , " << vectorFieldDims_.y);
    LogProcessorInfo("texDims dims " << texDims_.x << " , " << texDims_.y);


    for (auto j = 0; j < texDims_.y; j++) {
        for (auto i = 0; i < texDims_.x; i++) {
            // FastLIC
            if (propFastLIC.get()) {
                // FastLIC: If not visited yet
                if (!visited[i][j]) {

                    // 1.) Calculate Stream Line & sample Greyscale values
                    std::vector<vec3> streamline; // (x,y,color)
                    int colorStart = Interpolator::sampleFromGrayscaleImage(tr, vec2(i,j));
                    calculateStreamline(vr, tr, vectorFieldDims_, streamline, vec3(i,j, colorStart));

                    // FastLIC: Repeat along stream line
                    for (auto pos=0; pos < streamline.size(); pos++) {

                        // 3.) Calculate average based on kernel
                        // Borders reduce kernel size & update sum (sliding window)
                        int posBack = 0;
                        int posForward = 0;
                        if (pos-kernelLength >= 0){
                            int posBack = (pos-kernelLength);
                        }
                        if (pos+kernelLength < streamline.size()) {
                            int posForward = (pos+kernelLength);
                        }

                        // Distribution Kernel
                        std::vector<double>elements(streamline[posBack][2], streamline[posForward][2]);
                        double sum = 0;
                        double totalProb = 0;
                        for (auto k=-posBack; k<=posForward; k++){
                            double pdf_gaussian = (1/(std * sqrt(2*M_PI))) * exp(-0.5*pow((k-mean)/std, 2.0));
                            totalProb += pdf_gaussian;
                            sum += streamline[posBack+k][2] * pdf_gaussian;
                        }
                        int color = (sum/totalProb);


                        // 4.) Assign value to field position in output image
                        lr->setFromDVec4(size2_t(streamline[pos][0], streamline[pos][1]), dvec4(color, color, color, 255));

                        // FastLIC: Set field position as visited
                        visited[streamline[pos][0]][streamline[pos][1]] = true;
                    }
                }
            // Basic LIC
            } else if (propBasicLIC.get()){
                // 1.) Calculate Stream Line & sample Greyscale values
                std::vector<vec3> streamline; // (x,y,color)
                int colorStart = Interpolator::sampleFromGrayscaleImage(tr, vec2(i, j));
                int startIndex = calculateStreamline(vr, tr, vectorFieldDims_, streamline, vec3(i, j, colorStart));

                // 2.) Calculate average based on kernel (at field position only)
                int posBack = (startIndex - kernelLength >= 0) ? (startIndex - kernelLength >= 0) : 0;
                int posForward = (startIndex + kernelLength < streamline.size()) ? (startIndex + kernelLength <
                                                                                    streamline.size()) :
                                 streamline.size() - 1;

                // 3.) Kernel average (Box)
                int sum = 0;
                for (auto k = 0; k < (posForward - posBack); k++) {
                    sum += streamline[k][2];
                }
                //int sum = std::accumulate(streamline[posBack][2], streamline[posForward][2], 0);
                int color = (posForward - posBack) > 0 ? (sum / (posForward - posBack)) : sum;


                // 4.) Assign value to field position in output image
                lr->setFromDVec4(size2_t(i, j), dvec4(color, color, color, 255));
            }
            else {
                int val = int(licTexture[i][j]);
                lr->setFromDVec4(size2_t(i,j), dvec4(val,val,val, 255));
            }
        }
    }

    licOut_.setData(outImage);
}

int LICProcessor::calculateStreamline(const VolumeRAM* vr,
                                       const ImageRAM* tr,
                                       const size3_t dims,
                                       std::vector<vec3>& streamline,
                                       vec3 startPoint) {
    // Initialize variables
    vec2 prevPosition;
    vec2 position = startPoint;
    vec2 changeVec;
    float velocity = 1.0f;
    float arcLength = 0.0f;

    // parameters
    int direction = 0;
    bool normalized = false;
    int maxSteps = 1000;
    float stepSize = 1;
    bool doArcLen = true;
    float arcLenParam = 3000;

    // save startpoint index
    int startIndex = 0;

    // Runga Kutta 4th Order (forward and backward
    for (int direction=-1; direction<=1; direction=direction+2) {
        if (direction == 1) {
            // Switch reverse order
            std::reverse(streamline.begin(), streamline.end());
            // Add start point
            streamline.push_back(startPoint);
            startIndex = streamline.size()-1;
        }
        for (int i=0; i < maxSteps; i++) {
            // a.) direction, b.) stepsize & c.) normalized direction field
            prevPosition = position;
            position = Integrator::RK4(vr, dims, position, direction * stepSize, normalized);
            changeVec = position - prevPosition;
            velocity = float(sqrt((changeVec.x*changeVec.x)+(changeVec.y*changeVec.y)));
            arcLength += velocity;

            // e.) after certain arc length
            if (doArcLen && arcLength > arcLenParam) {
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


            // Get color
            int color = Interpolator::sampleFromGrayscaleImage(tr, position);

            // Add vertex
            streamline.push_back({position.x, position.y, color});
        }
    }
    return startIndex;
}



}  // namespace inviwo
