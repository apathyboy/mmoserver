#include "Boundary.h"

class BPOL : public TRNLib::Boundary
{
public:
	BPOL(unsigned char* data, unsigned int dataSize);
	
	std::vector<TRNLib::VERTEX*> verts;

	int feather_type;
	float shore_smoothness;
	int use_water_height;
	float water_height;
	float water_shader_size;
	unsigned char* water_shader;

	bool isContained(float x, float z);
};