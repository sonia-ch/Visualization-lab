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
	, propApplyGaussian("filter", "Gaussian Filter")
	, propSigma("sigma", "Sigma", 0.5f, 0.1f, 1.0f, 0.01f)
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

    // The default transfer function has just two blue points
    /*propIsoTransferFunc.get().clearPoints();
    propIsoTransferFunc.get().addPoint(vec2(0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));
    propIsoTransferFunc.get().addPoint(vec2(1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));
    propIsoTransferFunc.setCurrentStateAsDefault();*/

	// The customized transfer function has blue for low values, green for middle-low, 
	// yellow for middle-high and red for high.
	propIsoTransferFunc.get().clearPoints();
	propIsoTransferFunc.get().addPoint(vec2(0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f)); // blue
	propIsoTransferFunc.get().addPoint(vec2(0.3f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f)); // green
	propIsoTransferFunc.get().addPoint(vec2(0.6f, 1.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f)); // yellow
	propIsoTransferFunc.get().addPoint(vec2(1.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f)); // red
	propIsoTransferFunc.setCurrentStateAsDefault();

	addProperty(propApplyGaussian);
	addProperty(propSigma);

    util::hide(propGridColor, propNumContours, propIsoTransferFunc, propSigma);

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

	// Show the sigma property only if Gaussian Filter is actually applied
	propApplyGaussian.onChange([this]()
	{
		if (propApplyGaussian.get())
		{
			util::show(propSigma);
		}
		else
		{
			util::hide(propSigma);
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
            // util::hide(propIsoValue);
            // util::show(propIsoColor, propNumContours);
            
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
    //LogProcessorInfo("This scalar field contains values between " << minValue << " and " << maxValue << ".");
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

    // Initialize mesh and vertices
    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;

    // Values within the input data are accessed by the function below
    // It's input is the VolumeRAM from above, the dimensions of the volume
    // and the indeces i and j of the position to be accessed where
    // i is in [0, dims.x-1] and j is in [0, dims.y-1]
    // float valueat00 = getInputValue(vr, dims, 0, 0);
    // LogProcessorInfo("Value at (0,0) is: " << valueat00);
    // You can assume that dims.z = 1 and do not need to consider others cases

    // TODO (Bonus) Gaussian filter
    // Our input is const, but you need to compute smoothed data and write it somewhere
    // Create an editable volume like this:
    // Volume volSmoothed(vol->getDimensions(), vol->getDataFormat());
    // auto vrSmoothed = volSmoothed.getEditableRepresentation<VolumeRAM>();
    // Values can be set with
    // vrSmoothed->setFromDouble(vec3(i,j,0), value);
    // getting values works with an editable volume as well
    // getInputValue(vrSmoothed, dims, 0, 0);

	
	// GAUSSIAN FILTER

	Volume volSmoothed(vol->getDimensions(), vol->getDataFormat());
	auto vrSmoothed = volSmoothed.getEditableRepresentation<VolumeRAM>();
	
	const int radius = 3; // radius x radius of neighbours to calculate gaussian smoothing
	float sigma = propSigma.get(); // Default value => 0.5f

	const VolumeRAM* myVolume = propApplyGaussian.get() ?  gaussianSmoothing(vr, vrSmoothed, dims, sigma, radius) : vr;


    // Grid

    // Properties are accessed with propertyName.get() 
    if (propShowGrid.get())
    {
        // TODO: Add grid lines of the given color 

        // The function drawLineSegments creates two vertices at the specified positions, 
        // that are placed into the Vertex vector defining our mesh. 
        // An index buffer specifies which of those vertices should be grouped into to make up lines/trianges/quads.
        // Here two vertices make up a line segment.
        auto indexBufferGrid = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);

        // Draw a line segment from v1 to v2 with a color, the coordinates in the final 
        // image range from 0 to 1 for both x and y
        //vec2 v1 = vec2(0.5, 0.5);
        //vec2 v2 = vec2(0.7, 0.7);
        //drawLineSegment(v1, v2, propGridColor.get(), indexBufferGrid, vertices);

		for (size_t i = 0; i < dims.x; i++)
		{
			float ix = 1.0 / (dims.x - 1) * i;
			drawLineSegment(vec2(ix, 0), vec2(ix, 1), propGridColor.get(), indexBufferGrid, vertices);
		}

		for (size_t j = 0; j < dims.y; j++)
		{
			float iy = 1.0 / (dims.y - 1) * j;
			drawLineSegment(vec2(0, iy), vec2(1, iy), propGridColor.get(), indexBufferGrid, vertices);
		}
		
    }

    // Iso contours

    if (propMultiple.get() == 0)
    {
        // TODO: Draw a single isoline at the specified isovalue (propIsoValue) 
        // and color it with the specified color (propIsoColor)

		auto isoBufferGrid = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
		drawIsolineSingleValue(propIsoValue, propIsoColor.get(), myVolume, dims, isoBufferGrid, vertices);
    }
    else
    {
        // TODO: Draw the given number (propNumContours) of isolines between 
        // the minimum and maximum value

		float w = (propIsoValue.getMaxValue() - propIsoValue.getMinValue()) / (propNumContours.get() + 1);

		for (size_t iso = 0; iso < propNumContours.get(); iso++)
		{
			const double isoVal = propIsoValue.getMinValue() + (iso + 1) * w;
			double isoValNormalized = (isoVal - propIsoValue.getMinValue()) / (propIsoValue.getMaxValue() - propIsoValue.getMinValue());

			const vec4& color = propIsoTransferFunc.get().sample(isoValNormalized);

			drawIsolineSingleValue(isoVal, color, myVolume, dims, mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None), vertices);
		}
        
        // TODO (Bonus): Use the transfer function property to assign a color
        // The transfer function normalizes the input data and sampling colors
        // from the transfer function assumes normalized input, that means
        // vec4 color = propIsoTransferFunc.get().sample(0.0f);
        // is the color for the minimum value in the data
        // vec4 color = propIsoTransferFunc.get().sample(1.0f);
        // is the color for the maximum value in the data

    }

    // Note: It is possible to add multiple index buffers to the same mesh,
    // thus you could for example add one for the grid lines and one for
    // each isoline
    // Also, consider to write helper functions to avoid code duplication
    // e.g. for the computation of a single iso contour

    mesh->addVertices(vertices);
    meshOut.setData(mesh);
}

VolumeRAM* MarchingSquares::gaussianSmoothing(const VolumeRAM* vr, VolumeRAM* vrSmoothed, const size3_t dims, float sigma, const int radius)
{
	// for every cell
	for (int x = 0; x < dims.x; x++) {
		for (int y = 0; y < dims.y; y++) {

			double total = 0.0;
			double totalFilter = 0.0;

			// apply filter , but consider dimension boundaries
			int startX = (x - radius < 0) ? 0 : x - radius;
			int endX = (x + radius > dims.x - 1) ? dims.x - 1 : x + radius;
			int startY = (y - radius < 0) ? 0 : y - radius;
			int endY = (y + radius > dims.y - 1) ? dims.y - 1 : y + radius;

			// TODO: Not very nice, as we do not save values and recalculate for every cell.. (should do DYNAMIC PROGRAMMING)
			for (int i = startX; i <= endX; i++) {
				for (int j = startY; j <= endY; j++) {
					// Calculate kernel-value by applying gaussian(x,y) centered at current cell
					double filterVal = gaussian(i - x, j - y, sigma);
					totalFilter += filterVal;
					total += getInputValue(vr, dims, i, j) * filterVal;
				}
			}
			// Divide by total number of cells we include to get average
			vrSmoothed->setFromDouble(vec3(x, y, 0), total / totalFilter * 1.0);

			//LogProcessorInfo("Previous " << getInputValue(vr, dims, x, y) << " Smoothed " << getInputValue(vrSmoothed, dims, x, y) << " Applied " << total / totalFilter << "  "
			//	<< (endX - startX)*(endY - startY));
		}
	}
	return vrSmoothed;
}

void MarchingSquares::drawIsolineSingleValue(const double c, const vec4& color, const VolumeRAM* vr, const size3_t dims, IndexBufferRAM* isoBufferGrid,
	std::vector<BasicMesh::Vertex>& vertices)
{
	for (float ix = 0; ix < (dims.x - 1); ix++)
	{
		for (float iy = 0; iy < (dims.y - 1); iy++)
		{
			float f00 = getInputValue(vr, dims, ix, iy);
			float f01 = getInputValue(vr, dims, ix, iy + 1);
			float f11 = getInputValue(vr, dims, ix + 1, iy + 1);
			float f10 = getInputValue(vr, dims, ix + 1, iy);

			float fmin = std::min({ f00, f01, f10, f11 });
			float fmax = std::max({ f00, f01, f10, f11 });
			if (fmin < c && fmax > c) // There is a isoline in this cell
			{
				drawSingleIsoline(ix, iy, c, color, vr, dims, isoBufferGrid, vertices);
			}
		}
	}
}

void MarchingSquares::drawSingleIsoline(float ix, float iy, const double c, const vec4& color, const VolumeRAM* vr, const size3_t dims, IndexBufferRAM* isoBufferGrid,
	std::vector<BasicMesh::Vertex>& vertices)
{
	float f00 = getInputValue(vr, dims, ix, iy);
	float f01 = getInputValue(vr, dims, ix, iy + 1);
	float f11 = getInputValue(vr, dims, ix + 1, iy + 1);
	float f10 = getInputValue(vr, dims, ix + 1, iy);

	vec2 coord[] = { vec2(ix,iy),vec2(ix,iy+1),vec2(ix+1,iy+1),vec2(ix+1,iy) };
	vec2 dir[] = { vec2(0,1), vec2(1,0), vec2(0,-1), vec2(-1,0) }; // This is a bit messy but basically it shows in which direction we have 
																	// to add the 'x' for each of the cases
	float f[] = { f00,f01,f11,f10 };

	std::vector<vec2> p;
	float f1, f2;

	for (size_t it = 0; it < 4; it++)
	{
		if (it < 3)
		{
			f1 = f[it];
			f2 = f[it + 1];
		}
		else
		{
			f1 = f[it];
			f2 = f[0];
		}
		if ((f1 < c && f2 >= c) || (f2 < c && f1 >= c))
		{
			float x = (c - f1) / (f2 - f1); //Compute interpolation
			vec2 pIso = vec2((coord[it].x + x*dir[it].x) / (dims.x - 1), (coord[it].y + x*dir[it].y) / (dims.y - 1));
			p.push_back(pIso);
		}
	}

	// Draw line
	int count = p.size();
	if (count == 2)
	{
		drawLineSegment(p[0], p[1], color, isoBufferGrid, vertices);
	}
	else if (count == 4) //Ambiguity!
	{
		if (propDeciderType.get() == 0) // Midpoint decider:
		{
			float fc = 0.25*(f00 + f01 + f10 + f11);

			if ((fc < c) && (f00 < c) || ((fc >= c) && (f00 >= c)))
			{
				drawLineSegment(p[0], p[1], color, isoBufferGrid, vertices);
				drawLineSegment(p[2], p[3], color, isoBufferGrid, vertices);
			}
			else
			{
				drawLineSegment(p[0], p[3], color, isoBufferGrid, vertices);
				drawLineSegment(p[1], p[2], color, isoBufferGrid, vertices);
			}
		}
		else // Asymptotic decider:
		{
			float fab = (f00 * f11 - f10 * f01) / (f11 + f00 - f01 - f10); // There was a faster solution by sorting the values...
			if (((c >= fab) && (f00 >= c)) || ((c < fab) && (f00 < c)))
			{
				drawLineSegment(p[0], p[3], color, isoBufferGrid, vertices);
				drawLineSegment(p[1], p[2], color, isoBufferGrid, vertices);
			}
			else
			{
				drawLineSegment(p[0], p[1], color, isoBufferGrid, vertices);
				drawLineSegment(p[2], p[3], color, isoBufferGrid, vertices);
			}
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

const double MarchingSquares::gaussian(int x, int y, float sigma) {
	return (1.0 / (2.0*sigma*M_PI) * exp(-(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2))));
}

} // namespace
