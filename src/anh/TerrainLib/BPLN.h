#include "Boundary.h"

class BPLN : public TRNLib::Boundary
{
public:
	BPLN(unsigned char* data, unsigned int dataSize);
	
	std::vector<TRNLib::VERTEX*> verts;

	int feather_type;
	float feather_amount;
	float line_width;

	bool isContained(float x, float z);
};