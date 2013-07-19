// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <map>
#include "swganh/byte_buffer.h"

#include "fractal.h"
#include "layer.h"

namespace swganh
{
namespace tre
{
	class Fractal;

    class FilterLayer : public Layer
    {
    public:

		virtual LayerType GetType() { return LAYER_TYPE_FILTER; }

		virtual float Process(float x, float z, float transform_value, float& base_value, FractalMap& fractals) = 0;
		
		int   feather_type;
        float feather_amount;
    };
}
}
