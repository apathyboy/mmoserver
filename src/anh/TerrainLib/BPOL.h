#include "Layer.h"

class BPOL : public TRNLib::LAYER
{
public:
	BPOL(unsigned char* data, unsigned int dataSize);
	
	std::vector<TRNLib::VERTEX*> verts;

	int feather_type;
	float shore_smoothness;
	int is_water;
	float water_height;
	float water_shader_size;
	unsigned char* water_shader;
};