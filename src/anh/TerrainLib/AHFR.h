#include "Height.h"
#include "../../ZoneServer/TerrainManager.h"

class AHFR : public TRNLib::Height
{
public:
	AHFR(unsigned char* data, unsigned int dataSize);

	int   fractal_id;
	int   transform_type;
	float height;

	float getBaseHeight(float x, float z, TerrainManager* tm);
};