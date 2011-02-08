#ifndef ANH_TERRAIN_BOUNDARY_H_
#define ANH_TERRAIN_BOUNDARY_H_

#include "Layer.h"

namespace TRNLib
{
	class Boundary : public LAYER
	{
	public:
		virtual bool isContained(float x, float z) = 0;
	};
};

#endif