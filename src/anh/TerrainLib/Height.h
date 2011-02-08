#ifndef ANH_TERRAIN_HEIGHT_H_
#define ANH_TERRAIN_HEIGHT_H_

#include "layer.h"
#include "../../ZoneServer/TerrainManager.h"

namespace TRNLib
{
	class Height : public TRNLib::LAYER
	{
	public:
		virtual float getBaseHeight(float x, float z, TerrainManager* tm) = 0;
	};
};
#endif
