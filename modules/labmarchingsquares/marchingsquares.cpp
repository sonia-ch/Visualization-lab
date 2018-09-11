/*********************************************************************
 *  Author  : Himangshu Saikia, Wiebke Koepp, ...
 *  Init    : Monday, September 11, 2017 - 12:58:42
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labmarchingsquares/marchingsquares.h>
#include <inviwo/core/util/utilities.h>

namespace inviwo
{

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MarchingSquares::processorInfo_
{
    "org.inviwo.MarchingSquares",      // Class identifier
    "Marching Squares",                // Display name
    "KTH Lab",                          // Category
    CodeState::Experimental,           // Code state
    Tags::None,                        // Tags
};

const ProcessorInfo MarchingSquares::getProcessorInfo() const
{
    return processorInfo_;
}


MarchingSquares::MarchingSquares()
    :Processor()
    , inData("volumeIn")
    , meshOut("meshOut")
    , propShowGrid("showGrid", "Show Grid")
    , propDeciderType("deciderType", "Decider Type")
    , propMultiple("multiple", "Iso Levels")
    , propIsoValue("isovalue", "Iso Value")
    , propGridColor("gridColor", "Grid Lines Color", vec4(0.0f, 0.0f, 0.0f, 1.0f),
        vec4(0.0f), vec4(1.0f), vec4(0.1f),
        InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , propIsoColor("isoColor", "Color", vec4(0.0f, 0.0f, 1.0f, 1.0f),
        vec4(0.0f), vec4(1.0f), vec4(0.1f),
        InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , propNumContours("numContours", "Number of Contours", 1, 1, 50, 1)
    , propIsoTransferFunc("isoTransferFunc", "Colors", &inData)
    , propApplyGaussian("filter","GaussianFilter")
{
    // Register ports
    addPort(inData);
    addPort(meshOut);
	
    // Register properties
    addProperty(propShowGrid);
    addProperty(propGridColor);
	
    addProperty(propDeciderType);
    propDeciderType.addOption("midpoint", "Mid Point", 0);
    propDeciderType.addOption("asymptotic", "Asymptotic", 1);

    addProperty(propMultiple);
    
    propMultiple.addOption("single", "Single", 0);
    addProperty(propIsoValue);
    addProperty(propIsoColor);

    propMultiple.addOption("multiple", "Multiple", 1);
    addProperty(propNumContours);
    addProperty(propIsoTransferFunc);

    // Add Gaussian smoothing property
    addProperty(propApplyGaussian);

    // TODO (Bonus): Use the transfer function property to assign a color
    // The transfer function normalizes the input data and sampling colors
    // from the transfer function assumes normalized input, that means
    // vec4 color = propIsoTransferFunc.get().sample(0.0f);
    // is the color for the minimum value in the data
    // vec4 color = propIsoTransferFunc.get().sample(1.0f);
    // is the color for the maximum value in the data

    // The customized transfer function has
    // a blue point (0,0) for low values
    // and a red point (1,1) for high values
    propIsoTransferFunc.get().clearPoints();
    propIsoTransferFunc.get().addPoint(vec2(0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f)); // blue
    propIsoTransferFunc.get().addPoint(vec2(1.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f)); // red
    propIsoTransferFunc.setCurrentStateAsDefault();


    util::hide(propGridColor, propNumContours, propIsoTransferFunc);

    // Show the grid color property only if grid is actually displayed
    propShowGrid.onChange([this]()
    {
        if (propShowGrid.get())
        {
            util::show(propGridColor);
        }
        else
        {
            util::hide(propGridColor);
        }
    });

    // Show options based on display of one or multiple iso contours
    propMultiple.onChange([this]()
    {
        if (propMultiple.get() == 0)
        {
            util::show(propIsoValue, propIsoColor);
            util::hide(propNumContours, propIsoTransferFunc);
        }
        else
        {
//            util::hide(propIsoValue);
//            util::show(propIsoColor, propNumContours);
            
            // TODO (Bonus): Comment out above if you are using the transfer function
            // and comment in below instead
             util::hide(propIsoValue, propIsoColor);
             util::show(propNumContours, propIsoTransferFunc);
        }
    });

}



void MarchingSquares::process()
{
    if (!inData.hasData()) {
	    return;
    }

    // This results in a shared pointer to a volume
    auto vol = inData.getData();

    // Extract the minimum and maximum value from the input data
    const double minValue = vol->dataMap_.valueRange[0];
    const double maxValue = vol->dataMap_.valueRange[1];

    // Set the range for the isovalue to that minimum and maximum
    propIsoValue.setMinValue(minValue);
    propIsoValue.setMaxValue(maxValue);

    // You can print to the Inviwo console with Log-commands:
    LogProcessorInfo("This scalar field contains values between " << minValue << " and " << maxValue << ".");
    // You can also inform about errors and warnings:
    // LogProcessorWarn("I am warning about something"); // Will print warning message in yellow
    // LogProcessorError("I am letting you know about an error"); // Will print error message in red
    // (There is also LogNetwork...() and just Log...(), these display a different source,
    // LogProcessor...() for example displays the name of the processor in the workspace while
    // Log...() displays the identifier of the processor (thus with multiple processors of the
    // same kind you would not know which one the information is coming from
    
    // Retreive data in a form that we can access it
    const VolumeRAM* vr = vol->getRepresentation< VolumeRAM >();
    const size3_t dims = vol->getDimensions();

    // Define cellWidths on unit-square
    float cellWidthX = (1.0/(dims.x-1));
    float cellWidthY = (1.0/(dims.y-1));

    // Initialize mesh and vertices
    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;

    // The function drawLineSegments creates two vertices at the specified positions,
    // that are placed into the Vertex vector defining our mesh.
    // An index buffer specifies which of those vertices should be grouped into to make up lines/trianges/quads.
    // Here two vertices make up a line segment.
    auto indexBufferGrid = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);


    // Values within the input data are accessed by the function below
    // It's input is the VolumeRAM from above, the dimensions of the volume
    // and the indeces i and j of the position to be accessed where
    // i is in [0, dims.x-1] and j is in [0, dims.y-1]
    float valueat00 = getInputValue(vr, dims, 0, 0);
    LogProcessorInfo("Value at (0,0) is: " << valueat00);
    // You can assume that dims.z = 1 and do not need to consider others cases

    // TODO: Gaussian call
    const int radius = 3; // radius x radius of neighbours to calculate gaussian smoothing
    const VolumeRAM* data = propApplyGaussian.get() ?  gaussianSmoothing(vol, vr, dims, radius) : vr;

    // Grid

    // Properties are accessed with propertyName.get() 
    if (propShowGrid.get())
    {
        // TODO: Add grid lines of the given color
        // Task1 a) Draw a bounding box for the data
        // Dimensions give us the amount of points in x/y
        // They are saved as a matrix of values, and accessible (x,y) => height-value
        for (float i=0; i < dims.x; i++){
            float x = cellWidthX *i;
            vec2 v1 = vec2(x, 0);
            vec2 v2 = vec2(x, 1);
            drawLineSegment(v1, v2, propGridColor.get(), indexBufferGrid, vertices);
        }
        for (float j=0; j < dims.y; j++){
            float y = cellWidthY *j;
            vec2 v1 = vec2(0, y);
            vec2 v2 = vec2(1, y);
            drawLineSegment(v1, v2, propGridColor.get(), indexBufferGrid, vertices);
        }
    }

    // Vector containing the isolines
    std::vector<float> isolines;


    if (propMultiple.get() == 0)
    {
        LogProcessorInfo(" Draw single Isoline for isovalue " );

        // TODO: Draw a single isoline at the specified isovalue (propIsoValue) 
        // and color it with the specified color (propIsoColor)
        isolines.push_back(propIsoValue.get());

        // call algorithm with only one isoline value
        algorithm(isolines, dims, cellWidthX,cellWidthY, data, indexBufferGrid, vertices);
    }
    else
    {
        // TODO: Draw the given number (propNumContours) of isolines between 
        // the minimum and maximum value
        int n = propNumContours.get();
        float range = propIsoValue.getMaxValue() - propIsoValue.getMinValue();
        // Divide by (n+1) such that for one line it would be in the middle. (Not on the borders)
        float width = range/(n+1);
        for (int i=0; i<n; i++){
            isolines.push_back(minValue + width*(i+1));
        }

        // call algorithm with multiple isoline values
        algorithm(isolines, dims, cellWidthX,cellWidthY, data, indexBufferGrid, vertices);


    }

    // Note: It is possible to add multiple index buffers to the same mesh,
    // thus you could for example add one for the grid lines and one for
    // each isoline
    // Also, consider to write helper functions to avoid code duplication
    // e.g. for the computation of a single iso contour

    mesh->addVertices(vertices);
    meshOut.setData(mesh);
}

VolumeRAM* MarchingSquares::gaussianSmoothing(const std::shared_ptr<const Volume> vol,
                                             const VolumeRAM* vr,
                                             const size3_t& dims,
                                             const int radius){
    // TODO (Bonus) Gaussian filter
    // Our input is const, but you need to compute smoothed data and write it somewhere
    // Create an editable volume like this:
    // Volume volSmoothed(vol->getDimensions(), vol->getDataFormat());
    // auto vrSmoothed = volSmoothed.getEditableRepresentation<VolumeRAM>();
    // Values can be set with
    // vrSmoothed->setFromDouble(vec3(i,j,0), value);
    // getting values works with an editable volume as well
    // getInputValue(vrSmoothed, dims, 0, 0);

    Volume volSmoothed(vol->getDimensions(), vol->getDataFormat());
    auto vrSmoothed = volSmoothed.getEditableRepresentation<VolumeRAM>();

    for (int i=0; i < dims.x; i++){
        for (int j=0; j < dims.y; j++){
//            Code..
//            value =
//            vrSmoothed->setFromDouble(vec3(i,j,0), value);
//            getInputValue(vrSmoothed, dims, 0, 0);

        }
    }
    return vrSmoothed;
}

void MarchingSquares::algorithm(const std::vector<float>& isolines,
                                const size3_t& dims,
                                const float& cellWidthX,
                                const float& cellWidthY,
                                const VolumeRAM* vr,
                                IndexBufferRAM* indexBufferGrid,
                                std::vector<BasicMesh::Vertex>& vertices){
    // Iso contours
    // Basic Marching Squares algorithm:
    // 1. Input: data array and isovalue
    // 2. Find all grid cells which are intersected by the isoline
    // 3. Find intersection points of grid boundaries and isoline by interpolation
    // 4. Draw isoline

    // For all isolines
    for (int i=0; i < isolines.size(); i++) {
        const float& c = isolines[i];

        // Determine color, single = propIsoColor , multiple = transferFunction
        const vec4& color = (propMultiple.get() == 0) ? propIsoColor.get() : propIsoTransferFunc.get().sample(c);

        // Iterate until SMALLER size-1 to acces all grid-cells from left lower corner (fixpoint - [x,y])
        for (int x=0; x < dims.x-1; x++){
            for (int y=0; y < dims.y-1; y++){
                double f00 = getInputValue(vr, dims, x, y);
                double f10 = getInputValue(vr, dims, x+1, y);
                double f01 = getInputValue(vr, dims, x, y+1);
                double f11 = getInputValue(vr, dims, x+1, y+1);
                std::vector<double> cell = {f00, f10, f01, f11};

                // relative point coordinates for x,y-index
                float coordX0 = x*cellWidthX;
                float coordX1 = (x+1)*cellWidthX;
                float coordY0 = y*cellWidthY;
                float coordY1 = (y+1)*cellWidthY;


                // Linear interpolation along cell edges
                // 1.) Calculate intersection points
                std::vector<vec2> intersectionPoints;
                // bottom edge
                if ((f00 < c and f10 > c) or (f00 > c and f10 < c)){
                    float weightX = (c-f00)/(f10-f00);
                    intersectionPoints.push_back(vec2(coordX0 + (weightX * cellWidthX), coordY0));
                }
                // left edge
                if ((f00 < c and f01 > c) or (f00 > c and f01 < c)) {
                    float weightY = (c-f00)/(f01-f00);
                    intersectionPoints.push_back(vec2(coordX0, coordY0 + (weightY * cellWidthY)));
                }
                // top edge
                if ((f11 < c and f01 > c) or (f11 > c and f01 < c)){
                    float weightX = (c-f01)/(f11-f01);
                    intersectionPoints.push_back(vec2(coordX0 + (weightX * cellWidthX), coordY1));
                }
                // right edge
                if ((f11 < c and f10 > c) or (f11 > c and f10 < c)){
                    float weightY = (c-f10)/(f11-f10);
                    intersectionPoints.push_back(vec2(coordX1, coordY0 + (weightY * cellWidthY)));
                }

                // 2.) Draw the lines based on the intersection points (4 cases: no points, 2 points, 4 points -> neighbours or opponents)
                LogProcessorInfo(intersectionPoints.size() << " # Intersection points for (" << x << "," << y << ") cell");
                for (int i=0; i<intersectionPoints.size(); i++){
                    LogProcessorInfo(intersectionPoints[i]);
                }

                // Case 1: 2 points
                if (intersectionPoints.size() == 2){
                    drawLineSegment(intersectionPoints[0], intersectionPoints[1], color, indexBufferGrid, vertices);
                } else if (intersectionPoints.size() == 4){
                    // Case 2 and 3 are ambigious
                    if (propDeciderType.get() == 1){
                        asymptoticDecider(intersectionPoints, indexBufferGrid, vertices, color);
                    } else {
                        midPointDecider(c, f00, f10, f01, f11, intersectionPoints, indexBufferGrid, vertices, color);
                    }

                }// Case 4: All points same sign => does not cross
            }
        }
    }
}

void MarchingSquares::asymptoticDecider(std::vector<vec2> intersectionPoints,
                                        IndexBufferRAM* indexBufferGrid,
                                        std::vector<BasicMesh::Vertex>& vertices,
                                        const vec4& color) {
    // We use the simpler method by sorting the points on the x axis and then draw a line between 1-2 and 3-4
    // Instead of calculating the asymptotes mathematically
    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](vec2 point1,vec2 point2){ return (point1[0]<point2[0]);});
    drawLineSegment(intersectionPoints[0], intersectionPoints[1], color, indexBufferGrid, vertices);
    drawLineSegment(intersectionPoints[2], intersectionPoints[3], color, indexBufferGrid, vertices);

}

void MarchingSquares::midPointDecider(const float& c,
                                      const double& f00,
                                      const double& f10,
                                      const double& f01,
                                      const double& f11,
                                      const std::vector<vec2>& intersectionPoints,
                                      IndexBufferRAM* indexBufferGrid,
                                      std::vector<BasicMesh::Vertex>& vertices,
                                      const vec4& color) {

    // Midpoint decider
    float midpoint = (1.0/4.0) * (f00 + f10 + f01 + f11);
    // Case 2: Connect negative
    if (midpoint < c){
        if (f00 < c){
            // + |---------| -
            //   |    -    |
            // - |---------| +
            // Connect left and top edge
            drawLineSegment(intersectionPoints[1], intersectionPoints[2], color, indexBufferGrid, vertices);
            // Connect bottom and right edge
            drawLineSegment(intersectionPoints[0], intersectionPoints[3], color, indexBufferGrid, vertices);
        } else {
            // - |---------| +
            //   |    -    |
            // + |---------| -
            // Connect bottom and left edge
            drawLineSegment(intersectionPoints[0], intersectionPoints[1], color, indexBufferGrid, vertices);
            // Connect top and right edge
            drawLineSegment(intersectionPoints[2], intersectionPoints[3], color, indexBufferGrid, vertices);
        }
        // Case 3: Connect positive
    } else {
        if (f00 < c){
            // + |---------| -
            //   |    +    |
            // - |---------| +
            // Connect bottom and left edge
            drawLineSegment(intersectionPoints[0], intersectionPoints[1], color, indexBufferGrid, vertices);
            // Connect top and right edge
            drawLineSegment(intersectionPoints[2], intersectionPoints[3], color, indexBufferGrid, vertices);
        } else {
            // - |---------| +
            //   |    +    |
            // + |---------| -
            // Connect left and top edge
            drawLineSegment(intersectionPoints[1], intersectionPoints[2], color, indexBufferGrid, vertices);
            // Connect bottom and right edge
            drawLineSegment(intersectionPoints[0], intersectionPoints[3], color, indexBufferGrid, vertices);
        }
    }
}

double MarchingSquares::getInputValue(const VolumeRAM* data, const size3_t dims, 
    const size_t i, const size_t j) {
    // Check if the indices are withing the dimensions of the volume
    if (i < dims.x && j < dims.y) {
        return data->getAsDouble(size3_t(i, j, 0));
    } else {
        LogProcessorError(
            "Attempting to access data outside the boundaries of the volume, value is set to 0");
        return 0;
    }
}

void MarchingSquares::drawLineSegment(const vec2& v1, const vec2& v2, const vec4& color,
                                      IndexBufferRAM* indexBuffer,
                                      std::vector<BasicMesh::Vertex>& vertices) {
    // Add first vertex
    indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
    // A vertex has a position, a normal, a texture coordinate and a color
    // we do not use normal or texture coordinate, but still have to specify them
    vertices.push_back({vec3(v1[0], v1[1], 0), vec3(0, 0, 1), vec3(v1[0], v1[1], 0), color});
    // Add second vertex
    indexBuffer->add(static_cast<std::uint32_t>(vertices.size()));
    vertices.push_back({vec3(v2[0], v2[1], 0), vec3(0, 0, 1), vec3(v2[0], v2[1], 0), color});
}

} // namespace
