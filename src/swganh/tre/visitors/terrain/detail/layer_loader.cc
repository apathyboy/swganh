// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#include "layer_loader.h"

#include <swganh/byte_buffer.h>

#include "container_layer.h"
#include "boundary_circle.h"
#include "boundary_polygon.h"
#include "boundary_polyline.h"
#include "boundary_rectangle.h"
#include "filter_fractal.h"
#include "filter_height.h"
#include "filter_slope.h"
#include "height_constant.h"
#include "height_fractal.h"

using namespace swganh::tre;

/** 
	This class is used to load a particular implementation based on it's node name.
	We should consider using a map here instead as this is slightly inefficient.
*/
std::shared_ptr<Layer> LayerLoader(std::string node_name)
{
	std::shared_ptr<Layer> layer = nullptr;
	if(node_name == "LAYRFORM")
		layer = std::make_shared<ContainerLayer>();
	else if(node_name == "BCIRFORM")
		layer = std::make_shared<BoundaryCircle>();
	else if(node_name == "BPOLFORM")
		layer = std::make_shared<BoundaryPolygon>();
	else if(node_name == "BPLNFORM")
		layer = std::make_shared<BoundaryPolyline>();
	else if(node_name == "BRECFORM")
		layer = std::make_shared<BoundaryRectangle>();
	else if(node_name == "FFRAFORM")
		layer = std::make_shared<FractalFilter>();
	else if(node_name == "FHGTFORM")
		layer = std::make_shared<HeightFilter>();
	else if(node_name == "FSLPFORM")
		layer = std::make_shared<SlopeFilter>();
	else if(node_name == "AHCNFORM")
		layer = std::make_shared<HeightConstant>();
	else if(node_name == "AHFRFORM")
		layer = std::make_shared<HeightFractal>();

	return layer;
}