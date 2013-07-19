// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include "height_layer.h"

#include "fractal.h"

namespace swganh
{
namespace tre
{

	/**
		Sets the height for the parent container to a particular fractally generated value.
	*/
	class HeightFractal : public HeightLayer
	{
	public:

		virtual void Deserialize(swganh::ByteBuffer& buffer);
		
		virtual void GetBaseHeight(float x, float z, float transform_value, float& base_value, FractalMap& fractals);

	private:
		int   fractal_id;
		int   transform_type;
		float height_val;
	};

}
}
