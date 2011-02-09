#include "Height.h"
#include "../../ZoneServer/TerrainManager.h"

class AHCN : public TRNLib::Height
{
public:
	AHCN(unsigned char* data, unsigned int dataSize);

	int   transform_type;
	float height_val;

	float getBaseHeight(float x, float z, TerrainManager* tm);
};